/**
 * @file       BmsApp.c
 * @version    1.0.0
 * @brief      Top-level BMS application: sensor reads, signal packing, CAN TX.
 *
 * @details    Each Task50/100/200/500ms function follows the same pattern:
 *               1. Read sensor through the corresponding CDD.
 *               2. Convert raw values into the encoded form described in the
 *                  CAN message table (§3 of project_context.md).
 *               3. Call a small pack helper (BmsApp_PackXxx) that lays the
 *                  bytes into the static SDU.
 *               4. Hand the SDU to Can_43_FLEXCAN_Write().
 *
 *             The pack helpers are isolated so they can be unit-tested
 *             without an MCU (backlog item #7).
 */

/*******************************************************************************
* Includes
*******************************************************************************/
//#include "Std_Types.h"
//#include "BmsApp.h"
//#include "BmsFault.h"
//#include "BmsSoc_Cfg.h"
//#include "CDD_INA219.h"
//#include "CDD_STM32Temp.h"
//#include "BMS_Nvm.h"
//#include "BMS_SoC.h"

#include "Std_Types.h"
#include "BmsApp.h"
#include "BmsFault.h"
#include "BmsSoc_Cfg.h"
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
 * @brief   Voltage scaling: Volts -> 0.001 V LSB (CAN 0x100 / 0x210).
 */
#define BMS_V_TO_MV_SCALE            (1000.0f)

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
 * @brief   Hysteresis thresholds for CAN 0x300 byte 2 state selection.
 *
 * @details Dùng raw curr (0.1 mA LSB) với hysteresis 4 mA (enter ±7, exit ±3)
 *          để chống flicker khi noise INA219 dao động quanh điểm zero ảo.
 *
 *          Khi BMS_SOC_BIDIR_SIM_ENABLE = STD_ON (demo biến trở):
 *            curr_raw đã là VIRTUAL signed (qua BmsSoc_VirtualCurrent_mA).
 *          Khi STD_OFF (pin thật):
 *            curr_raw là INA219 signed thật.
 *          Cả 2 trường hợp logic state byte giống nhau, chỉ là nguồn signed
 *          khác nhau.
 */
#define BMS_STATE_ENTER_DCHG_RAW     (70)    /* +7.0 mA → vào DISCHARGING */
#define BMS_STATE_EXIT_DCHG_RAW      (30)    /* <+3.0 mA → về IDLE */
#define BMS_STATE_ENTER_CHG_RAW      (-70)   /* −7.0 mA → vào CHARGING */
#define BMS_STATE_EXIT_CHG_RAW       (-30)   /* >−3.0 mA → về IDLE */

#define BMS_SOC_METHOD_COULOMB       (0x01U)  /* Coulomb counting method */

/*******************************************************************************
* Prototypes
*******************************************************************************/

static void BmsApp_PackElecMeasure(uint16 volt_mV, sint16 curr_raw, uint16 pow_raw);
static void BmsApp_PackVoltage(uint16 volt_mV);
static void BmsApp_PackSoc(uint8 soc_raw, sint16 curr_raw);
static void BmsApp_PackTemperature(uint8 temp_raw, uint8 status);
static void BmsApp_TxBmsFrame(Can_HwHandleType hoh, uint32 canId, PduIdType swPduHandle,
                              uint8 dlc, uint8 *pSdu);
static void BmsApp_HandleHmiCommand(const uint8 *pData);

static float32 BmsSoc_VirtualCurrent_mA(float32 raw_mA);

/**
 * @brief   Kiểm tra và lưu SOC vào Flash nếu thay đổi đủ ngưỡng.
 * @details Gọi từ BmsApp_Task100ms() để lưu SOC định kỳ.
 *          Chỉ lưu khi SOC thay đổi > 1% để tránh mài mòn Flash.
 *
 * @param   currentSocPercent  SOC hiện tại (0.0 - 100.0)
 * @return  void
 */
static void BmsApp_NvmSaveSocIfNeeded(float32 currentSocPercent);

/*******************************************************************************
* Variables
*******************************************************************************/

/**
 * @brief   Static SDUs for outbound CAN messages.
 * @details Declared at file scope so the FlexCAN mailbox can reference them
 *          across calls. Each is sized to its DLC.
 */
static uint8 s_sduElecMeasure[BMS_ELEC_DLC]                  = {0U};
static uint8 s_sduTemperature[BMS_TEMP_DLC]                  = {0U};
static uint8 s_sduVoltage[BMS_VOLT_DLC]                      = {0U};
static uint8 s_sduSoc[BMS_SOC_DLC]                           = {0U};
static uint8 s_sduPrediction[BMS_PRED_DLC]                   = {0U};

/**
 * @brief   Last-known sensor readings (used across tasks of different rates).
 * @details All variables here are file-scope static so they persist across
 *          task invocations AND remain visible in the S32DS Variables /
 *          Expressions debugger view. Inspect these names directly:
 *
 *            s_lastVoltV       - bus voltage in volts (float)
 *            s_lastCurrMa      - current in milliamps (float, signed)
 *            s_lastPowMw       - power in milliwatts (float)
 *            s_lastTempC       - temperature in degrees Celsius (float)
 *            s_lastVoltMv      - bus voltage in 0.001 V LSB (uint16)
 *            s_lastCurrRaw     - current in 0.1 mA LSB (sint16)
 *            s_lastSocRaw      - SOC in 0.5%% LSB (uint8, 0..200)
 *            s_lastTempRaw     - temperature in 0.5 deg C LSB, offset -40 (uint8)
 */
/* All telemetry is `volatile` so the optimiser cannot keep the value in
 * a register or constant-fold it away -- otherwise S32DS shows
 * "<optimized out>" in Expression view at -O0 still occasionally and
 * always at -Os/-O1.
 */
static volatile float32 s_lastVoltV    = 0.0f;
static volatile float32 s_lastCurrMa   = 0.0f;
static volatile float32 s_lastPowMw    = 0.0f;
static volatile float32 s_lastTempC    = 0.0f;
static volatile uint16  s_lastVoltMv   = 0U;
static volatile sint16  s_lastCurrRaw  = 0;
static volatile uint8   s_lastSocRaw   = 0U;
static volatile uint8   s_lastTempRaw  = 0U;

static volatile float32 g_current;


/**
 * @brief   Last-known driver return codes (visible in debugger).
 * @details Useful when CAN frames don't appear on the bus -- inspect these
 *          to localise the failure:
 *
 *            s_lastInaStatusV - INA219 voltage read return (0=OK)
 *            s_lastInaStatusI - INA219 current read return
 *            s_lastInaStatusP - INA219 power read return
 *            s_lastTempStatus - STM32 temperature read return (0=OK,
 *                               1=TIMEOUT, 2=HEADER, 3=CRC, 4=RANGE, 5=PARAM)
 */
static volatile uint8 s_lastInaStatusV = 0U;
static volatile uint8 s_lastInaStatusI = 0U;
static volatile uint8 s_lastInaStatusP = 0U;
static volatile uint8 s_lastTempStatus = 0U;

/**
 * @brief   CAN driver-level counters and last-result (visible in debugger).
 * @details Watch-list for CAN troubleshooting:
 *
 *            s_canTxConfirmCnt  - increments when CanIf_TxConfirmation runs
 *                                 (proves frames physically left the MCU)
 *            s_canTxFlag        - set TRUE on each TX confirm
 *            s_canRxFlag        - set TRUE on each RX indication
 *            s_canTxLastResult  - return code of the most recent
 *                                 Can_43_FLEXCAN_Write (E_OK == 0)
 *            s_canTxFailCnt     - increments every time Can_Write returns
 *                                 something other than E_OK (mailbox busy
 *                                 or driver not started)
 *            s_canTxAttemptCnt  - increments on every Can_Write call
 *                                 regardless of result
 */
static volatile uint8          s_canTxConfirmCnt  = 0U;
static volatile boolean        s_canTxFlag        = FALSE;
static volatile boolean        s_canRxFlag        = FALSE;
static volatile Std_ReturnType s_canTxLastResult  = E_OK;
static volatile uint32         s_canTxFailCnt     = 0U;
static volatile uint32         s_canTxAttemptCnt  = 0U;

/*******************************************************************************
* Variables - NVM state tracking
*******************************************************************************/

/**
 * @brief   SOC lần cuối đã lưu vào Flash (để tránh ghi liên tục)
 */
static volatile float32 s_lastSavedSocPercent = 101.0f;  /* Khởi tạo ngoài range */

/**
 * @brief   Ngưỡng thay đổi SOC để trigger ghi Flash (%)
 * @details Tránh mài mòn Flash: chỉ ghi khi SOC thay đổi >= 1%
 */
#define BMS_NVM_SAVE_THRESHOLD_PCT  (1.0f)

/*******************************************************************************
* Code
*******************************************************************************/

/**
 * @brief   Pack BMS_ElecMeasure (0x100, 8 bytes) into s_sduElecMeasure.
 *
 * @param[in] volt_mV   Bus voltage in 0.001 V LSB.
 * @param[in] curr_raw  Current in 0.1 mA LSB (signed).
 * @param[in] pow_raw   Power in 0.1 mW LSB.
 *
 * @return  void
 *
 * @post    s_sduElecMeasure layout:
 *            B0-B1 voltage, B2-B3 current, B4-B5 power, B6 status, B7 reserved.
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
 * @brief   Pack BMS_Voltage (0x210, 4 bytes) into s_sduVoltage.
 */
static void BmsApp_PackVoltage(uint16 volt_mV)
{
    s_sduVoltage[0U] = (uint8)(volt_mV >> 8U);
    s_sduVoltage[1U] = (uint8)(volt_mV & 0xFFU);
    s_sduVoltage[2U] = BMS_INA219_STATUS_OK;
    s_sduVoltage[3U] = 0x00U;
}

/**
 * @brief   Pack BMS_SOC (0x300, 6 bytes) into s_sduSoc.
 *
 * @param[in] soc_raw   SOC in 0.5% LSB.
 * @param[in] curr_raw  Current in 0.1 mA LSB (used to pick charge/discharge state).
 */
static void BmsApp_PackSoc(uint8 soc_raw, sint16 curr_raw)
{
    /* State machine với hysteresis cho IDLE/CHARGING/DISCHARGING.
     * Lưu state qua các lần gọi (static) để giữ hysteresis. */
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
    s_sduSoc[1U] = soc_raw;                                        /* SOH = SOC for demo */
    s_sduSoc[2U] = s_busState;
    s_sduSoc[3U] = 0x00U;                                          /* energy MSB -- TODO real value */
    s_sduSoc[4U] = 0x00U;                                          /* energy LSB */
    s_sduSoc[5U] = BMS_SOC_METHOD_COULOMB;                         /* method = Coulomb counting (v2) */
}

/**
 * @brief   Pack BMS_Temperature (0x200, 4 bytes) into s_sduTemperature.
 */
static void BmsApp_PackTemperature(uint8 temp_raw, uint8 status)
{
    s_sduTemperature[0U] = temp_raw;
    s_sduTemperature[1U] = temp_raw;
    s_sduTemperature[2U] = temp_raw;
    s_sduTemperature[3U] = status;
}

/**
 * @brief   Common helper: transmit one BMS CAN frame through the FlexCAN driver.
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
 * @brief   Dispatch HMI command bytes received on CAN 0x710.
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
                /* pData[1] carries the target SOC in PERCENT (0..100).
                 * Re-seed the integrator -- the SDU will be re-populated on
                 * the next Task100ms tick from the freshly cached raw value. */
            	BMS_SoC_Init((float32)pData[1U]);
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
	Std_ReturnType status = E_OK;
    uint8 i;
    volatile float32 current_soc_val;

    BMS_Nvm_Init();

    current_soc_val = BMS_Nvm_ReadSoC();

    status = BMS_SoC_Init(current_soc_val);

    if (status == E_NOT_OK)
    {
    	for (i = 0U; i < BMS_ELEC_DLC; i++)
    	{
    	 	s_sduElecMeasure[i] = 0U;
    	}
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
    /* Seed the Coulomb-counting SOC integrator.
     * In demo mode (BMS_SOC_DEMO_MODE == STD_ON) the voltage argument is
     * ignored -- the seed comes from BMS_SOC_INITIAL_PCT in BmsSoc_Cfg.h.
     * In real-pack mode we would replace 0U with a freshly read voltage. */
    //BmsSoc_Init(0U);

    //BmsApp_NvmRestoreSoc();

    return status;
}

/**
 * @brief   See BmsApp.h.
 */

Std_ReturnType BmsApp_Task50ms(void)
{
    Std_ReturnType retV, retI, retP;
    float32 volt_mV, curr_mA, power_W;
    float32 virt_curr_mA;
    float32 virt_pow_mW;
    sint32  curr_raw_s32;
    sint32  pow_raw_s32;
    uint16  pow_raw;
    uint16  volt_mV_int;
    Std_ReturnType status = E_OK;

    volt_mV   = 0.0f;
    curr_mA   = 0.0f;
    power_W   = 0.0f;
    pow_raw   = 0U;
    virt_curr_mA = 0.0f;
    virt_pow_mW  = 0.0f;

    // for test
    CDD_INA219_ReadCurrent_mA(&g_current);

    /* Đọc INA219 */
    retV = CDD_INA219_ReadBusVoltage_mV(&volt_mV);
    retI = CDD_INA219_ReadCurrent_mA(&curr_mA);
    retP = CDD_INA219_ReadPower(&power_W);

    /* Cache debug variables */
    s_lastInaStatusV = (uint8)retV;
    s_lastInaStatusI = (uint8)retI;
    s_lastInaStatusP = (uint8)retP;
    s_lastVoltV = volt_mV / 1000.0f;      /* mV → V */
    s_lastCurrMa = curr_mA;
    s_lastPowMw = power_W * 1000.0f;       /* W → mW */

    if ((retV == E_OK) && (retI == E_OK) && (retP == E_OK))
    {
        /* Clear INA219 communication fault */
        BmsFault_SetCommError(BMS_FAULT_SRC_INA_COMM, FALSE, 0U);

        /* ────── Virtual current mapping ──────
         * BMS_SoC_MapCurrent trả về giá trị signed (A), chuyển về mA
         */
        virt_curr_mA = BmsSoc_VirtualCurrent_mA(curr_mA);

        /* Virtual power = virtual_I × V (signed) */
        virt_pow_mW = virt_curr_mA * (volt_mV / 1000.0f);

        /* CAN encoding cho 0x100 */
        volt_mV_int = (uint16)volt_mV;
        s_lastVoltMv = volt_mV_int;

        curr_raw_s32 = (sint32)(virt_curr_mA * BMS_MA_TO_RAW_SCALE);
        if (curr_raw_s32 > 32767) { curr_raw_s32 = 32767; }
        if (curr_raw_s32 < -32768) { curr_raw_s32 = -32768; }
        s_lastCurrRaw = (sint16)curr_raw_s32;

        pow_raw_s32 = (sint32)((virt_pow_mW >= 0.0f ? virt_pow_mW : -virt_pow_mW) * BMS_MW_TO_RAW_SCALE);
        if (pow_raw_s32 > 65535) { pow_raw_s32 = 65535; }
        if (pow_raw_s32 < 0) { pow_raw_s32 = 0; }
        pow_raw = (uint16)pow_raw_s32;

        /* Update debug mirror */
        s_lastCurrMa = virt_curr_mA;
        s_lastPowMw = virt_pow_mW;

        /* Update SOC integrator using BMS_SoC_Update (dùng current raw, hàm sẽ tự map) */
        BMS_SoC_Update(curr_mA);

        /* Pack and send CAN 0x100 */
        BmsApp_PackElecMeasure(volt_mV, s_lastCurrRaw, pow_raw);
        BmsApp_TxBmsFrame(Can_43_FLEXCANConf_CanHardwareObject_HOH_Tx_ElecMeasure,
                          BMS_ELEC_CAN_ID, BMS_ELEC_SW_PDU,
                          BMS_ELEC_DLC, s_sduElecMeasure);

        /* Check electrical faults */
        BmsFault_CheckElec(s_lastVoltMv, s_lastCurrRaw);
    }
    else
    {
        status = E_NOT_OK;
        BmsFault_SetCommError(BMS_FAULT_SRC_INA_COMM, TRUE, (uint8)retV);
    }

    return status;
}
//void BmsApp_Task50ms(void)
//{
//    INA219_ReturnType retV;
//    INA219_ReturnType retI;
//    INA219_ReturnType retP;
//    uint16            pow_raw;
//
//    pow_raw = 0U;
//
//    /* Sensor đã tự scale raw → engineering units (float). Không cần
//     * thêm tầng integer API ở CDD -- BmsSoc_Update sẽ tự cast 1 lần ở
//     * biên rồi tích phân integer trong nội bộ. */
//    retV = CDD_INA219_ReadBusVoltage_mV(&s_lastVoltV); // v -> mV
//    retI = CDD_INA219_ReadCurrent_mA(&s_lastCurrMa);
//    retP = CDD_INA219_ReadPower(&s_lastPowMw); // mW->CDD_INA219_ReadPower
//
//    /* Cache statuses so the debugger can see why a frame did/didn't go out. */
//    s_lastInaStatusV = (uint8)retV;
//    s_lastInaStatusI = (uint8)retI;
//    s_lastInaStatusP = (uint8)retP;
//
//    if ((retV == INA219_OK) && (retI == INA219_OK) && (retP == INA219_OK))
//    {
//        float32 virt_curr_mA;
//        float32 virt_pow_mW;
//        sint32  curr_raw_s32;
//        sint32  pow_raw_s32;
//
//        /* Read OK -- clear any previously-latched INA comm error so the
//         * HMI banner auto-clears as soon as the bus recovers. */
//        BmsFault_SetCommError(BMS_FAULT_SRC_INA_COMM, FALSE, 0U);
//
//        /* ────── Unidir → bidir virtual mapping (demo biến trở) ──────
//         * Raw current INA219 luôn dương; map qua zero-point để có
//         * khái niệm sạc/xả ảo. Khi BMS_SOC_BIDIR_SIM_ENABLE = STD_OFF
//         * (gắn pin thật), helper trả về raw passthrough. */
//        virt_curr_mA = BmsSoc_VirtualCurrent_mA(s_lastCurrMa);
//
//        /* Virtual power = virtual_I × V (signed): dương = xả, âm = sạc.
//         * Dùng |virtual_pow| cho averaging prediction. */
//        //virt_pow_mW  = virt_curr_mA * s_lastVoltV;
//
//        /* CAN encoding cho 0x100 -- defensive saturation, dùng VIRTUAL
//         * values để HMI thấy chiều sạc/xả đúng cho demo:
//         *   volt: 0.001 V/LSB → s_lastVoltMv = V × 1000 (uint16).
//         *   curr: 0.1 mA/LSB  → virtual mA × 10 (sint16 range).
//         *   pow : 0.1 mW/LSB  → |virtual mW| × 10 (uint16 range). */
//        s_lastVoltMv  = (uint16)(s_lastVoltV * BMS_V_TO_MV_SCALE);
//
//    // cmt for test
////        curr_raw_s32 = (sint32)(virt_curr_mA * BMS_MA_TO_RAW_SCALE);
////        if (curr_raw_s32 >  32767) { curr_raw_s32 =  32767; }
////        if (curr_raw_s32 < -32768) { curr_raw_s32 = -32768; }
////        s_lastCurrRaw = (sint16)curr_raw_s32;
////
////        pow_raw_s32 = (sint32)((virt_pow_mW >= 0.0f ? virt_pow_mW : -virt_pow_mW)
////                                * BMS_MW_TO_RAW_SCALE);
////        if (pow_raw_s32 > 65535) { pow_raw_s32 = 65535; }
////        if (pow_raw_s32 < 0)     { pow_raw_s32 = 0;     }
////        pow_raw = (uint16)pow_raw_s32;
////
////        /* Update debug mirror để S32DS Expressions hiển thị giá trị ảo */
//        s_lastCurrMa = virt_curr_mA;   /* shadow original raw with virtual */
////        s_lastPowMw  = virt_pow_mW;
//
//        /* Feed integrator với VIRTUAL current — SoC tăng/giảm theo demo */
//        BmsSoc_Update(virt_curr_mA);
//        //BmsSoc_Update(s_lastCurrMa);
//
//        /* Feed 1-minute power moving-average với |VIRTUAL power| (magnitude)
//         * — divisor cho TTE/TTF luôn dương. */
//        BmsSoc_AccumulatePower(virt_pow_mW >= 0.0f ? virt_pow_mW : -virt_pow_mW);
//
//        BmsApp_PackElecMeasure(s_lastVoltMv, s_lastCurrRaw, pow_raw);
//        BmsApp_TxBmsFrame(Can_43_FLEXCANConf_CanHardwareObject_HOH_Tx_ElecMeasure,
//                          BMS_ELEC_CAN_ID, BMS_ELEC_SW_PDU,
//                          BMS_ELEC_DLC, s_sduElecMeasure);
//
//        BmsFault_CheckElec(s_lastVoltMv, s_lastCurrRaw);
//    }
//    else
//    {
//        /* INA219 I2C error -- raise dedicated comm fault (HMI will NOT
//         * misinterpret as overtemperature). */
//        BmsFault_SetCommError(BMS_FAULT_SRC_INA_COMM, TRUE, (uint8)retV);
//    }
//}

/**
 * @brief   See BmsApp.h.
 */

Std_ReturnType BmsApp_Task100ms(void)
{
    float32 socPercent;
    uint8 soc_percent;
    uint8 soc_raw;          /* Thêm biến raw (0-200) cho CAN */
    Std_ReturnType status = E_OK;

    /* Lấy SOC từ BMS_SoC (trả về 0-100%) */
    soc_percent = BMS_SoC_Get();
    s_lastSocRaw = soc_percent;  /* Lưu lại dạng percent cho debug */
    socPercent = (float32)soc_percent;

    /* Chuyển đổi từ percent (0-100) sang raw (0-200) cho CAN 0x300 byte 0 */
    /* Công thức: soc_raw = soc_percent × 2 */
    soc_raw = soc_percent * 2U;

    /* Giới hạn an toàn (phòng trường hợp lỗi) */
    if (soc_raw > 200U)
    {
        soc_raw = 200U;
    }

    /* Lưu SOC vào Flash nếu cần (vẫn dùng percent) */
    BmsApp_NvmSaveSocIfNeeded(socPercent);

    /* Pack và gửi CAN 0x300 (SOC) - dùng soc_raw (0-200) */
    BmsApp_PackSoc(soc_raw, s_lastCurrRaw);  /* ← sửa thành soc_raw */
    BmsApp_TxBmsFrame(Can_43_FLEXCANConf_CanHardwareObject_HOH_Tx_SOC,
                      BMS_SOC_CAN_ID, BMS_SOC_SW_PDU,
                      BMS_SOC_DLC, s_sduSoc);

    /* Pack và gửi CAN 0x210 (Voltage) - không thay đổi */
    BmsApp_PackVoltage(s_lastVoltMv);
    BmsApp_TxBmsFrame(Can_43_FLEXCANConf_CanHardwareObject_HOH_Tx_Voltage,
                      BMS_VOLT_CAN_ID, BMS_VOLT_SW_PDU,
                      BMS_VOLT_DLC, s_sduVoltage);

    return status;
}
//void BmsApp_Task100ms(void)
//{
//	float32 socPercent;
//
//    /* SOC is now produced by the Coulomb-counting integrator (BmsSoc_Update
//     * fed every 50 ms in Task50ms). Just snapshot the cached raw value. */
//    s_lastSocRaw = BmsSoc_GetSocRaw();
//
//    /* Convert raw (0-200) to percent (0-100) for NVM storage */
//    socPercent = (float32)s_lastSocRaw / 2.0f;
//
//    BmsApp_NvmSaveSocIfNeeded(socPercent);
//
//    BmsApp_PackSoc(s_lastSocRaw, s_lastCurrRaw);
//    BmsApp_TxBmsFrame(Can_43_FLEXCANConf_CanHardwareObject_HOH_Tx_SOC,
//                      BMS_SOC_CAN_ID, BMS_SOC_SW_PDU,
//                      BMS_SOC_DLC, s_sduSoc);
//
//    BmsApp_PackVoltage(s_lastVoltMv);
//    BmsApp_TxBmsFrame(Can_43_FLEXCANConf_CanHardwareObject_HOH_Tx_Voltage,
//                      BMS_VOLT_CAN_ID, BMS_VOLT_SW_PDU,
//                      BMS_VOLT_DLC, s_sduVoltage);
//}

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

    /* Cache the status code so it's visible in the debugger every 200 ms. */
    s_lastTempStatus = (uint8)retT;

    if (retT == STM32_TEMP_OK)
    {
        /* Read OK -- clear STM32 comm latch (auto-cancels yellow strip). */
        BmsFault_SetCommError(BMS_FAULT_SRC_STM_COMM, FALSE, 0U);

        temp_raw = (uint8)((temp_C + BMS_TEMP_OFFSET_C) / BMS_TEMP_LSB_C);

        /* Cache decoded values for debugger watch. */
        s_lastTempC   = temp_C;
        s_lastTempRaw = temp_raw;

        BmsApp_PackTemperature(temp_raw, BMS_TEMP_STATUS_OK);
        BmsFault_CheckTemp(temp_C, temp_raw);
    }
    else
    {
        BmsApp_PackTemperature(0U, BMS_TEMP_STATUS_FAIL);
        /* STM32 I2C / CRC error -- raise STM32 COMM fault. */
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
    float32 remainingHours;
    uint8 soc_percent;
    uint16 tte_min;
    uint16 ttf_min;
    uint8 valid_flags;
    float32 curr_mA;
    Std_ReturnType status = E_OK;

    CDD_INA219_ReadCurrent_mA(&curr_mA);

    /* Lấy SOC hiện tại */
    soc_percent = BMS_SoC_Get();

    /* Lấy thời gian còn lại (giờ) từ BMS_SoC */
    remainingHours = BMS_SoC_GetRemainingHours(curr_mA);

    /* Khởi tạo mặc định */
    tte_min = 0xFFFFU;
    ttf_min = 0xFFFFU;
    valid_flags = 0x00U;

    /* Kiểm tra ý nghĩa của remainingHours */
    if (remainingHours >= 1e5f)  /* Dòng = 0 hoặc rất nhỏ → không xác định */
    {
        tte_min = 0xFFFFU;
        ttf_min = 0xFFFFU;
        valid_flags = 0x00U;
    }
    else if (remainingHours < 0.0f)  /* Không còn dung lượng (chỉ xảy ra khi pin hết và vẫn xả) */
    {
        tte_min = 0xFFFFU;
        ttf_min = 0xFFFFU;
        valid_flags = 0x00U;
    }
    else
    {
        /* remainingHours luôn DƯƠNG - đó là TTE hoặc TTF */
        /* Cần phân biệt đang xả hay sạc để gán vào đúng field */
        float32 mappedCurrent_A = BMS_SoC_MapCurrent(curr_mA);

        if (mappedCurrent_A > 0.0f)  /* Đang xả */
        {
            tte_min = (uint16)(remainingHours * 60.0f);  /* giờ → phút */
            if (tte_min > 65534U)
            {
            	tte_min = 65534U;
            }
            valid_flags = 0x01U;  /* TTE valid */
        }
        else if (mappedCurrent_A < 0.0f)  /* Đang sạc */
        {
            ttf_min = (uint16)(remainingHours * 60.0f);  /* giờ → phút */
            if (ttf_min > 65534U)
            {
            	ttf_min = 65534U;
            }
            valid_flags = 0x02U;  /* TTF valid */
        }
        /* mappedCurrent_A == 0 đã được xử lý ở trên */
    }

    /* Đóng gói CAN 0x400 (6 bytes) */
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
//void BmsApp_Task500ms(void)
//{
//    //BmsSoc_PredictionPack(s_lastVoltV, s_lastCurrMa, s_lastPowMw,
//      //                    s_lastSocRaw, s_sduPrediction);
//	BmsSoc_PredictionPack(s_lastCurrMa, s_lastSocRaw, s_sduPrediction);
//    BmsApp_TxBmsFrame(Can_43_FLEXCANConf_CanHardwareObject_HOH_Tx_Prediction,
//                      BMS_PREDICTION_CAN_ID, BMS_PREDICTION_SW_PDU,
//                      BMS_PRED_DLC, s_sduPrediction);
//}

/*============================================================================*/
/*                            NVM Integration                                 */
/*============================================================================*/


/**
 * @brief   Lưu SOC vào Flash nếu cần (thay đổi đủ ngưỡng và đủ thời gian)
 */
void BmsApp_NvmSaveSocIfNeeded(float32 currentSocPercent)
{
    float32 diff;
    boolean shouldSave;

    /* Chỉ lưu khi NVM đang IDLE (không có operation nào đang chạy) */
    if (BMS_Nvm_GetStatus() != NVM_RET_OK)
    {
        return;  /* Busy, chờ lần sau */
    }

    /* Tính độ chênh lệch SOC so với lần lưu cuối */
    diff = currentSocPercent - s_lastSavedSocPercent;
    if (diff < 0.0f)
    {
        diff = -diff;  /* Giá trị tuyệt đối */
    }

    /* Kiểm tra điều kiện lưu:
       1. SOC thay đổi >= ngưỡng (1%)
    */
    shouldSave = (diff >= BMS_NVM_SAVE_THRESHOLD_PCT);

    if (shouldSave)
    {
        /* Yêu cầu ghi async vào Flash */
        BMS_Nvm_WriteSoC_Async(currentSocPercent);

        /* Cập nhật state */
        s_lastSavedSocPercent = currentSocPercent;
    }
}

float32 BmsSoc_VirtualCurrent_mA(float32 raw_mA)
{
    float32 v;

#if (BMS_SOC_BIDIR_SIM_ENABLE == STD_ON)
    v = (raw_mA > BMS_SOC_BIDIR_ZERO_POINT_mA) ? (raw_mA - BMS_SOC_BIDIR_ZERO_POINT_mA) : -raw_mA;
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

/*******************************************************************************
* AUTOSAR CanIf callbacks (referenced from generated FlexCAN config)
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
 *
 * @details Increments transmission counter and sets TX flag for super-loop
 *          observation. Conforms to AUTOSAR SWS_CANIF prototype.
 */
void CanIf_TxConfirmation(PduIdType CanTxPduId)
{
    (void)CanTxPduId;
    s_canTxConfirmCnt++;
    s_canTxFlag = TRUE;
}

/**
 * @brief   FlexCAN RX-indication callback (interrupt context).
 *
 * @details Dispatches HMI commands received on CAN 0x710 byte 0.
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
