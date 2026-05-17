/**
 * @file       BmsSoc.c
 * @version    2.0.0
 * @brief      Coulomb-counting SOC tracker + stateless TTE/TTF predictor.
 *
 * @details    Replaces the v1 voltage-only OCV lookup (which was inaccurate
 *             under load because IR drop was not compensated) with a Coulomb
 *             counter:
 *
 *               remain_Ah(t+dt) = remain_Ah(t) - I(t) * dt / 3600
 *
 *             A small dead zone is applied around 0 A so that ADC noise on
 *             a resting bus does not drift the integrator.
 *
 *             In demo mode (BMS_SOC_DEMO_MODE == STD_ON) the integrator is
 *             seeded from a configuration constant (BMS_SOC_INITIAL_PCT)
 *             because the bench setup powers INA219 from the S32K144 3V3
 *             rail through a fixed resistor -- the measured voltage does
 *             NOT represent a battery state. The OCV lookup table is kept
 *             below an #if 0 guard so it can be re-enabled instantly when
 *             a real 1S Li-ion cell is connected.
 *
 *             All state is module-scope static (s_*) so the integrator
 *             survives across scheduler ticks and is visible in the S32DS
 *             debugger.
 */

/*******************************************************************************
* Includes
*******************************************************************************/
#include "Std_Types.h"
#include "BmsSoc.h"

/*******************************************************************************
* Definitions
*******************************************************************************/

/**
 * @brief   Nominal cell voltage (V) -- 1S Li-ion mid-discharge.
 * @details Used to convert Ah capacity into Wh for the energy model.
 *          With BMS_SOC_NOMINAL_CAPACITY_MAH = 100 and 3.7 V cell:
 *            Design_Wh = 0.100 Ah * 3.7 V = 0.37 Wh
 */
#define BMS_PRED_NOMINAL_V           (3.7f)

/**
 * @brief   Pack design capacity in Wh, derived from demo Ah * nominal V.
 * @details Used only by BmsSoc_PredictionPack (energy-based TTE/TTF).
 *          Auto-scales with BMS_SOC_NOMINAL_CAPACITY_MAH so changing the
 *          demo capacity rebuilds Wh correctly without manual sync.
 */
#define BMS_PRED_DESIGN_CAP_WH       (((float32)BMS_SOC_NOMINAL_CAPACITY_MAH / 1000.0f) * BMS_PRED_NOMINAL_V)

/**
 * @brief   Discharge current threshold for TTE validity (mA).
 * @details Lowered from 50 mA to 5 mA so the demo (typical load 17-180 mA)
 *          produces a valid TTE on the HMI. Matches the SOC dead-zone.
 */
#define BMS_PRED_DISCHG_MIN_MA       (5.0f)

/**
 * @brief   Charge current threshold (negative) for TTF validity (mA).
 */
#define BMS_PRED_CHARGE_MAX_MA       (-5.0f)

/**
 * @brief   Minimum useful power magnitude for division (W).
 * @details At 3.7 V x 5 mA the minimum meaningful power is ~18 mW, so
 *          0.01 W floors the divisor for very-low-current demos.
 */
#define BMS_PRED_MIN_POWER_W         (0.01f)

/**
 * @brief   Power LSB on CAN 0x400 byte 4 (W per bit).
 */
#define BMS_PRED_POWER_LSB_W         (0.5f)

/**
 * @brief   TTE/TTF saturation ceiling (minutes).
 * @details uint16 max là 65535 nhưng 0xFFFF được reserve cho BMS_PRED_INVALID,
 *          nên clamp thực tế ở 65534 phút (~1092 giờ ~ 45 ngày).
 */
#define BMS_PRED_MAX_MIN             (65534U)

/**
 * @brief   Số tick để 1 cửa sổ block-average power = 1 phút.
 *          1 min / 50 ms = 1200 tick.
 */
#define BMS_PRED_AVG_WINDOW_TICKS    (1200U)

/**
 * @brief   Power byte (B4) max for uint8 0.5 W/LSB: 255 × 0.5 = 127.5 W.
 */
#define BMS_PRED_POWER_BYTE_MAX      (255U)

/**
 * @brief   Nominal capacity expressed in mAh as float (for prediction path).
 * @details Used only by PredictionPack (approximation path). State path uses
 *          BMS_SOC_NOMINAL_nC from BmsSoc_Cfg.h (integer, exact).
 */
#define BMS_SOC_NOMINAL_MAH_F        ((float32)BMS_SOC_NOMINAL_CAPACITY_MAH)

#if (BMS_SOC_DEMO_MODE == STD_OFF)

/**
 * @brief   Number of OCV breakpoints (covers 0% to 100% in 10% steps).
 */
#define BMS_SOC_OCV_POINTS           (11U)

/**
 * @brief   Number of OCV segments between breakpoints.
 */
#define BMS_SOC_OCV_SEGMENTS         (10U)

/**
 * @brief   SOC step per OCV segment (encoded percent x 1, before x2 scaling).
 */
#define BMS_SOC_PCT_PER_SEGMENT      (10U)

/**
 * @brief   LSB scaling factor (raw = pct * 2 for 0.5% LSB encoding).
 */
#define BMS_SOC_RAW_SCALE            (2U)

/**
 * @brief   Voltage below which an OCV-based seed is considered invalid (mV).
 * @details For 1S Li-ion any reading below 2000 mV is almost certainly a
 *          disconnected / shorted sensor rather than a deeply-discharged
 *          cell (which clamps around 2.5 V protection threshold).
 */
#define BMS_SOC_OCV_VALID_MIN_MV     (2000U)

#endif /* BMS_SOC_DEMO_MODE == STD_OFF */

/*******************************************************************************
* Prototypes
*******************************************************************************/

static void  BmsSoc_RebuildSocRaw(void);
static uint8 BmsSoc_PctToRaw(uint8 pct);

#if (BMS_SOC_DEMO_MODE == STD_OFF)
static uint8 BmsSoc_OcvLookup(uint16 volt_mV);
#endif

/*******************************************************************************
* Variables
*******************************************************************************/

/**
 * @brief   Live remaining capacity in NANO-COULOMBS (sint64).
 * @details Primary state of the Coulomb counter -- integer, exact, no float
 *          rounding. Unit derivation:
 *            1 uA * 1 ms = 1 nC      (exact, no scaling factor needed)
 *          So tích phân mỗi tick chỉ là 1 lệnh SMULL trên ALU.
 *
 *          Clamped to [0, BMS_SOC_NOMINAL_nC] after each integration step
 *          inside BmsSoc_Update().
 *
 *          OVERFLOW AUDIT (sint64 range = +/- 9.2 * 10^18):
 *            - 100 mAh demo:    NOMINAL_nC = 3.6e11  -- headroom 2.5e7 X
 *            - 2 Ah Li-ion:     NOMINAL_nC = 7.2e12  -- headroom 1.3e6 X
 *            - 50 Ah industrial:NOMINAL_nC = 1.8e14  -- headroom 5.1e4 X
 *            - 50 kAh (absurd): NOMINAL_nC = 1.8e17  -- headroom 51 X
 *          Per-tick delta @ INA219 max (3.28 A) and TICK_MS = 50:
 *            curr_uA * TICK_MS = 3.28e6 * 50 = 1.64e8 nC.
 *          State * 200 trong RebuildSocRaw: max 1.8e17 * 200 = 3.6e19
 *          tại pin 50 kAh -- VƯỢT sint64. Realistic 50 Ah: 3.6e16 = OK.
 *          KẾT LUẬN: an toàn cho mọi pin cell từ 100 mAh đến 50 Ah
 *          (gấp 25 lần đề tài này). Không cần saturation.
 */
static volatile sint64 s_remain_nC = 0;

/**
 * @brief   Float mirror of s_remain_nC in mAh -- DEBUG / DISPLAY ONLY.
 * @details Recomputed at every state change for S32DS Expressions visibility.
 *          NEVER read back into the integration logic; this is a one-way
 *          derived value so its rounding error cannot corrupt the state.
 */
static volatile float32 s_remain_mAh_dbg = 0.0f;

/**
 * @brief   Cached SOC raw value (0.5% LSB), refreshed by BmsSoc_RebuildSocRaw.
 * @details Exposed to BmsApp via BmsSoc_GetSocRaw().
 */
static volatile uint8 s_socRaw = 0U;

/**
 * @brief   Tick counter -- DEBUG ONLY.
 * @details Increments on every successful BmsSoc_Update() call. Useful for
 *          confirming the SOC integrator is running when SOC stays constant
 *          (e.g. all current readings inside the dead zone).
 */
static volatile uint32 s_updateCnt = 0U;

/**
 * @brief   Power moving-average state (block-average, 1-phút window).
 *          s_avgPower_W giữ giá trị TB của cửa sổ trước; dùng cho prediction.
 */
static volatile float32 s_avgPower_W      = 0.0f;
static volatile float32 s_powerSum_mW     = 0.0f;
static volatile uint16  s_powerSumCount   = 0U;

#if (BMS_SOC_DEMO_MODE == STD_OFF)

/**
 * @brief   OCV lookup table for a 1S Li-ion cell (mV at 0%, 10%, ..., 100%).
 * @details Updated from 3S (9000-12600 mV) to 1S (3000-4200 mV) to match the
 *          single-cell demo configuration. Dead code in demo mode; restored
 *          when BMS_SOC_DEMO_MODE = STD_OFF and a real cell is connected.
 */
static const uint16 s_ocvTable[BMS_SOC_OCV_POINTS] =
{
    3000U,   /*   0% - 3.000 V */
    3300U,   /*  10% - 3.300 V */
    3500U,   /*  20% - 3.500 V */
    3600U,   /*  30% - 3.600 V */
    3650U,   /*  40% - 3.650 V */
    3700U,   /*  50% - 3.700 V */
    3750U,   /*  60% - 3.750 V */
    3800U,   /*  70% - 3.800 V */
    3900U,   /*  80% - 3.900 V */
    4000U,   /*  90% - 4.000 V */
    4200U    /* 100% - 4.200 V */
};

#endif /* BMS_SOC_DEMO_MODE == STD_OFF */

/*******************************************************************************
* Code
*******************************************************************************/

/**
 * @brief   Convert percent (0..100) to raw 0.5% LSB encoding (0..200).
 *
 * @param[in] pct  Percent value, will be clamped to 100.
 *
 * @return  Encoded raw value in [0, 200].
 */
static uint8 BmsSoc_PctToRaw(uint8 pct)
{
    uint8 clamped;
    uint8 raw;

    clamped = pct;
    if (clamped > 100U)
    {
        clamped = 100U;
    }

    raw = (uint8)((uint16)clamped * 2U);

    return raw;
}

/**
 * @brief   Recompute s_socRaw from s_remain_nC (integer math, round half up).
 *
 * @details Derived value pipeline:
 *            soc_raw = (state_nC * 200 + nominal_nC/2) / nominal_nC
 *          The + nominal_nC/2 trick rounds to the nearest integer instead
 *          of truncating. All arithmetic in sint64 to avoid intermediate
 *          overflow (max product = 3.6e11 * 200 = 7.2e13, well within int64).
 *
 *          Also refreshes the float debug mirror s_remain_mAh_dbg.
 */
static void BmsSoc_RebuildSocRaw(void)
{
    sint64 raw_calc;

    raw_calc = 0;

    if (BMS_SOC_NOMINAL_nC > 0)
    {
        raw_calc = ((s_remain_nC * (sint64)BMS_SOC_FULL_RAW) + (BMS_SOC_NOMINAL_nC / 2))
                   / BMS_SOC_NOMINAL_nC;
    }

    if (raw_calc < 0)
    {
        raw_calc = 0;
    }
    if (raw_calc > (sint64)BMS_SOC_FULL_RAW)
    {
        raw_calc = (sint64)BMS_SOC_FULL_RAW;
    }

    s_socRaw = (uint8)raw_calc;

    /* Update float debug mirror (1 mAh = 3.6e9 nC).
     * NOTE: divide ngoài state path → sai số float ở đây không quay lại
     * làm bẩn integer state. Chỉ dùng để xem trong S32DS Expressions. */
    s_remain_mAh_dbg = (float32)s_remain_nC / 3.6e9f;
}

#if (BMS_SOC_DEMO_MODE == STD_OFF)

/**
 * @brief   Voltage-based OCV lookup for a 1S Li-ion cell.
 *
 * @details Linear interpolation across 11 breakpoints (0%, 10%, ..., 100%).
 *          Used ONLY for the boot-time seed in real-pack mode. Never invoked
 *          at runtime, even when enabled.
 *
 * @param[in] volt_mV  Pack voltage in millivolts.
 *
 * @return  SOC raw in [0, 200].
 */
static uint8 BmsSoc_OcvLookup(uint16 volt_mV)
{
    uint8   ret;
    uint8   i;
    uint8   soc_pct;
    uint16  delta_v;
    uint16  above_v;
    boolean done;

    ret     = BMS_SOC_FULL_RAW;
    soc_pct = 0U;
    delta_v = 0U;
    above_v = 0U;
    done    = FALSE;

    if (volt_mV >= s_ocvTable[BMS_SOC_OCV_SEGMENTS])
    {
        ret  = BMS_SOC_FULL_RAW;
        done = TRUE;
    }
    else if (volt_mV <= s_ocvTable[0U])
    {
        ret  = BMS_SOC_EMPTY_RAW;
        done = TRUE;
    }
    else
    {
        for (i = 0U; (i < BMS_SOC_OCV_SEGMENTS) && (done == FALSE); i++)
        {
            if (volt_mV < s_ocvTable[i + 1U])
            {
                delta_v = s_ocvTable[i + 1U] - s_ocvTable[i];
                above_v = volt_mV - s_ocvTable[i];
                soc_pct = (uint8)((uint16)i * BMS_SOC_PCT_PER_SEGMENT)
                          + (uint8)((above_v * BMS_SOC_PCT_PER_SEGMENT) / delta_v);
                ret     = (uint8)(soc_pct * BMS_SOC_RAW_SCALE);
                done    = TRUE;
            }
        }
    }

    return ret;
}

#endif /* BMS_SOC_DEMO_MODE == STD_OFF */

/**
 * @brief   See BmsSoc.h.
 */
void BmsSoc_Init(uint16 boot_volt_mV)
{
    uint8 seed_raw;

#if (BMS_SOC_DEMO_MODE == STD_ON)
    /* Bench setup: 3V3 + resistor, no real pack. Voltage is ignored. */
    (void)boot_volt_mV;
    seed_raw = BmsSoc_PctToRaw((uint8)BMS_SOC_INITIAL_PCT);
#else
    seed_raw = BmsSoc_OcvLookup(boot_volt_mV);
    if ((seed_raw == BMS_SOC_EMPTY_RAW) && (boot_volt_mV < BMS_SOC_OCV_VALID_MIN_MV))
    {
        /* Voltage too low to be a real flat pack -- treat as sensor failure */
        seed_raw = BmsSoc_PctToRaw((uint8)BMS_SOC_INITIAL_PCT);
    }
#endif

    /* Seed integer state: state = seed_raw / FULL_RAW * NOMINAL_nC.
     * Order phép tính: nhân trước, chia sau để giữ precision.
     * Max intermediate: 200 * 7.2e12 (2 Ah) = 1.44e15 -- fits sint64. */
    s_remain_nC  = ((sint64)seed_raw * BMS_SOC_NOMINAL_nC) / (sint64)BMS_SOC_FULL_RAW;
    s_updateCnt  = 0U;

    BmsSoc_RebuildSocRaw();
}

/**
 * @brief   See BmsSoc.h.
 *
 * @details API float (input từ INA219 driver giữ nguyên). Internal chuyển
 *          sang integer 1 LẦN ở biên rồi tích phân exact. Sai số ULP của
 *          cast chỉ tích lũy theo N × LSB_int, không phải N × ULP × value
 *          như float-on-float arithmetic trước đây.
 */
void BmsSoc_Update(float32 curr_mA)
{
    sint32 curr_uA;
    sint64 delta_nC;

    /* ────── Single boundary cast: float mA → sint32 µA ──────
     * Sai số tối đa ≈ 1 µA per-tick (rounding của float×1000 + truncation).
     * Trên 1 ngày (1.7M tick × 50 ms): drift tổng ≤ 1.7M × 1 µA × 50 ms = 85 µC
     * ≈ 0.024 µAh — hoàn toàn ignorable với pin 100 mAh.
     * Quan trọng: KHÔNG có tích lũy float-on-float trong state. */
    curr_uA = (sint32)(curr_mA * 1000.0f);

    /* Dead zone (5 mA) — kill ADC noise around zero load */
    if ((curr_uA < BMS_SOC_DEAD_ZONE_uA) && (curr_uA > -BMS_SOC_DEAD_ZONE_uA))
    {
        curr_uA = 0;
    }

    /* ────── Integer-only integration (compiler emit SMULL 32×32→64) ──────
     * Unit: 1 µA × 1 ms = 1 nC exactly, no scaling factor needed. */
    delta_nC = (sint64)curr_uA * (sint64)BMS_SOC_TICK_MS;

    s_remain_nC -= delta_nC;

    if (s_remain_nC < 0)
    {
        s_remain_nC = 0;
    }
    if (s_remain_nC > BMS_SOC_NOMINAL_nC)
    {
        s_remain_nC = BMS_SOC_NOMINAL_nC;
    }

    s_updateCnt++;

    BmsSoc_RebuildSocRaw();
}

/**
 * @brief   See BmsSoc.h.
 */
uint8 BmsSoc_GetSocRaw(void)
{
    return s_socRaw;
}

/**
 * @brief   See BmsSoc.h.
 */
void BmsSoc_SetSocPct(uint8 soc_pct)
{
    uint8 raw;

    raw = BmsSoc_PctToRaw(soc_pct);

    /* Integer rebuild: state_nC = raw * NOMINAL_nC / FULL_RAW */
    s_remain_nC = ((sint64)raw * BMS_SOC_NOMINAL_nC) / (sint64)BMS_SOC_FULL_RAW;

    BmsSoc_RebuildSocRaw();
}

/**
 * @brief   See BmsSoc.h.
 *
 * @details Mapping 1 chiều → 2 chiều ảo dùng cho demo có biến trở tải.
 *          virtual_I = raw_I − ZERO_POINT, có dead-zone đối xứng quanh
 *          điểm zero để IDLE state ổn định.
 */
float32 BmsSoc_VirtualCurrent_mA(float32 raw_mA)
{
    float32 v;

#if (BMS_SOC_BIDIR_SIM_ENABLE == STD_ON)
    v = raw_mA - BMS_SOC_BIDIR_ZERO_POINT_mA;
    if ((v < BMS_SOC_BIDIR_DEAD_mA) && (v > -BMS_SOC_BIDIR_DEAD_mA))
    {
        v = 0.0f;
    }
#else
    /* Real-pack mode: INA219 đã trả signed bidirectional, không map */
    v = raw_mA;
#endif

    return v;
}

/**
 * @brief   See BmsSoc.h.
 *
 * @details Block-average: cộng dồn N mẫu trong 1 phút, snapshot trung bình
 *          rồi reset bộ đếm. Dùng để giảm jitter cho prediction TTE/TTF.
 */
void BmsSoc_AccumulatePower(float32 inst_power_mW)
{
    s_powerSum_mW   += inst_power_mW;
    s_powerSumCount += 1U;

    if (s_powerSumCount >= BMS_PRED_AVG_WINDOW_TICKS)
    {
        s_avgPower_W    = (s_powerSum_mW / (float32)s_powerSumCount) / 1000.0f;
        s_powerSum_mW   = 0.0f;
        s_powerSumCount = 0U;
    }
}

/**
 * @brief   Helper: saturating cast float minutes -> uint16, dùng cho TTE/TTF.
 *          Trả về clamped value, không bao giờ trùng INVALID sentinel.
 */
static uint16 BmsSoc_SaturateMinutes(float32 minutes_f)
{
    uint16 ret;

    if (minutes_f < 0.0f)
    {
        ret = 0U;
    }
    else if (minutes_f >= (float32)BMS_PRED_MAX_MIN)
    {
        ret = (uint16)BMS_PRED_MAX_MIN;   /* 65534 -- tránh nhầm với INVALID 0xFFFF */
    }
    else
    {
        ret = (uint16)minutes_f;
    }

    return ret;
}

/**
 * @brief   See BmsSoc.h.
 *
 * @details Dùng s_avgPower_W (moving avg 1-phút) làm divisor cho TTE để
 *          khử jitter. Saturate TTE/TTF tránh tràn uint16. Saturate byte 4
 *          power tránh tràn uint8.
 *
 *          Có 1 quick-bootstrap: nếu chưa đủ 1 cửa sổ avg (cold start),
 *          tạm dùng pow_mW tức thời để TTE không cần đợi 1 phút mới hiện
 *          giá trị đầu.
 */
void BmsSoc_PredictionPack(float32 volt_V, float32 curr_mA, float32 pow_mW,
                           uint8 soc_raw, uint8 *pOutSdu)
{
    float32 remain_Wh;
    float32 power_W;
    float32 empty_Wh;
    float32 chg_W;
    float32 pow_byte_f;
    uint16  tte_min;
    uint16  ttf_min;
    uint8   pow_byte;
    uint8   valid_flags;

    if (pOutSdu != NULL_PTR)
    {
        remain_Wh   = BMS_PRED_DESIGN_CAP_WH * ((float32)soc_raw / (float32)BMS_SOC_FULL_RAW);

        /* Power divisor cho TTE: ưu tiên avg 1-phút; bootstrap = inst nếu
         * chưa có cửa sổ nào hoàn thành (s_avgPower_W == 0). */
        if (s_avgPower_W > 0.0f)
        {
            power_W = s_avgPower_W;
        }
        else
        {
            power_W = pow_mW / 1000.0f;
        }

        valid_flags = 0x00U;
        tte_min     = BMS_PRED_INVALID;
        ttf_min     = BMS_PRED_INVALID;

        /* TTE: chỉ khi đang xả thật (curr > +5 mA) và có công suất đủ ý nghĩa */
        if ((curr_mA > BMS_PRED_DISCHG_MIN_MA) && (power_W > BMS_PRED_MIN_POWER_W))
        {
            tte_min      = BmsSoc_SaturateMinutes((remain_Wh / power_W) * 60.0f);
            valid_flags |= BMS_PRED_VALID_TTE;
        }

        /* TTF: chỉ khi đang sạc */
        if (curr_mA < BMS_PRED_CHARGE_MAX_MA)
        {
            empty_Wh = BMS_PRED_DESIGN_CAP_WH - remain_Wh;
            chg_W    = (-curr_mA / 1000.0f) * volt_V;
            if (chg_W > BMS_PRED_MIN_POWER_W)
            {
                ttf_min      = BmsSoc_SaturateMinutes((empty_Wh / chg_W) * 60.0f);
                valid_flags |= BMS_PRED_VALID_TTF;
            }
        }

        /* Power byte (avg, 0.5 W/LSB, saturate 0..127.5 W) */
        pow_byte_f = power_W / BMS_PRED_POWER_LSB_W;
        if (pow_byte_f < 0.0f)                              { pow_byte_f = 0.0f; }
        if (pow_byte_f > (float32)BMS_PRED_POWER_BYTE_MAX)  { pow_byte_f = (float32)BMS_PRED_POWER_BYTE_MAX; }
        pow_byte = (uint8)pow_byte_f;

        pOutSdu[0U] = (uint8)(tte_min >> 8U);
        pOutSdu[1U] = (uint8)(tte_min & 0xFFU);
        pOutSdu[2U] = (uint8)(ttf_min >> 8U);
        pOutSdu[3U] = (uint8)(ttf_min & 0xFFU);
        pOutSdu[4U] = pow_byte;
        pOutSdu[5U] = valid_flags;
    }
}
