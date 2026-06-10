/*==================================================================================================
* Project : BMS AUTOSAR Demo
* Platform : CORTEXM
* Component : BMS_SoC
* Module : BMS_SoC.h
* Description : Header file for State of Charge (SoC) estimation module (Coulomb Counting).
*               Real 1S Li-ion pack -- INA219 trả về dòng signed bidirectional.
*==================================================================================================*/

#ifndef BMS_SOC_H
#define BMS_SOC_H

#ifdef __cplusplus
extern "C"
{
#endif

/*==================================================================================================
* INCLUDE FILES
==================================================================================================*/
#include "Std_Types.h"

/*==================================================================================================
* DEFINITIONS AND MACROS
==================================================================================================*/

/**
 * @brief Nominal battery capacity in Milliampere-hours (mAh).
 * @details For a 1S Li-ion pack, typical capacity is around 2000 mAh. Adjust as needed for different packs.
 */
#define BMS_NOMINAL_CAPACITY_MAH         (2000.0f)

/**
 * @brief Low SoC warning threshold in percent (5%).
 */
#define BMS_SOC_WARNING_THRESHOLD       (5.0f)

/**
 * @brief Dead zone for current measurement in mA to filter out noise when idle.
 *          Adjust based on ADC noise characteristics.
 */
#define BMS_CURRENT_DEAD_ZONE_mA      (5.0f)

/*==================================================================================================
* TYPEDEFS AND STRUCTURES
==================================================================================================*/

/**
 * @brief Structure to hold the SoC module's state variables.
 */
typedef struct
{
    float32 CurrentSoC;          /* Current State of Charge (%) */
    float32 RemainingCapacity_mAh; /* Remaining capacity in Milliampere-hours (mAh) */
    float32 NominalCapacity_mAh;   /* Nominal capacity in Milliampere-hours (mAh) */
    boolean IsCharging;          /* Flag to indicate charging state */
    boolean LowSoCWarning;       /* Flag to indicate low SoC warning */
} BMS_SoC_StateType;

/*==================================================================================================
* FUNCTION PROTOTYPES
==================================================================================================*/

/**
 * @brief Get elapsed time in seconds since last call, using GPT timer.
 */
float32 BMS_GetDeltaTime(void);

/**
 * @brief       Initialize the SoC module.
 * @param[in]   initialSoC_Percent  Initial SoC percentage upon startup (0.0 - 100.0).
 */
Std_ReturnType BMS_SoC_Init(float32 initialSoC_Percent);

/**
 * @brief       Update the SoC using Coulomb counting.
 * @param[in]   current_mA  Signed current (mA). Positive = discharge, negative = charge.
 */
Std_ReturnType BMS_SoC_Update(float32 current_mA);

/**
 * @brief       Get the current State of Charge.
 * @return      uint8  Current SoC in percentage (0 - 100).
 */
uint8 BMS_SoC_Get(void);

/**
 * @brief       Get the remaining capacity in Milliampere-hours.
 */
float32 BMS_SoC_GetRemaining_mAh(void);

/**
 * @brief       Get estimated remaining hours of operation (discharge) or time
 *              to full charge.
 * @param[in]   current_mA  Signed current (mA). >0 = discharging, <0 = charging.
 * @return      float32  Remaining hours. 1e6 if current ~0 (no change). -1 if
 *              empty while discharging.
 */
float32 BMS_SoC_GetRemainingHours(float32 current_mA);

/**
 * @brief       Check if the low SoC warning is active.
 */
boolean BMS_SoC_IsChargeWarning(void);

#ifdef __cplusplus
}
#endif

#endif /* BMS_SOC_H */
