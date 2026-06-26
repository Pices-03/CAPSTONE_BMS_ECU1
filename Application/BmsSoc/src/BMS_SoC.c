/**
 * @file       BMS_SoC.c
 * @version    2.0.0
 * @brief      State-of-Charge (SoC) estimation -- Coulomb counting with
 *             signed bidirectional current from INA219 (real 1S Li-ion).
 */

/*******************************************************************************
* Includes
*******************************************************************************/
#include "BMS_SoC.h"
#include "Gpt.h"

/*******************************************************************************
* Definitions
*******************************************************************************/

/**
 * @brief   GPT channel 1 tick frequency (8 MHz).
 */
#define TIMER_FREQ_HZ       (8000000U)

/**
 * @brief   Overflow mask for the 32-bit free-running timer counter.
 */
#define TIMER_MAX_TICKS     (0xFFFFFFFFU)

/*******************************************************************************
* Prototypes
*******************************************************************************/

static void BMS_SoC_LimitAndUpdateWarning(void);

/*******************************************************************************
* Variables
*******************************************************************************/

/**
 * @brief   Module state (SoC, capacity, flags).
 */
static BMS_SoC_StateType BMS_SoC_State =
{
    .CurrentSoC            = 0.0f,
    .RemainingCapacity_mAh = 0.0f,
    .NominalCapacity_mAh   = BMS_NOMINAL_CAPACITY_MAH,
    .IsCharging            = FALSE,
    .LowSoCWarning         = FALSE
};

/**
 * @brief   Previous GPT tick captured by BMS_GetDeltaTime.
 */
static uint32 s_timePrev  = 0U;

/**
 * @brief   First-call flag used to seed s_timePrev on the very first call.
 */
static uint8  s_firstCall = 1U;

/* Thêm biến moving average cho dòng điện */
static volatile float32 s_avgCurrent_mA = 0.0f;
static volatile float32 s_currentSum_mA = 0.0f;
static volatile uint16  s_currentCount = 0U;

/*******************************************************************************
* Code
*******************************************************************************/

/**
 * @brief  Get elapsed time in seconds since the last call, via the GPT timer.
 *         Handles 32-bit timer overflow and initialises on the first call.
 * @return Elapsed time (s). 0.0f on the first call.
 */
float32 BMS_GetDeltaTime(void)
{
    uint32  timeNow      = 0U;
    uint32  elapsedTicks = 0U;
    float32 deltaSec     = 0.0f;

    timeNow = Gpt_GetTimeElapsed(GptConf_GptChannelConfiguration_GptChannelConfiguration_1);

    if (s_firstCall)
    {
        s_firstCall = 0U;
        s_timePrev  = timeNow;
    }
    else
    {
        if (timeNow >= s_timePrev)
        {
            elapsedTicks = timeNow - s_timePrev;
        }
        else
        {
            elapsedTicks = (TIMER_MAX_TICKS - s_timePrev) + timeNow;
        }

        deltaSec   = (float32)elapsedTicks / TIMER_FREQ_HZ;
        s_timePrev = timeNow;
    }

    return deltaSec;
}

/**
 * @brief See BMS_SoC.h.
 */
Std_ReturnType BMS_SoC_Init(float32 initialSoC_Percent)
{
    Std_ReturnType Status = E_OK;

    if ((initialSoC_Percent < 0.0f) || (initialSoC_Percent > 100.0f))
    {
        Status = E_NOT_OK;
    }
    else
    {
        BMS_SoC_State.CurrentSoC            = initialSoC_Percent;
        BMS_SoC_State.NominalCapacity_mAh   = BMS_NOMINAL_CAPACITY_MAH;
        BMS_SoC_State.RemainingCapacity_mAh = (initialSoC_Percent / 100.0f) * BMS_NOMINAL_CAPACITY_MAH;
        BMS_SoC_State.IsCharging            = FALSE;
        BMS_SoC_State.LowSoCWarning         = (BMS_SoC_State.CurrentSoC < BMS_SOC_WARNING_THRESHOLD);
    }

    return Status;
}

/**
 * @brief See BMS_SoC.h.
 */
Std_ReturnType BMS_SoC_Update(float32 current_mA)
{
    Std_ReturnType Status        = E_OK;
    float32        deltaTime_sec = 0.0f;
    float32        delta_mAh     = 0.0f;

    deltaTime_sec = BMS_GetDeltaTime();

    if (deltaTime_sec <= 0.0f)
    {
        Status = E_NOT_OK;
    }
    else
    {
        /* Dead-zone around 0 mA to suppress ADC noise when idle. */
        if ((current_mA < BMS_CURRENT_DEAD_ZONE_mA) &&
            (current_mA > -BMS_CURRENT_DEAD_ZONE_mA))
        {
            current_mA = 0.0f;
        }

        s_currentSum_mA += current_mA;
        s_currentCount++;

        if (s_currentCount >= BMS_AVG_CURRENT_SAMPLES)
        {
            s_avgCurrent_mA = s_currentSum_mA / (float32)s_currentCount;
            s_currentSum_mA = 0.0f;
            s_currentCount = 0U;
        }

        BMS_SoC_State.IsCharging = (current_mA < 0.0f);

        /* Coulomb counting: delta_mAh = I (mA) * delta_t (s) / 3600 (s/h). */
        delta_mAh = (current_mA * deltaTime_sec) / 3600.0f;

        /* Discharge subtracts capacity, charge adds (current < 0 => -delta > 0). */
        BMS_SoC_State.RemainingCapacity_mAh -= delta_mAh;

        BMS_SoC_LimitAndUpdateWarning();

        if (BMS_SoC_State.NominalCapacity_mAh > 0.0f)
        {
            BMS_SoC_State.CurrentSoC =
                (BMS_SoC_State.RemainingCapacity_mAh / BMS_SoC_State.NominalCapacity_mAh) * 100.0f;
        }
        else
        {
            BMS_SoC_State.CurrentSoC = 0.0f;
        }

        if (BMS_SoC_State.CurrentSoC < 0.0f)
        {
            BMS_SoC_State.CurrentSoC = 0.0f;
        }
        else if (BMS_SoC_State.CurrentSoC > 100.0f)
        {
            BMS_SoC_State.CurrentSoC = 100.0f;
        }

        BMS_SoC_State.LowSoCWarning = (BMS_SoC_State.CurrentSoC < BMS_SOC_WARNING_THRESHOLD);
    }

    return Status;
}

/**
 * @brief See BMS_SoC.h.
 */
uint8 BMS_SoC_Get(void)
{
    float32 intSoC = BMS_SoC_State.CurrentSoC;

    if (intSoC > 100.0f)
    {
        intSoC = 100.0f;
    }
    if (intSoC < 0.0f)
    {
        intSoC = 0.0f;
    }

    return (uint8)(intSoC + 0.5f);
}

/**
 * @brief See BMS_SoC.h.
 */
float32 BMS_SoC_GetRemaining_mAh(void)
{
    return BMS_SoC_State.RemainingCapacity_mAh;
}

/**
 * @brief See BMS_SoC.h.
 */
float32 BMS_SoC_GetRemainingHours(float32 current_mA)
{
    float32 remainingHours = 0.0f;
    float32 needed_mAh     = 0.0f;
    float32 currentToUse;

    if (s_currentCount > 0 && s_avgCurrent_mA > 0.0f)
    {
        currentToUse = s_avgCurrent_mA;
    }
    else
    {
        currentToUse = current_mA;
    }

    if (currentToUse > 0.0f)        /* Discharging */
    {
        if (BMS_SoC_State.RemainingCapacity_mAh <= 0.0f)
        {
            remainingHours = -1.0f;
        }
        else
        {
            /* mAh / mA = hours */
            remainingHours = BMS_SoC_State.RemainingCapacity_mAh / currentToUse;
        }
    }
    else if (currentToUse < 0.0f)   /* Charging */
    {
        needed_mAh = BMS_SoC_State.NominalCapacity_mAh - BMS_SoC_State.RemainingCapacity_mAh;
        if (needed_mAh <= 0.0f)
        {
            remainingHours = 0.0f;
        }
        else
        {
            remainingHours = needed_mAh / (-currentToUse);
        }
    }
    else                          /* Approximately zero current */
    {
        remainingHours = 1e6f;
    }

    return remainingHours;
}

/**
 * @brief See BMS_SoC.h.
 */
boolean BMS_SoC_IsChargeWarning(void)
{
    return BMS_SoC_State.LowSoCWarning;
}

/**
 * @brief Clamp RemainingCapacity_mAh into [0, Nominal] and refresh LowSoCWarning.
 */
static void BMS_SoC_LimitAndUpdateWarning(void)
{
    if (BMS_SoC_State.RemainingCapacity_mAh < 0.0f)
    {
        BMS_SoC_State.RemainingCapacity_mAh = 0.0f;
    }
    if (BMS_SoC_State.RemainingCapacity_mAh > BMS_SoC_State.NominalCapacity_mAh)
    {
        BMS_SoC_State.RemainingCapacity_mAh = BMS_SoC_State.NominalCapacity_mAh;
    }
}
