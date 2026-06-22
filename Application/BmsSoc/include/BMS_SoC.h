/**
 * @file       BMS_SoC.h
 * @version    2.0.0
 * @brief      State-of-Charge (SoC) estimation -- Coulomb counting.
 *
 * @details    Real 1S Li-ion pack. INA219 returns a signed bidirectional
 *             current (positive = discharge, negative = charge), so no
 *             virtual mapping is required.
 */

#ifndef _BMS_SOC_
#define _BMS_SOC_

#ifdef __cplusplus
extern "C"
{
#endif

#include "Std_Types.h"

/*******************************************************************************
* Definitions
*******************************************************************************/

/**
 * @brief   Average current window for SoC estimation (seconds).
 */
#define BMS_AVG_CURRENT_WINDOW_SEC     (60U)   /* 1 phút */

/**
 * @brief   Average current window for SoC estimation (samples).
 */
#define BMS_AVG_CURRENT_SAMPLES        (1200U) /* 1 phút @ 50ms tick */

/**
 * @brief Nominal battery capacity in Milliampere-hours (mAh).
 * @details Real 1S Li-ion 2000 mAh. Must stay in sync with
 *          BMS_SOC_NOMINAL_CAPACITY_MAH in BmsSoc_Cfg.h.
 */
#define BMS_NOMINAL_CAPACITY_MAH        (2000.0f)

/**
 * @brief Low-SoC warning threshold (percent).
 */
#define BMS_SOC_WARNING_THRESHOLD       (5.0f)

/**
 * @brief Dead-zone around 0 A (mA) -- suppress ADC noise near idle.
 *        |I| below 5 mA is treated as zero (no integration into SoC).
 */
#define BMS_CURRENT_DEAD_ZONE_mA        (5.0f)

/**
 * @brief Structure holding the SoC module's state variables.
 */
typedef struct
{
    float32 CurrentSoC;            /* Current State of Charge (%)                  */
    float32 RemainingCapacity_mAh; /* Remaining capacity in Milliampere-hours (mAh)*/
    float32 NominalCapacity_mAh;   /* Nominal capacity in Milliampere-hours (mAh)  */
    boolean IsCharging;            /* TRUE while charging (current < 0)            */
    boolean LowSoCWarning;         /* TRUE when SoC < BMS_SOC_WARNING_THRESHOLD    */
} BMS_SoC_StateType;

/*******************************************************************************
* API
*******************************************************************************/

/**
 * @brief Get elapsed time in seconds since the last call, via GPT timer.
 */
float32 BMS_GetDeltaTime(void);

/**
 * @brief       Initialise the SoC module.
 * @param[in]   initialSoC_Percent  Initial SoC at start-up (0.0 - 100.0).
 * @return      E_OK on success, E_NOT_OK if the input is out of range.
 */
Std_ReturnType BMS_SoC_Init(float32 initialSoC_Percent);

/**
 * @brief       Update SoC using Coulomb counting.
 * @param[in]   current_mA  Signed current (mA). Positive = discharge,
 *                          negative = charge.
 * @return      E_OK on success, E_NOT_OK if deltaTime is non-positive.
 */
Std_ReturnType BMS_SoC_Update(float32 current_mA);

/**
 * @brief       Get the current State of Charge (rounded percent).
 * @return      SoC percentage in [0, 100].
 */
uint8 BMS_SoC_Get(void);

/**
 * @brief       Get the remaining capacity in Milliampere-hours.
 */
float32 BMS_SoC_GetRemaining_mAh(void);

/**
 * @brief       Estimate remaining hours (discharge) or time to full (charge).
 * @param[in]   current_mA  Signed current (mA). >0 = discharging, <0 = charging.
 * @return      Remaining hours. 1e6 if current is approximately zero.
 *              -1 if empty while still discharging.
 */
float32 BMS_SoC_GetRemainingHours(float32 current_mA);

/**
 * @brief       Check whether the low-SoC warning is active.
 */
boolean BMS_SoC_IsChargeWarning(void);

#ifdef __cplusplus
}
#endif

#endif /* _BMS_SOC_ */
