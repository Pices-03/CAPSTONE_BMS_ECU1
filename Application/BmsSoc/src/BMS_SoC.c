/*==================================================================================================
* Project : BMS AUTOSAR Demo
* Platform : CORTEXM
* Component : BMS_SoC
* Module : BMS_SoC.c
* Description : State of Charge (SoC) estimation -- Coulomb counting with signed
*               bidirectional current từ INA219 (real 1S Li-ion pack).
*==================================================================================================*/

/*==================================================================================================
* INCLUDE FILES
==================================================================================================*/
#include "BMS_SoC.h"
#include "Gpt.h"

/*==================================================================================================
* DEFINITIONS
==================================================================================================*/
#define TIMER_FREQ_HZ       (8000000U) /* GPT channel 1 tick frequency (8 MHz) */

/*==================================================================================================
* LOCAL VARIABLES (MODULE STATE)
==================================================================================================*/
static BMS_SoC_StateType BMS_SoC_State =
{
    .CurrentSoC          = 0.0f,
    .RemainingCapacity_mAh = 0.0f,
    .NominalCapacity_mAh   = BMS_NOMINAL_CAPACITY_MAH,
    .IsCharging          = FALSE,
    .LowSoCWarning       = FALSE
};

static uint32 g_timePrev  = 0U; /* Previous GPT tick for delta time */
static uint8  g_firstCall = 1U; /* First-call flag để khởi g_timePrev */

/*==================================================================================================
* LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static void BMS_SoC_LimitAndUpdateWarning(void);

/*==================================================================================================
* FUNCTION IMPLEMENTATIONS
==================================================================================================*/

/**
 * @brief  Get elapsed time in seconds since last call, using GPT timer.
 *         Handles 32-bit timer overflow and initialises on first call.
 * @return Elapsed time (s). 0.0f on first call.
 */
float32 BMS_GetDeltaTime(void)
{
    uint32  timeNow;
    uint32  elapsedTicks;
    float32 deltaSec = 0.0f;

    timeNow = Gpt_GetTimeElapsed(GptConf_GptChannelConfiguration_GptChannelConfiguration_1);

    if (g_firstCall)
    {
        g_firstCall = 0U;
        g_timePrev  = timeNow;
    }
    else
    {
        if (timeNow >= g_timePrev)
        {
            elapsedTicks = timeNow - g_timePrev;
        }
        else
        {
            elapsedTicks = (0xFFFFFFFFU - g_timePrev) + timeNow;
        }

        deltaSec   = (float32)elapsedTicks / TIMER_FREQ_HZ;
        g_timePrev = timeNow;
    }

    return deltaSec;
}

/**
 * @brief Initialise the SoC module với SOC ban đầu (lấy từ NVM).
 */
Std_ReturnType BMS_SoC_Init(float32 initialSoC_Percent)
{
    Std_ReturnType Status = E_OK;

    if (initialSoC_Percent < 0.0f || initialSoC_Percent > 100.0f)
    {
        Status = E_NOT_OK;
    }
    else
    {
        BMS_SoC_State.CurrentSoC          = initialSoC_Percent;
        BMS_SoC_State.NominalCapacity_mAh   = BMS_NOMINAL_CAPACITY_MAH;
        BMS_SoC_State.RemainingCapacity_mAh = (initialSoC_Percent / 100.0f) * BMS_NOMINAL_CAPACITY_MAH;
        BMS_SoC_State.IsCharging          = FALSE;
        BMS_SoC_State.LowSoCWarning       = (BMS_SoC_State.CurrentSoC < BMS_SOC_WARNING_THRESHOLD);
    }

    return Status;
}

/**
 * @brief Update SoC bằng Coulomb counting với dòng signed bidirectional.
 * @param current_mA  Dòng INA219 (mA). Dương = xả, âm = sạc.
 */
Std_ReturnType BMS_SoC_Update(float32 current_mA)
{
    Std_ReturnType Status = E_OK;
    float32        deltaTime_sec;
    float32        delta_mAh;

    deltaTime_sec = BMS_GetDeltaTime();

    if (deltaTime_sec <= 0.0f)
    {
        Status = E_NOT_OK;
    }
    else
    {
        /* Dead zone quanh 0 A để chống nhiễu ADC khi idle */
        if ((current_mA < (BMS_CURRENT_DEAD_ZONE_mA)) &&
            (current_mA > -(BMS_CURRENT_DEAD_ZONE_mA)))
        {
            current_mA = 0.0f;
        }

        BMS_SoC_State.IsCharging = (current_mA < 0.0f);

        /* Coulomb counting: ΔmAh = I (mA) × Δt (s) / 3600 (s/mAh) */
        delta_mAh = current_mA * deltaTime_sec / 3600.0f;

        /* Discharge subtracts, charge adds (current âm → -delta_mAh dương) */
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

        if (BMS_SoC_State.CurrentSoC < 0.0f)   { BMS_SoC_State.CurrentSoC = 0.0f; }
        if (BMS_SoC_State.CurrentSoC > 100.0f) { BMS_SoC_State.CurrentSoC = 100.0f; }

        BMS_SoC_State.LowSoCWarning = (BMS_SoC_State.CurrentSoC < BMS_SOC_WARNING_THRESHOLD);
    }

    return Status;
}

/**
 * @brief Get current SoC percentage (rounded).
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
 * @brief Get remaining capacity in mAh.
 */
float32 BMS_SoC_GetRemaining_mAh(void)
{
    return BMS_SoC_State.RemainingCapacity_mAh;
}

/**
 * @brief Estimate remaining hours (xả) hoặc time to full charge (sạc).
 *        Dòng vào là signed: >0 = xả, <0 = sạc.
 */
float32 BMS_SoC_GetRemainingHours(float32 current_mA)
{
    float32 remainingHours = 0.0f;
    float32 needed_mAh;

    if (current_mA > 0.0f)  /* Discharging */
    {
        if (BMS_SoC_State.RemainingCapacity_mAh <= 0.0f)
        {
            remainingHours = -1.0f;
        }
        else
        {
            remainingHours = BMS_SoC_State.RemainingCapacity_mAh / current_mA;
        }
    }
    else if (current_mA < 0.0f)  /* Charging */
    {
        needed_mAh = BMS_SoC_State.NominalCapacity_mAh - BMS_SoC_State.RemainingCapacity_mAh;
        if (needed_mAh <= 0.0f)
        {
            remainingHours = 0.0f;
        }
        else
        {
            remainingHours = needed_mAh / (-current_mA);
        }
    }
    else
    {
        remainingHours = 1e6f;
    }

    return remainingHours;
}

/**
 * @brief Check if low SoC warning is active.
 */
boolean BMS_SoC_IsChargeWarning(void)
{
    return BMS_SoC_State.LowSoCWarning;
}

/*==================================================================================================
* LOCAL FUNCTIONS
==================================================================================================*/

/**
 * @brief Clamp RemainingCapacity vào [0, Nominal] và refresh LowSoCWarning.
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
