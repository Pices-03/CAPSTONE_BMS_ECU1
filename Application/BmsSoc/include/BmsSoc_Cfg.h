/**
 * @file       BmsSoc_Cfg.h
 * @version    2.0.0
 * @brief      Compile-time configuration for the BMS SOC module (real-pack).
 *
 * @details    Demo mode (resistor rig + INA219 1 chiều) đã bị bỏ. Hiện chạy
 *             pin thật 1S Li-ion với nguồn 2 chiều → INA219 trả về dòng
 *             signed tự nhiên (positive = xả, negative = sạc), không cần
 *             mapping virtual.
 *
 *             SOC khởi tạo lấy từ NVM (BMS_Nvm_ReadSoC) trong BmsApp_Init.
 *             Không còn macro BMS_SOC_INITIAL_PCT hardcode.
 */

#ifndef _BMSSOC_CFG_H_
#define _BMSSOC_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Std_Types.h"

/*******************************************************************************
* Definitions
*******************************************************************************/

/**
 * @brief   Nominal pack capacity (milliamp-hours).
 * @details Pin thật 1S Li-ion 2000 mAh. Đồng bộ với BMS_NOMINAL_CAPACITY_AH
 *          (= 2.0f) trong BMS_SoC.h.
 */
#define BMS_SOC_NOMINAL_CAPACITY_MAH (2000U)

/**
 * @brief   Dead-zone current magnitude (mA).
 * @details |I| < 5 mA → coi như 0 để chống nhiễu ADC INA219 quanh idle.
 */
#define BMS_SOC_CURRENT_DEAD_ZONE_MA (5.0f)

/**
 * @brief   Coulomb-counting tick period (milliseconds).
 * @details Must match BmsScheduler tick period -- BmsSoc_Update() is called
 *          once per 50 ms slot.
 */
#define BMS_SOC_TICK_MS              (50U)

#ifdef __cplusplus
}
#endif

#endif /* _BMSSOC_CFG_H_ */
