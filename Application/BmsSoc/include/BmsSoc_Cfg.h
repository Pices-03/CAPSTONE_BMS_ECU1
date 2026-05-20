/**
 * @file       BmsSoc_Cfg.h
 * @version    1.0.0
 * @brief      Compile-time configuration for the BMS SOC module.
 *
 * @details    Splits configuration from logic so that swapping demo mode for a
 *             real-battery deployment requires only a macro flip in this file
 *             (and a hardware change). All values here are demo-tuned for the
 *             current bench setup: S32K144 3V3 + resistive load, no real pack.
 *
 * @note       BMS_SOC_DEMO_MODE = STD_ON   -> Coulomb counting only.
 *                                              Initial SOC seeded from
 *                                              BMS_SOC_INITIAL_PCT (hardcode).
 *                                              OCV lookup table is dead code
 *                                              (compiled out).
 *             BMS_SOC_DEMO_MODE = STD_OFF  -> Real 1S Li-ion cell.
 *                                              Initial SOC seeded from OCV
 *                                              lookup at boot voltage.
 *                                              BMS_SOC_INITIAL_PCT becomes the
 *                                              fallback if OCV read fails.
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
 * @brief   Demo-mode switch.
 * @details Demo bench uses S32K144 3.3 V rail through a fixed resistor; the
 *          measured voltage does NOT reflect a battery state of charge, so
 *          OCV lookup must be disabled. Set to STD_OFF when migrating to a
 *          real Li-ion pack.
 */
#define BMS_SOC_DEMO_MODE            (STD_ON)

/**
 * @brief   Initial SOC at boot (percent, 0 - 100).
 * @details Demo: used directly as the Coulomb-counter seed.
 *          Real pack: used as a fallback if OCV read fails (e.g. I2C error).
 */
#define BMS_SOC_INITIAL_PCT          (95U)

/**
 * @brief   Virtual / nominal pack capacity (milliamp-hours).
 * @details Chosen small enough for the demo so that the visible SOC change
 *          is meaningful within minutes at typical demo load currents:
 *              100 mAh @  50 mA load -> full discharge in  2 hours
 *              100 mAh @ 200 mA load -> full discharge in 30 minutes
 *          When migrating to a real pack, override with the data-sheet value
 *          (e.g. 2000U for a 2 Ah 1S Li-ion cell).
 */
#define BMS_SOC_NOMINAL_CAPACITY_MAH (100U)

/**
 * @brief   Dead-zone current magnitude (mA, legacy float view).
 */
#define BMS_SOC_CURRENT_DEAD_ZONE_MA (5.0f)

/*******************************************************************************
* Unidirectional → Bidirectional virtual mapping (DEMO only)
*******************************************************************************/

/**
 * @brief   Bật mapping từ dòng 1 chiều (biến trở) sang dòng 2 chiều ảo.
 * @details Tắt khi gắn pin thật + nguồn 2 chiều → INA219 sẽ đo signed
 *          tự nhiên (positive = xả, negative = sạc). Bật khi demo với
 *          nguồn 5 V cố định + tải biến trở (INA219 chỉ đọc dòng dương).
 *
 *          STD_ON  : virtual_I = raw_I − ZERO_POINT
 *                    → biến trở nhỏ (dòng to)  ⇒ virtual dương ⇒ "xả"
 *                    → biến trở to  (dòng nhỏ) ⇒ virtual âm    ⇒ "sạc"
 *          STD_OFF : virtual_I = raw_I  (passthrough — pin thật)
 */
#define BMS_SOC_BIDIR_SIM_ENABLE     (STD_ON)

/**
 * @brief   Zero point (mA) — dòng tại đó coi là "không sạc / không xả".
 * @details Người vận hành chọn 1 vị trí cố định trên biến trở làm "rest"
 *          và đo giá trị dòng INA219 hiển thị, ghi vào macro này.
 *          Ví dụ: nguồn 5 V, biến trở ở 100 Ω → dòng 50 mA → zero = 50.
 *
 *          Có thể chỉnh runtime qua HMI cmd nếu sau này wire vào.
 */
#define BMS_SOC_BIDIR_ZERO_POINT_mA  (90.0f)

/**
 * @brief   Dead-zone quanh zero point (mA) — để tránh flicker chg/dchg.
 * @details |raw − ZERO_POINT| < DEAD → virtual_I = 0 → state = IDLE.
 *          Chọn nhỏ hơn nhiễu INA219 đo thực để không gây chệch SoC.
 */
#define BMS_SOC_BIDIR_DEAD_mA        (3.0f)

/**
 * @brief   Dead-zone current magnitude (uA, integer path).
 * @details |i_uA| below this is treated as 0. 5000 uA = 5 mA matches the
 *          float legacy value. Used by BmsSoc_Update integer path.
 */
#define BMS_SOC_DEAD_ZONE_uA         (5000)

/**
 * @brief   Nominal capacity expressed in nano-coulombs (integer path).
 * @details Conversion derivation:
 *            1 mAh = 1e-3 A * 3600 s = 3.6 C = 3.6e9 nC
 *          So: nominal_nC = mAh * 3,600,000,000.
 *          With 100 mAh demo: 3.6e11 nC -- requires sint64.
 *          With 2000 mAh real cell: 7.2e12 nC -- still well within sint64.
 *
 *          State value range fits comfortably in sint64 (max 9.2e18 nC,
 *          enough for ~2.5 billion Ah which is obviously absurd).
 */
#define BMS_SOC_NOMINAL_nC           ((sint64)BMS_SOC_NOMINAL_CAPACITY_MAH * 3600000000LL)

/**
 * @brief   Coulomb-counting tick period (milliseconds).
 * @details Must match BmsScheduler tick period -- BmsSoc_Update() is called
 *          once per 50 ms slot. Defined here so the integrator constant
 *          delta_t = TICK_MS / 1000 / 3600 stays a single source of truth.
 */
#define BMS_SOC_TICK_MS              (50U)

#ifdef __cplusplus
}
#endif

#endif /* _BMSSOC_CFG_H_ */
