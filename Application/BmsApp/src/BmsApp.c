/**
 * @file       BmsApp.c
 * @version    2.0.0
 * @brief      Top-level BMS application: sensor reads, signal packing, CAN TX.
 *
 * @details    Real 1S Li-ion pack (4.2 V OV / 3.0 V UV). INA219 trả dòng
 *             signed bidirectional. Cờ cảnh báo UV/OV được pack vào CAN 0x210
 *             byte 3 và CAN 0x300 byte 2 để HMI hiển thị tức thời.
 *
 *             Mỗi Task50/100/200/500ms vẫn theo pattern:
 *               1. Đọc cảm biến qua CDD.
 *               2. Encode raw → CAN signals.
 *               3. Pack vào SDU tĩnh.
 *               4. Can_43_FLEXCAN_Write(...).
 */

/*******************************************************************************
* Includes
*******************************************************************************/
#include "Std_Types.h"
#include "BmsApp.h"
#include "BmsFault.h"
#include "CDD_INA219.h"
#include "CDD_STM32Temp.h"
#include "BMS_Nvm.h"
#include "BMS_SoC.h"

/*******************************************************************************
* Definitions
*******************************************************************************/

/**
 * @brief   CAN identifiers and DLCs for outbound BMS messages.
 */
#define BMS_ELEC_CAN_ID              (0x100U)
#define BMS_ELEC_DLC                 (8U)
#define BMS_ELEC_SW_PDU              (0U)

#define BMS_TEMP_CAN_ID              (0x200U)
#define BMS_TEMP_DLC                 (4U)
#define BMS_TEMP_SW_PDU              (2U)

#define BMS_VOLT_CAN_ID              (0x210U)
#define BMS_VOLT_DLC                 (4U)
#define BMS_VOLT_SW_PDU              (3U)

#define BMS_SOC_CAN_ID               (0x300U)
#define BMS_SOC_DLC                  (6U)
#define BMS_SOC_SW_PDU               (4U)

#define BMS_PREDICTION_CAN_ID        (0x400U)
#define BMS_PREDICTION_SW_PDU        (5U)
#define BMS_PRED_DLC                 (6U)

/**
 * @brief   Voltage warning flag bits (CAN 0x210 byte 3 và 0x300 byte 2).
 * @details bit 0 = Undervoltage (V < 3.0 V).
 *          bit 1 = Overvoltage  (V > 4.2 V).
 */
#define BMS_VOLT_WARN_UV             (0x01U)
#define BMS_VOLT_WARN_OV             (0x02U)

/**
 * @brief   Current scaling: mA -> 0.1 mA LSB (CAN 0x100).
 */
#define BMS_MA_TO_RAW_SCALE          (10.0f)

/**
 * @brief   Power scaling: mW -> 0.1 mW LSB (CAN 0x100).
 */
#define BMS_MW_TO_RAW_SCALE          (10.0f)

/**
 * @brief   Temperature encoding: raw_8 = (temp_C + 40) / 0.5.
 */
#define BMS_TEMP_OFFSET_C            (40.0f)
#define BMS_TEMP_LSB_C               (0.5f)

/**
 * @brief   Hysteresis thresholds cho CAN 0x300 byte 1 (state machine).
 * @details Dùng raw curr (0.1 mA LSB) với hysteresis ±7 mA enter / ±3 mA exit
 *          để chống flicker khi noise INA219 dao động quanh 0. Pin thật trả
 *          dòng signed: dương = xả, âm = sạc.
 */
#define BMS_STATE_ENTER_DCHG_RAW     (70)    /* +7.0 mA → vào DISCHARGING */
#define BMS_STATE_EXIT_DCHG_RAW      (30)    /* <+3.0 mA → về IDLE        */
#define BMS_STATE_ENTER_CHG_RAW      (-70)   /* −7.0 mA → vào CHARGING    */
#define BMS_STATE_EXIT_CHG_RAW       (-30)   /* >−3.0 mA → về IDLE        */

/**
 * @brief   SOC method byte (CAN 0x300 byte 3) -- Coulomb counting.
 */
#define BMS_SOC_METHOD_COULOMB       (0x01U)

/**
 * @brief   Ngưỡng ΔSOC (%) để trigger ghi Flash. Tránh mài mòn cell.
 */
#define BMS_NVM_SAVE_THRESHOLD_PCT   (1.0f)

/*******************************************************************************
* Prototypes
*******************************************************************************/

static void  BmsApp_PackElecMeasure(uint16 volt_mV, sint16 curr_raw, uint16 pow_raw);
static void  BmsApp_PackVoltage(uint16 volt_mV, uint8 warnFlags);
static void  BmsApp_PackSoc(uint8 soc_raw, sint16 curr_raw, uint8 warnFlags);
static void  BmsApp_PackTemperature(uint8 temp_raw, uint8 status);
static void  BmsApp_TxBmsFrame(Can_HwHandleType hoh, uint32 canId, PduIdType swPduHandle,
                               uint8 dlc, uint8 *pSdu);
static void  BmsApp_HandleHmiCommand(const uint8 *pData);
static uint8 BmsApp_ComputeVoltageWarn(uint16 volt_mV);
static void  BmsApp_NvmSaveSocIfNeeded(float32 currentSocPercent);

/*******************************************************************************
* Variables
*******************************************************************************/

/**
 * @brief   Static SDUs cho các CAN message outbound.
 */
static uint8 s_sduElecMeasure[BMS_ELEC_DLC] = {0U};
static uint8 s_sduTemperature[BMS_TEMP_DLC] = {0U};
static uint8 s_sduVoltage[BMS_VOLT_DLC]     = {0U};
static uint8 s_sduSoc[BMS_SOC_DLC]          = {0U};
static uint8 s_sduPrediction[BMS_PRED_DLC]  = {0U};

/**
 * @brief   Last-known sensor readings -- volatile để debugger luôn thấy giá trị mới.
 */
static volatile float32 s_lastVoltV     = 0.0f;
static volatile float32 s_lastCurrMa    = 0.0f;
static volatile float32 s_lastPowMw     = 0.0f;
static volatile float32 s_lastTempC     = 0.0f;
static volatile uint16  s_lastVoltMv    = 0U;
static volatile sint16  s_lastCurrRaw   = 0;
static volatile uint8   s_lastSocRaw    = 0U;
static volatile uint8   s_lastTempRaw   = 0U;
static volatile uint8   s_lastWarnFlags = 0U;  /* UV/OV flags từ Task50ms */

/**
 * @brief   Driver-level statuses (debug visibility).
 */
static volatile uint8 s_lastInaStatusV = 0U;
static volatile uint8 s_lastInaStatusI = 0U;
static volatile uint8 s_lastInaStatusP = 0U;
static volatile uint8 s_lastTempStatus = 0U;

/**
 * @brief   CAN driver counters / debug.
 */
static volatile uint8          s_canTxConfirmCnt  = 0U;
static volatile boolean        s_canTxFlag        = FALSE;
static volatile boolean        s_canRxFlag        = FALSE;
static volatile Std_ReturnType s_canTxLastResult  = E_OK;
static volatile uint32         s_canTxFailCnt     = 0U;
static volatile uint32         s_canTxAttemptCnt  = 0U;

/**
 * @brief   SOC % lần cuối đã lưu Flash (khởi 101 = ngoài range để ép lưu lần đầu).
 */
static volatile float32 s_lastSavedSocPercent = 101.0f;

/*******************************************************************************
* Code
*******************************************************************************/

/**
 * @brief   Tính cờ cảnh báo áp dựa trên ngưỡng UV/OV trong BmsFault.h.
 * @return  Bit-mask: BMS_VOLT_WARN_UV | BMS_VOLT_WARN_OV.
 */
static uint8 BmsApp_ComputeVoltageWarn(uint16 volt_mV)
{
    uint8 flags = 0U;

    if (volt_mV > BMS_FAULT_OV_THRESH_MV)
    {
        flags |= BMS_VOLT_WARN_OV;
    }
    if (volt_mV < BMS_FAULT_UV_THRESH_MV)
    {
        flags |= BMS_VOLT_WARN_UV;
    }

    return flags;
}

/**
 * @brief   Pack BMS_ElecMeasure (0x100, 8 bytes).
 */
static void BmsApp_PackElecMeasure(uint16 volt_mV, sint16 curr_raw, uint16 pow_raw)
{
    s_sduElecMeasure[0U] = (uint8)(volt_mV >> 8U);
    s_sduElecMeasure[1U] = (uint8)(volt_mV & 0xFFU);
    s_sduElecMeasure[2U] = (uint8)((uint16)curr_raw >> 8U);
    s_sduElecMeasure[3U] = (uint8)((uint16)curr_raw & 0xFFU);
    s_sduElecMeasure[4U] = (uint8)(pow_raw >> 8U);
    s_sduElecMeasure[5U] = (uint8)(pow_raw & 0xFFU);
    s_sduElecMeasure[6U] = BMS_INA219_STATUS_OK;
    s_sduElecMeasure[7U] = 0x00U;
}

/**
 * @brief   Pack BMS_Voltage (0x210, 4 bytes).
 *          B0-B1 : voltage mV
 *          B2    : status (BMS_INA219_STATUS_OK)
 *          B3    : warning flags (bit 0 UV, bit 1 OV) -- NEW v2
 */
static void BmsApp_PackVoltage(uint16 volt_mV, uint8 warnFlags)
{
    s_sduVoltage[0U] = (uint8)(volt_mV >> 8U);
    s_sduVoltage[1U] = (uint8)(volt_mV & 0xFFU);
    s_sduVoltage[2U] = BMS_INA219_STATUS_OK;
    s_sduVoltage[3U] = warnFlags;
}

/**
 * @brief   Pack BMS_SOC (0x300, 6 bytes -- SOH bỏ).
 *          B0   : SOC raw (0.5 %/LSB)
 *          B1   : state (0=IDLE, 1=CHG, 2=DCHG) với hysteresis
 *          B2   : warning flags (bit 0 UV, bit 1 OV)
 *          B3   : SOC method (1=Coulomb)
 *          B4-5 : reserved (0x00) -- giữ DLC=6 để khớp FlexCAN PBcfg
 */
static void BmsApp_PackSoc(uint8 soc_raw, sint16 curr_raw, uint8 warnFlags)
{
    static uint8 s_busState = BMS_STATE_IDLE;

    switch (s_busState)
    {
        case BMS_STATE_DISCHARGING:
            if (curr_raw < BMS_STATE_EXIT_DCHG_RAW)
            {
                s_busState = BMS_STATE_IDLE;
            }
            break;

        case BMS_STATE_CHARGING:
            if (curr_raw > BMS_STATE_EXIT_CHG_RAW)
            {
                s_busState = BMS_STATE_IDLE;
            }
            break;

        case BMS_STATE_IDLE:
        default:
            if (curr_raw > BMS_STATE_ENTER_DCHG_RAW)
            {
                s_busState = BMS_STATE_DISCHARGING;
            }
            else if (curr_raw < BMS_STATE_ENTER_CHG_RAW)
            {
                s_busState = BMS_STATE_CHARGING;
            }
            else
            {
                /* Stay IDLE */
            }
            break;
    }

    s_sduSoc[0U] = soc_raw;
    s_sduSoc[1U] = s_busState;
    s_sduSoc[2U] = warnFlags;
    s_sduSoc[3U] = BMS_SOC_METHOD_COULOMB;
    s_sduSoc[4U] = 0x00U;
    s_sduSoc[5U] = 0x00U;
}

/**
 * @brief   Pack BMS_Temperature (0x200, 4 bytes).
 */
static void BmsApp_PackTemperature(uint8 temp_raw, uint8 status)
{
    s_sduTemperature[0U] = temp_raw;
    s_sduTemperature[1U] = temp_raw;
    s_sduTemperature[2U] = temp_raw;
    s_sduTemperature[3U] = status;
}

/**
 * @brief   Common helper: transmit 1 BMS CAN frame qua FlexCAN.
 */
static void BmsApp_TxBmsFrame(Can_HwHandleType hoh, uint32 canId, PduIdType swPduHandle,
                              uint8 dlc, uint8 *pSdu)
{
    Can_PduType    pdu;
    Std_ReturnType ret;

    pdu.id          = canId;
    pdu.swPduHandle = swPduHandle;
    pdu.length      = dlc;
    pdu.sdu         = pSdu;

    ret = Can_43_FLEXCAN_Write(hoh, &pdu);

    s_canTxAttemptCnt++;
    s_canTxLastResult = ret;
    if (ret != E_OK)
    {
        s_canTxFailCnt++;
    }
}

/**
 * @brief   Dispatch HMI command bytes received trên CAN 0x710.
 */
static void BmsApp_HandleHmiCommand(const uint8 *pData)
{
    if (pData != NULL_PTR)
    {
        switch (pData[0U])
        {
            case BMS_HMI_CMD_RESET_FAULT:
                BmsFault_Reset();
                break;

            case BMS_HMI_CMD_CALIB_SOC:
                /* pData[1] = target SOC percent (0..100). */
                (void)BMS_SoC_Init((float32)pData[1U]);
                break;

            case BMS_HMI_CMD_SNAPSHOT:
                BmsApp_TxBmsFrame(Can_43_FLEXCANConf_CanHardwareObject_HOH_Tx_ElecMeasure,
                                  BMS_ELEC_CAN_ID, BMS_ELEC_SW_PDU,
                                  BMS_ELEC_DLC, s_sduElecMeasure);
                BmsApp_TxBmsFrame(Can_43_FLEXCANConf_CanHardwareObject_HOH_Tx_Temperature,
                                  BMS_TEMP_CAN_ID, BMS_TEMP_SW_PDU,
                                  BMS_TEMP_DLC, s_sduTemperature);
                BmsApp_TxBmsFrame(Can_43_FLEXCANConf_CanHardwareObject_HOH_Tx_Voltage,
                                  BMS_VOLT_CAN_ID, BMS_VOLT_SW_PDU,
                                  BMS_VOLT_DLC, s_sduVoltage);
                BmsApp_TxBmsFrame(Can_43_FLEXCANConf_CanHardwareObject_HOH_Tx_SOC,
                                  BMS_SOC_CAN_ID, BMS_SOC_SW_PDU,
                                  BMS_SOC_DLC, s_sduSoc);
                break;

            default:
                /* Ignore unknown HMI command */
                break;
        }
    }
}

/**
 * @brief   See BmsApp.h.
 */
Std_ReturnType BmsApp_Init(void)
{
    Std_ReturnType   status = E_OK;
    uint8            i;
    volatile float32 current_soc_val;

    BMS_Nvm_Init();
    current_soc_val = BMS_Nvm_ReadSoC();
    status          = BMS_SoC_Init(current_soc_val);

    if (status == E_NOT_OK)
    {
        for (i = 0U; i < BMS_ELEC_DLC; i++) { s_sduElecMeasure[i] = 0U; }
        for (i = 0U; i < BMS_TEMP_DLC; i++) { s_sduTemperature[i] = 0U; }
        for (i = 0U; i < BMS_VOLT_DLC; i++) { s_sduVoltage[i]     = 0U; }
        for (i = 0U; i < BMS_SOC_DLC;  i++) { s_sduSoc[i]         = 0U; }
        for (i = 0U; i < BMS_PRED_DLC; i++) { s_sduPrediction[i]  = 0U; }

        s_lastVoltV    = 0.0f;
        s_lastCurrMa   = 0.0f;
        s_lastPowMw    = 0.0f;
        s_lastTempC    = 0.0f;
        s_lastVoltMv   = 0U;
        s_lastCurrRaw  = 0;
        s_lastSocRaw   = 0U;
        s_lastTempRaw  = 0U;
        s_lastWarnFlags = 0U;

        s_lastInaStatusV = 0U;
        s_lastInaStatusI = 0U;
        s_lastInaStatusP = 0U;
        s_lastTempStatus = 0U;

        s_canTxConfirmCnt = 0U;
        s_canTxFlag       = FALSE;
        s_canRxFlag       = FALSE;
        s_canTxLastResult = E_OK;
        s_canTxFailCnt    = 0U;
        s_canTxAttemptCnt = 0U;
    }

    return status;
}

/**
 * @brief   See BmsApp.h.
 *
 * @details Đọc INA219 (V/I/P signed), update Coulomb counter, pack & TX 0x100,
 *          check OV/UV/OC fault, refresh s_lastWarnFlags cho 0x210/0x300.
 */
Std_ReturnType BmsApp_Task50ms(void)
{
    Std_ReturnType retV;
    Std_ReturnType retI;
    Std_ReturnType retP;
    Std_ReturnType status = E_OK;
    float32        volt_mV;
    float32        curr_mA;
    float32        power_W;
    float32        power_mW;
    sint32         curr_raw_s32;
    sint32         pow_raw_s32;
    uint16         pow_raw;
    uint16         volt_mV_int;

    volt_mV  = 0.0f;
    curr_mA  = 0.0f;
    power_W  = 0.0f;
    power_mW = 0.0f;
    pow_raw  = 0U;

    retV = CDD_INA219_ReadBusVoltage_mV(&volt_mV);
    retI = CDD_INA219_ReadCurrent_mA(&curr_mA);
    retP = CDD_INA219_ReadPower(&power_W);

    s_lastInaStatusV = (uint8)retV;
    s_lastInaStatusI = (uint8)retI;
    s_lastInaStatusP = (uint8)retP;

    if ((retV == E_OK) && (retI == E_OK) && (retP == E_OK))
    {
        /* Clear INA comm fault */
        BmsFault_SetCommError(BMS_FAULT_SRC_INA_COMM, FALSE, 0U);

        /* CAN encode -- saturate phòng overflow uint16/sint16 */
        volt_mV_int = (uint16)volt_mV;

        curr_raw_s32 = (sint32)(curr_mA * BMS_MA_TO_RAW_SCALE);
        if (curr_raw_s32 >  32767)  { curr_raw_s32 =  32767; }
        if (curr_raw_s32 < -32768)  { curr_raw_s32 = -32768; }

        power_mW = power_W * 1000.0f;
        pow_raw_s32 = (sint32)((power_mW >= 0.0f ? power_mW : -power_mW) * BMS_MW_TO_RAW_SCALE);
        if (pow_raw_s32 > 65535) { pow_raw_s32 = 65535; }
        if (pow_raw_s32 < 0)     { pow_raw_s32 = 0;     }
        pow_raw = (uint16)pow_raw_s32;

        /* Update debug mirrors */
        s_lastVoltV    = volt_mV / 1000.0f;
        s_lastCurrMa   = curr_mA;
        s_lastPowMw    = power_mW;
        s_lastVoltMv   = volt_mV_int;
        s_lastCurrRaw  = (sint16)curr_raw_s32;

        /* Refresh UV/OV display flags ngay khi có voltage tươi */
        s_lastWarnFlags = BmsApp_ComputeVoltageWarn(volt_mV_int);

        /* SOC integrator dùng dòng signed thật từ INA219 */
        (void)BMS_SoC_Update(curr_mA);

        /* Pack & TX CAN 0x100 */
        BmsApp_PackElecMeasure(volt_mV_int, s_lastCurrRaw, pow_raw);
        BmsApp_TxBmsFrame(Can_43_FLEXCANConf_CanHardwareObject_HOH_Tx_ElecMeasure,
                          BMS_ELEC_CAN_ID, BMS_ELEC_SW_PDU,
                          BMS_ELEC_DLC, s_sduElecMeasure);

        /* Run OV/UV/OC fault latches */
        BmsFault_CheckElec(s_lastVoltMv, s_lastCurrRaw);
    }
    else
    {
        status = E_NOT_OK;
        BmsFault_SetCommError(BMS_FAULT_SRC_INA_COMM, TRUE, (uint8)retV);
    }

    return status;
}

/**
 * @brief   See BmsApp.h.
 */
Std_ReturnType BmsApp_Task100ms(void)
{
    Std_ReturnType status = E_OK;
    uint8          soc_percent;
    uint8          soc_raw;
    float32        socPercent;

    soc_percent  = BMS_SoC_Get();
    s_lastSocRaw = soc_percent;
    socPercent   = (float32)soc_percent;

    /* Encode 0..100 % → 0..200 (0.5 %/LSB) */
    soc_raw = (uint8)((uint16)soc_percent * 2U);
    if (soc_raw > 200U)
    {
        soc_raw = 200U;
    }

    /* Persist SOC vào Flash nếu thay đổi đủ ngưỡng */
    BmsApp_NvmSaveSocIfNeeded(socPercent);

    /* Pack & TX CAN 0x300 (SOC) -- truyền warn flags từ Task50ms */
    BmsApp_PackSoc(soc_raw, s_lastCurrRaw, s_lastWarnFlags);
    BmsApp_TxBmsFrame(Can_43_FLEXCANConf_CanHardwareObject_HOH_Tx_SOC,
                      BMS_SOC_CAN_ID, BMS_SOC_SW_PDU,
                      BMS_SOC_DLC, s_sduSoc);

    /* Pack & TX CAN 0x210 (Voltage) -- thêm UV/OV warn byte 3 */
    BmsApp_PackVoltage(s_lastVoltMv, s_lastWarnFlags);
    BmsApp_TxBmsFrame(Can_43_FLEXCANConf_CanHardwareObject_HOH_Tx_Voltage,
                      BMS_VOLT_CAN_ID, BMS_VOLT_SW_PDU,
                      BMS_VOLT_DLC, s_sduVoltage);

    return status;
}

/**
 * @brief   See BmsApp.h.
 */
Std_ReturnType BmsApp_Task200ms(void)
{
    STM32_Temp_ReturnType retT;
    float32               temp_C;
    uint8                 temp_raw;

    temp_C   = 0.0f;
    temp_raw = 0U;

    retT = CDD_STM32Temp_Read(&temp_C);
    s_lastTempStatus = (uint8)retT;

    if (retT == STM32_TEMP_OK)
    {
        BmsFault_SetCommError(BMS_FAULT_SRC_STM_COMM, FALSE, 0U);

        temp_raw = (uint8)((temp_C + BMS_TEMP_OFFSET_C) / BMS_TEMP_LSB_C);

        s_lastTempC   = temp_C;
        s_lastTempRaw = temp_raw;

        BmsApp_PackTemperature(temp_raw, BMS_TEMP_STATUS_OK);
        BmsFault_CheckTemp(temp_C, temp_raw);
    }
    else
    {
        BmsApp_PackTemperature(0U, BMS_TEMP_STATUS_FAIL);
        BmsFault_SetCommError(BMS_FAULT_SRC_STM_COMM, TRUE, (uint8)retT);
    }

    BmsApp_TxBmsFrame(Can_43_FLEXCANConf_CanHardwareObject_HOH_Tx_Temperature,
                      BMS_TEMP_CAN_ID, BMS_TEMP_SW_PDU,
                      BMS_TEMP_DLC, s_sduTemperature);

    return (retT == STM32_TEMP_OK) ? E_OK : E_NOT_OK;
}

/**
 * @brief   See BmsApp.h.
 */
Std_ReturnType BmsApp_Task500ms(void)
{
    Std_ReturnType status = E_OK;
    float32        remainingHours;
    uint8          soc_percent;
    uint16         tte_min;
    uint16         ttf_min;
    uint8          valid_flags;
    float32        current_A;
    float32        curr_mA;

    curr_mA = s_lastCurrMa;
    soc_percent    = BMS_SoC_Get();
    remainingHours = BMS_SoC_GetRemainingHours(curr_mA);

    tte_min     = 0xFFFFU;
    ttf_min     = 0xFFFFU;
    valid_flags = 0x00U;

    if ((remainingHours >= 1e5f) || (remainingHours < 0.0f))
    {
        /* dòng ~0 (idle) hoặc pin cạn → invalid */
    }
    else
    {
        current_A = curr_mA / 1000.0f;

        if (current_A > 0.0f)         /* xả */
        {
            tte_min = (uint16)(remainingHours * 60.0f);
            if (tte_min > 65534U) { tte_min = 65534U; }
            valid_flags = 0x01U;
        }
        else if (current_A < 0.0f)    /* sạc */
        {
            ttf_min = (uint16)(remainingHours * 60.0f);
            if (ttf_min > 65534U) { ttf_min = 65534U; }
            valid_flags = 0x02U;
        }
        else
        {
            /* xử lý ở nhánh trên */
        }
    }

    s_sduPrediction[0U] = (uint8)(tte_min >> 8U);
    s_sduPrediction[1U] = (uint8)(tte_min & 0xFFU);
    s_sduPrediction[2U] = (uint8)(ttf_min >> 8U);
    s_sduPrediction[3U] = (uint8)(ttf_min & 0xFFU);
    s_sduPrediction[4U] = soc_percent;
    s_sduPrediction[5U] = valid_flags;

    BmsApp_TxBmsFrame(Can_43_FLEXCANConf_CanHardwareObject_HOH_Tx_Prediction,
                      BMS_PREDICTION_CAN_ID, BMS_PREDICTION_SW_PDU,
                      BMS_PRED_DLC, s_sduPrediction);

    return status;
}

/*============================================================================*/
/*                            NVM Integration                                 */
/*============================================================================*/

/**
 * @brief   Lưu SOC vào Flash nếu |ΔSOC| ≥ 1 % và NVM idle.
 */
static void BmsApp_NvmSaveSocIfNeeded(float32 currentSocPercent)
{
    float32 diff;
    boolean shouldSave;

    if (BMS_Nvm_GetStatus() != NVM_RET_OK)
    {
        return;
    }

    diff = currentSocPercent - s_lastSavedSocPercent;
    if (diff < 0.0f)
    {
        diff = -diff;
    }

    shouldSave = (diff >= BMS_NVM_SAVE_THRESHOLD_PCT);

    if (shouldSave)
    {
        (void)BMS_Nvm_WriteSoC_Async(currentSocPercent);
        s_lastSavedSocPercent = currentSocPercent;
    }
}

/*******************************************************************************
* AUTOSAR CanIf callbacks (referenced từ generated FlexCAN config)
*******************************************************************************/

/**
 * @brief   I2c driver callback. Currently unused; required by I2c config.
 */
void I2c_Callback(uint8 u8Event, uint8 u8Channel)
{
    (void)u8Event;
    (void)u8Channel;
}

/**
 * @brief   FlexCAN bus-off notification. Currently unused.
 */
void CanIf_ControllerBusOff(uint8 ControllerId)
{
    (void)ControllerId;
}

/**
 * @brief   FlexCAN controller-mode change notification. Currently unused.
 */
void CanIf_ControllerModeIndication(uint8 ControllerId,
                                    Can_ControllerStateType ControllerMode)
{
    (void)ControllerId;
    (void)ControllerMode;
}

/**
 * @brief   FlexCAN TX-confirmation callback (interrupt context).
 */
void CanIf_TxConfirmation(PduIdType CanTxPduId)
{
    (void)CanTxPduId;
    s_canTxConfirmCnt++;
    s_canTxFlag = TRUE;
}

/**
 * @brief   FlexCAN RX-indication callback (interrupt context).
 */
void CanIf_RxIndication(const Can_HwType  *Mailbox,
                        const PduInfoType *PduInfoPtr)
{
    s_canRxFlag = TRUE;
    if ((Mailbox != NULL_PTR) && (PduInfoPtr != NULL_PTR))
    {
        if (Mailbox->CanId == BMS_HMI_CMD_CAN_ID)
        {
            BmsApp_HandleHmiCommand(PduInfoPtr->SduDataPtr);
        }
    }
}
