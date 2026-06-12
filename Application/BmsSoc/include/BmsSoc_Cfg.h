/**
 * @file       BmsSoc_Cfg.h
 * @version    2.0.0
 * @brief      Compile-time configuration for the BMS SOC module (real-pack).
 *
 * @details    The demo mode (resistor rig + unidirectional INA219) has been
 *             dropped. The system now runs against a real 1S Li-ion pack with
 *             a bidirectional supply, so INA219 reports a signed current
 *             natively (positive = discharge, negative = charge). No virtual
 *             mapping is required.
 *
 *             The initial SOC is read from NVM (BMS_Nvm_ReadSoC) by
 *             BmsApp_Init, so the legacy hard-coded BMS_SOC_INITIAL_PCT macro
 *             has been removed.
 */

#ifndef _BMSSOC_CFG_
#define _BMSSOC_CFG_

#ifdef __cplusplus
extern "C" {
#endif

#include "Std_Types.h"

/*******************************************************************************
* Definitions
*******************************************************************************/

/**
 * @brief   Nominal pack capacity in milliamp-hours.
 * @details Real 1S Li-ion cell, 2000 mAh. Must stay in sync with
 *          BMS_NOMINAL_CAPACITY_MAH (= 2000.0f) in BMS_SoC.h.
 */
#define BMS_SOC_NOMINAL_CAPACITY_MAH (2000U)

/**
 * @brief   Dead-zone current magnitude (mA).
 * @details |I| below 5 mA is treated as zero, to suppress ADC noise around
 *          the idle (no-load) operating point.
 */
#define BMS_SOC_CURRENT_DEAD_ZONE_MA (5.0f)

/**
 * @brief   Coulomb-counting tick period (milliseconds).
 * @details Must match the BmsScheduler tick period -- BmsSoc_Update() is
 *          called once per 50 ms slot.
 */
#define BMS_SOC_TICK_MS              (50U)

#ifdef __cplusplus
}
#endif

#endif /* _BMSSOC_CFG_ */
