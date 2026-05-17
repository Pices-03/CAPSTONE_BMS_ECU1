/**
 * @file       BmsSoc.h
 * @version    2.0.0
 * @brief      State-of-Charge (SOC) tracking and battery prediction (TTE/TTF).
 *
 * @details    Runtime algorithm: COULOMB COUNTING.
 *               BmsSoc_Init(boot_volt_mV)
 *                 -> seeds the internal Ah counter from BMS_SOC_INITIAL_PCT
 *                    (demo mode) or from OCV lookup (real-pack mode -- see
 *                    BMS_SOC_DEMO_MODE in BmsSoc_Cfg.h).
 *               BmsSoc_Update(curr_mA)
 *                 -> called once per 50 ms scheduler tick by BmsApp_Task50ms.
 *                    Integrates I*dt into the remaining-capacity counter and
 *                    re-derives the SOC raw value.
 *               BmsSoc_GetSocRaw()
 *                 -> returns the latest integrator state, encoded 0.5%/LSB
 *                    (0..200), as required by CAN 0x300 byte 0.
 *               BmsSoc_SetSocPct(pct)
 *                 -> re-seeds the counter (HMI command 0x02 "calib SOC").
 *
 *             Prediction (TTE/TTF) is a stateless transform from a snapshot
 *             of (V, I, P, SOC) into the CAN 0x400 SDU and is unchanged.
 *
 * @note       History: v1 used voltage-only OCV lookup which degraded under
 *             load (no IR-drop compensation). v2 switches to Coulomb counting
 *             with OCV retained as boot-seed only (real-pack mode). See
 *             project_context.md backlog items #17 - #19.
 */

#ifndef _BMSSOC_H_
#define _BMSSOC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Std_Types.h"
#include "BmsSoc_Cfg.h"

/*******************************************************************************
* Definitions
*******************************************************************************/

/**
 * @brief   SOC value when pack is fully charged (0.5% LSB encoding).
 */
#define BMS_SOC_FULL_RAW             (200U)

/**
 * @brief   SOC value when pack is fully discharged (0.5% LSB encoding).
 */
#define BMS_SOC_EMPTY_RAW            (0U)

/**
 * @brief   SOC method codes (CAN 0x300 byte 5).
 * @details Tells the HMI which algorithm produced the SOC value so it can
 *          display the correct label.
 */
#define BMS_SOC_METHOD_VOLTAGE       (0x00U)  /**< Voltage-only OCV lookup.       */
#define BMS_SOC_METHOD_COULOMB       (0x01U)  /**< Coulomb counting (v2 default). */

/**
 * @brief   Invalid prediction sentinel for TTE / TTF (1 minute LSB).
 */
#define BMS_PRED_INVALID             (0xFFFFU)

/**
 * @brief   Valid-flag bits for the prediction message (CAN 0x400 byte 5).
 */
#define BMS_PRED_VALID_TTE           (0x01U)
#define BMS_PRED_VALID_TTF           (0x02U)

/**
 * @brief   Output PDU size of CAN 0x400 (BMS_Prediction).
 */
#define BMS_PRED_DLC                 (6U)

/*******************************************************************************
* API
*******************************************************************************/

/**
 * @brief   Seed the SOC integrator at boot.
 *
 * @details Demo mode (BMS_SOC_DEMO_MODE == STD_ON):
 *            boot_volt_mV is IGNORED.
 *            Remaining capacity = BMS_SOC_INITIAL_PCT / 100 * NominalCapacity.
 *
 *          Real-pack mode (BMS_SOC_DEMO_MODE == STD_OFF):
 *            boot_volt_mV is looked up against the OCV table (1S Li-ion).
 *            If the lookup yields 0 % AND voltage < 2000 mV the result is
 *            considered an I2C / wiring error and the fallback
 *            BMS_SOC_INITIAL_PCT is used instead.
 *
 * @pre     CDD_INA219_Init() has been called (real-pack mode requires a
 *          valid bus voltage reading; demo mode requires nothing).
 *
 * @param[in] boot_volt_mV  Pack voltage at boot (mV). Demo mode ignores this.
 *
 * @return  void
 *
 * @post    Internal Ah counter and the cached SOC raw value reflect the seed.
 *
 * @note    Safe to call again at any time; the integrator state is reset.
 */
void BmsSoc_Init(uint16 boot_volt_mV);

/**
 * @brief   Integrate one scheduler tick (50 ms) of current draw.
 *
 * @details API giữ float (input từ INA219 driver). Internal chuyển sang
 *          integer 1 lần duy nhất tại biên rồi tích phân exact:
 *            i_uA      = (sint32)(curr_mA × 1000)         (1 float→int cast)
 *            delta_nC  = i_uA × BMS_SOC_TICK_MS           (1 SMULL, exact)
 *            s_remain_nC -= delta_nC                      (1 SUB, exact)
 *
 *          Sai số bị chặn ở 1 lần ULP/tick (cast float→int, max ~1 µA),
 *          KHÔNG tích lũy theo float-on-float arithmetic. Trên N tick:
 *          tổng drift <= N × LSB_int << drift float gốc trước đây.
 *
 *          Dead-zone trong micro-amps: |i_uA| < BMS_SOC_DEAD_ZONE_uA
 *          (5000 µA = 5 mA) được set 0 để chống nhiễu ADC quanh điểm
 *          không tải.
 *
 * @pre     BmsSoc_Init() has been called.
 *          Caller has freshly read INA219 current via CDD_INA219_ReadCurrent_mA.
 *
 * @param[in] curr_mA  Pack current in milliamps (signed, float từ INA219).
 *                      Positive = discharge.
 *                      Range: ±3276.7 mA (INA219 16-bit signed × 0.1 mA LSB).
 *
 * @return  void
 *
 * @post    s_remain_nC cập nhật và clamp về [0, BMS_SOC_NOMINAL_nC].
 *          s_socRaw rebuild, s_remain_mAh_dbg float mirror refresh.
 *
 * @note    Trước (v2): tích phân thuần float → ULP rounding tích lũy hàng
 *          triệu tick. Nay (v3): state là sint64 nC, chỉ cast float→int
 *          một lần ở biên input. Cân bằng giữa simplicity (giữ float ở
 *          biên cảm biến) và determinism (integer trong accumulator).
 */
void BmsSoc_Update(float32 curr_mA);

/**
 * @brief   Return the latest cached SOC raw value (0.5% per LSB).
 *
 * @details Reflects the integrator state as of the most recent BmsSoc_Update()
 *          call. Encoded as 0..200 to match CAN 0x300 byte 0.
 *
 * @pre     BmsSoc_Init() has been called.
 *
 * @param   void
 *
 * @return  SOC encoded in 0.5% LSB units, range [0, 200].
 *
 * @post    No side effects.
 */
uint8 BmsSoc_GetSocRaw(void);

/**
 * @brief   Re-seed the SOC integrator from an absolute percent value.
 *
 * @details Implements HMI command 0x710 / 0x02 ("calib SOC"). Values outside
 *          [0, 100] are clamped to the nearest bound.
 *
 * @pre     BmsSoc_Init() has been called.
 *
 * @param[in] soc_pct  New SOC value (0 - 100 %).
 *
 * @return  void
 *
 * @post    Internal Ah counter rebuilt from (soc_pct / 100) * NominalCapacity.
 *          Cached SOC raw value updated.
 */
void BmsSoc_SetSocPct(uint8 soc_pct);

/**
 * @brief   Map raw INA219 current (positive only, do demo dùng nguồn 5 V +
 *          biến trở) sang dòng 2 chiều ảo, để demo state CHG/DCHG/IDLE.
 *
 * @details Khi BMS_SOC_BIDIR_SIM_ENABLE == STD_ON:
 *            virtual_I = raw_I − BMS_SOC_BIDIR_ZERO_POINT_mA
 *            |virtual_I| < BMS_SOC_BIDIR_DEAD_mA → 0  (IDLE)
 *            virtual_I > 0 → demo "đang xả" (biến trở vặn nhỏ = dòng to)
 *            virtual_I < 0 → demo "đang sạc" (biến trở vặn to = dòng nhỏ)
 *          Khi BMS_SOC_BIDIR_SIM_ENABLE == STD_OFF (pin thật):
 *            return raw_mA passthrough.
 *
 *          Phải gọi function này cho mọi tiêu thụ "current" downstream
 *          (Coulomb counter, state byte, prediction) để demo nhất quán.
 *
 * @param[in] raw_mA  Dòng đo trực tiếp từ CDD_INA219_ReadCurrent_mA.
 * @return  Dòng 2 chiều ảo (signed), đơn vị mA.
 */
float32 BmsSoc_VirtualCurrent_mA(float32 raw_mA);

/**
 * @brief   Feed one instantaneous power sample (mW) into the 1-minute
 *          moving-average accumulator. Gọi mỗi tick 50 ms từ BmsApp_Task50ms.
 *
 * @details Block-average qua 1200 mẫu (1 phút @ 50 ms tick). Khi đủ 1200
 *          mẫu, snapshot trung bình rồi reset. Được dùng làm power
 *          divisor trong TTE / TTF prediction để giảm jitter.
 *
 * @param[in] inst_power_mW  Công suất tức thời (mW), thường lấy từ INA219.
 *
 * @return  void
 */
void BmsSoc_AccumulatePower(float32 inst_power_mW);

/**
 * @brief   Compute time-to-empty / time-to-full into a 6-byte SDU.
 *
 * @details Energy model:
 *            remain_Wh = DESIGN_CAP * SOC_fraction.
 *            TTE_min   = remain_Wh / power_W * 60   (only if curr > +50 mA).
 *            empty_Wh  = DESIGN_CAP - remain_Wh.
 *            TTF_min   = empty_Wh / chg_W * 60      (only if curr < -50 mA).
 *          Invalid TTE / TTF are written as BMS_PRED_INVALID; valid flags in
 *          byte 5 reflect which fields are meaningful.
 *
 * @pre     pOutSdu points to a buffer of at least BMS_PRED_DLC bytes.
 *          soc_raw is in [0, 200]. Caller has captured volt_V/curr_mA/pow_mW
 *          from a single 50 ms sample to keep the prediction self-consistent.
 *
 * @param[in]  volt_V    Pack voltage (V).
 * @param[in]  curr_mA   Current (mA, signed). Positive = discharge.
 * @param[in]  pow_mW    Instantaneous power (mW).
 * @param[in]  soc_raw   SOC encoded in 0.5% LSB.
 * @param[out] pOutSdu   Pointer to caller's CAN 0x400 SDU. Must not be NULL_PTR.
 *
 * @return  void
 *
 * @post    pOutSdu[0..5] populated as:
 *            B0-B1 : TTE in minutes (MSB first), or BMS_PRED_INVALID.
 *            B2-B3 : TTF in minutes (MSB first), or BMS_PRED_INVALID.
 *            B4    : Average power (0.5 W LSB).
 *            B5    : Valid flags (BMS_PRED_VALID_TTE | BMS_PRED_VALID_TTF).
 */
void BmsSoc_PredictionPack(float32 volt_V, float32 curr_mA, float32 pow_mW,
                           uint8 soc_raw, uint8 *pOutSdu);

#ifdef __cplusplus
}
#endif

#endif /* _BMSSOC_H_ */
