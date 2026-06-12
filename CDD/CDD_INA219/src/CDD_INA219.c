///**
// * @file       CDD_INA219.c
// * @version    1.0.0
// * @brief      CDD driver implementation for INA219 power monitor via I2C.
// *
// * @details    Provides register-level read/write access to INA219 for:
// *               - Bus voltage measurement (0-26 V)
// *               - Shunt voltage measurement (+/- 40 mV)
// *               - Current measurement (signed)
// *               - Power measurement
// *             Hardware: LPI2C0 PTA2/PTA3, 100 kHz, slave 0x40.
// *             Platform: NXP S32K144, AUTOSAR RTD, bare-metal.
// *
// * @note       Static buffers are placed in the I2C MemMap section because
// *             AUTOSAR I2c LLD performs DMA / aligned access on them.
// *             I2c_RequestType uses positional initialization to avoid
// *             dependency on struct member names (which differ between RTD
// *             versions: SlaveAddrSize10bits vs BitsSlaveAddressSize, etc.).
// */
//
///*******************************************************************************
//* Includes
//*******************************************************************************/
//#include "Std_Types.h"
//#include "CDD_INA219.h"
//#include "CDD_I2c.h"
//
///*******************************************************************************
//* Definitions
//*******************************************************************************/
//
///**
// * @brief   Size of the TX register-write transaction (1 reg-ptr + 2 data).
// */
//#define INA219_TX_BUF_SIZE           (3U)
//
///**
// * @brief   Size of the RX register-read transaction (2 data bytes, MSB first).
// */
//#define INA219_RX_BUF_SIZE           (2U)
//
///**
// * @brief   Size of the register-pointer write before a read (1 byte).
// */
//#define INA219_REG_PTR_SIZE          (1U)
//
///*******************************************************************************
//* Prototypes
//*******************************************************************************/
//
//static INA219_ReturnType INA219_WriteReg(uint8 regAddr, uint16 value);
//static INA219_ReturnType INA219_ReadReg(uint8 regAddr, uint16 *pOutRaw);
//
///*******************************************************************************
//* Variables
//*******************************************************************************/
//
//#define I2C_START_SEC_VAR_INIT_UNSPECIFIED
//#include "I2c_MemMap.h"
//
///**
// * @brief   Shared TX buffer for INA219 I2C transactions.
// * @details Holds register pointer (byte 0) and optional 16-bit value (bytes 1-2).
// */
//static I2c_DataType s_ina219TxBuf[INA219_TX_BUF_SIZE] = {0U, 0U, 0U};
//
///**
// * @brief   RX buffer for register-read transactions (big-endian, MSB first).
// */
//static I2c_DataType s_ina219RxBuf[INA219_RX_BUF_SIZE] = {0U, 0U};
//
//#define I2C_STOP_SEC_VAR_INIT_UNSPECIFIED
//#include "I2c_MemMap.h"
//
///*******************************************************************************
//* Code
//*******************************************************************************/
//
///**
// * @brief   Write a 16-bit value to an INA219 register.
// *
// * @details Builds a 3-byte transaction (reg pointer + MSB + LSB) and sends
// *          asynchronously through the AUTOSAR I2c driver, then polls
// *          I2c_GetStatus() until the channel is no longer in I2C_CH_SEND.
// *          Returns INA219_ERR_TIMEOUT if the polling counter expires.
// *
// * @param[in] regAddr  Target INA219 register (see INA219_REG_xxx in _Cfg.h).
// * @param[in] value    16-bit value to write (big-endian on the wire).
// *
// * @return  INA219_OK           Send completed before timeout.
// * @return  INA219_ERR_TIMEOUT  Bus did not finish within INA219_I2C_TIMEOUT.
// */
//static INA219_ReturnType INA219_WriteReg(uint8 regAddr, uint16 value)
//{
//    /* Positional init: SlaveAddr, AddrSize10bit, ExpectNack, RepStart, BufSize, Direction, Buffer */
//    I2c_RequestType req =
//    {
//        INA219_I2C_ADDR, FALSE, FALSE, FALSE, INA219_TX_BUF_SIZE, I2C_SEND_DATA, s_ina219TxBuf
//    };
//    uint32            timeout;
//    I2c_StatusType    status;
//    INA219_ReturnType ret;
//
//    timeout = INA219_I2C_TIMEOUT;
//    ret     = INA219_OK;
//
//    s_ina219TxBuf[0U] = regAddr;
//    s_ina219TxBuf[1U] = (uint8)(value >> 8U);
//    s_ina219TxBuf[2U] = (uint8)(value & 0xFFU);
//
//    (void)I2c_AsyncTransmit(INA219_I2C_CHANNEL, &req);
//
//    do
//    {
//        status = I2c_GetStatus(INA219_I2C_CHANNEL);
//        timeout--;
//    } while ((status == I2C_CH_SEND) && (timeout > 0U));
//
//    if (timeout == 0U)
//    {
//        ret = INA219_ERR_TIMEOUT;
//    }
//
//    return ret;
//}
//
///**
// * @brief   Read a 16-bit value from an INA219 register.
// *
// * @details Two-step transaction:
// *            Step 1 -- write 1-byte register pointer.
// *            Step 2 -- read 2 bytes (big-endian raw value).
// *          Both steps are polled with the same INA219_I2C_TIMEOUT budget
// *          and either step's timeout returns INA219_ERR_TIMEOUT.
// *
// * @param[in]  regAddr   Target INA219 register.
// * @param[out] pOutRaw   Pointer to receive the 16-bit raw register value.
// *                        Must not be NULL_PTR.
// *
// * @return  INA219_OK           Both steps completed before timeout.
// * @return  INA219_ERR_TIMEOUT  Either write-pointer or read step timed out.
// * @return  INA219_ERR_PARAM    pOutRaw == NULL_PTR.
// */
//static INA219_ReturnType INA219_ReadReg(uint8 regAddr, uint16 *pOutRaw)
//{
//    I2c_RequestType reqPtr =
//    {
//        INA219_I2C_ADDR, FALSE, FALSE, FALSE, INA219_REG_PTR_SIZE, I2C_SEND_DATA, s_ina219TxBuf
//    };
//    I2c_RequestType reqRead =
//    {
//        INA219_I2C_ADDR, FALSE, FALSE, FALSE, INA219_RX_BUF_SIZE, I2C_RECEIVE_DATA, s_ina219RxBuf
//    };
//    uint32            timeout;
//    I2c_StatusType    status;
//    INA219_ReturnType ret;
//
//    ret = INA219_OK;
//
//    if (pOutRaw == NULL_PTR)
//    {
//        ret = INA219_ERR_PARAM;
//    }
//    else
//    {
//        /* Step 1: write register pointer */
//        s_ina219TxBuf[0U] = regAddr;
//
//        timeout = INA219_I2C_TIMEOUT;
//        (void)I2c_AsyncTransmit(INA219_I2C_CHANNEL, &reqPtr);
//        do
//        {
//            status = I2c_GetStatus(INA219_I2C_CHANNEL);
//            timeout--;
//        } while ((status == I2C_CH_SEND) && (timeout > 0U));
//
//        if (timeout == 0U)
//        {
//            ret = INA219_ERR_TIMEOUT;
//        }
//        else
//        {
//            /* Step 2: read 2 bytes */
//            timeout = INA219_I2C_TIMEOUT;
//            (void)I2c_AsyncTransmit(INA219_I2C_CHANNEL, &reqRead);
//            do
//            {
//                status = I2c_GetStatus(INA219_I2C_CHANNEL);
//                timeout--;
//            } while ((status == I2C_CH_RECEIVE) && (timeout > 0U));
//
//            if (timeout == 0U)
//            {
//                ret = INA219_ERR_TIMEOUT;
//            }
//            else
//            {
//                *pOutRaw = (uint16)(((uint16)s_ina219RxBuf[0U] << 8U) | (uint16)s_ina219RxBuf[1U]);
//            }
//        }
//    }
//
//    return ret;
//}
//
///**
// * @brief   See CDD_INA219.h.
// */
//void CDD_INA219_Init(void)
//{
//    /* Best-effort init -- error path is reported on first read. */
//    (void)CDD_INA219_Reset();
//    (void)INA219_WriteReg(INA219_REG_CALIBRATION, INA219_CALIB_VAL);
//    (void)INA219_WriteReg(INA219_REG_CONFIG, INA219_CONFIG_VAL);
//}
//
///**
// * @brief   See CDD_INA219.h.
// */
//INA219_ReturnType CDD_INA219_Reset(void)
//{
//    return INA219_WriteReg(INA219_REG_CONFIG, INA219_RESET_VAL);
//}
//
///**
// * @brief   See CDD_INA219.h.
// */
//boolean CDD_INA219_IsConnected(void)
//{
//    INA219_ReturnType ret;
//    uint16            dummy;
//    boolean           alive;
//
//    dummy = 0U;
//    alive = FALSE;
//
//    ret = INA219_ReadReg(INA219_REG_CONFIG, &dummy);
//    if (ret == INA219_OK)
//    {
//        alive = TRUE;
//    }
//
//    return alive;
//}
//
///**
// * @brief   See CDD_INA219.h.
// */
//INA219_ReturnType CDD_INA219_ReadAll(float32 *pOutVolt_V,
//                                     float32 *pOutCurr_mA,
//                                     float32 *pOutPower_mW,
//                                     float32 *pOutShunt_mV)
//{
//    INA219_ReturnType ret;
//
//    ret = INA219_OK;
//
//    if ((pOutVolt_V   == NULL_PTR) || (pOutCurr_mA  == NULL_PTR) ||
//        (pOutPower_mW == NULL_PTR) || (pOutShunt_mV == NULL_PTR))
//    {
//        ret = INA219_ERR_PARAM;
//    }
//    else
//    {
//        ret = CDD_INA219_ReadBusVoltage_V(pOutVolt_V);
//        if (ret == INA219_OK)
//        {
//            ret = CDD_INA219_ReadCurrent_mA(pOutCurr_mA);
//        }
//        if (ret == INA219_OK)
//        {
//            ret = CDD_INA219_ReadPower_mW(pOutPower_mW);
//        }
//        if (ret == INA219_OK)
//        {
//            ret = CDD_INA219_ReadShuntVoltage_mV(pOutShunt_mV);
//        }
//    }
//
//    return ret;
//}
//
///**
// * @brief   See CDD_INA219.h.
// */
//INA219_ReturnType CDD_INA219_ReadCurrent_mA(float32 *pOutCurrent_mA)
//{
//    INA219_ReturnType ret;
//    uint16            raw;
//    sint16            signedRaw;
//
//    raw = 0U;
//
//    if (pOutCurrent_mA == NULL_PTR)
//    {
//        ret = INA219_ERR_PARAM;
//    }
//    else
//    {
//        ret = INA219_ReadReg(INA219_REG_CURRENT, &raw);
//        if (ret == INA219_OK)
//        {
//            signedRaw       = (sint16)raw;
//            *pOutCurrent_mA = (float32)signedRaw * INA219_CURRENT_LSB_MA;
//        }
//    }
//
//    return ret;
//}
//
///**
// * @brief   See CDD_INA219.h.
// */
//INA219_ReturnType CDD_INA219_ReadBusVoltage_V(float32 *pOutVoltage_V)
//{
//    INA219_ReturnType ret;
//    uint16            raw;
//
//    raw = 0U;
//
//    if (pOutVoltage_V == NULL_PTR)
//    {
//        ret = INA219_ERR_PARAM;
//    }
//    else
//    {
//        ret = INA219_ReadReg(INA219_REG_BUS, &raw);
//        if (ret == INA219_OK)
//        {
//            /* Bit 15:3 = value, bit 1 = CNVR, bit 0 = OVF */
//            *pOutVoltage_V = (float32)(raw >> 3U) * INA219_BUS_LSB_V;
//        }
//    }
//
//    return ret;
//}
//
///**
// * @brief   See CDD_INA219.h.
// */
//INA219_ReturnType CDD_INA219_ReadShuntVoltage_mV(float32 *pOutShunt_mV)
//{
//    INA219_ReturnType ret;
//    uint16            raw;
//    sint16            signedRaw;
//
//    raw = 0U;
//
//    if (pOutShunt_mV == NULL_PTR)
//    {
//        ret = INA219_ERR_PARAM;
//    }
//    else
//    {
//        ret = INA219_ReadReg(INA219_REG_SHUNT, &raw);
//        if (ret == INA219_OK)
//        {
//            signedRaw     = (sint16)raw;
//            *pOutShunt_mV = (float32)signedRaw * INA219_SHUNT_LSB_MV;
//        }
//    }
//
//    return ret;
//}
//
///**
// * @brief   See CDD_INA219.h.
// */
//INA219_ReturnType CDD_INA219_ReadPower_mW(float32 *pOutPower_mW)
//{
//    INA219_ReturnType ret;
//    uint16            raw;
//
//    raw = 0U;
//
//    if (pOutPower_mW == NULL_PTR)
//    {
//        ret = INA219_ERR_PARAM;
//    }
//    else
//    {
//        ret = INA219_ReadReg(INA219_REG_POWER, &raw);
//        if (ret == INA219_OK)
//        {
//            *pOutPower_mW = (float32)raw * INA219_POWER_LSB_MW;
//        }
//    }
//
//    return ret;
//}


/*==================================================================================================
* Project : BMS AUTOSAR Demo
* Platform : CORTEXM
* Peripheral : S32K144
* Component : CDD_INA219
* Module : CDD_INA219.c
* Description : Implementation file for INA219 Current/Voltage Monitor Driver (Interrupt Mode)
*==================================================================================================*/

#include "CDD_INA219.h"
#include "CDD_I2c.h"

/*==================================================================================================
* LOCAL DEFINITIONS
==================================================================================================*/
#define INA219_TX_BUF_SIZE           (3U)
#define INA219_RX_BUF_SIZE           (2U)
#define INA219_REG_PTR_SIZE          (1U)
#define INA219_I2C_TIMEOUT           (0xFFFFU)

/*==================================================================================================
* LOCAL VARIABLES
==================================================================================================*/
static uint8 s_ina219TxBuffer[INA219_TX_BUF_SIZE];
static uint8 s_ina219RxBuffer[INA219_RX_BUF_SIZE];

/*==================================================================================================
* LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static Std_ReturnType INA219_WriteRegister(uint8 RegAddr, uint16 Value);
static Std_ReturnType INA219_ReadRegister(uint8 RegAddr, uint16 *ValuePtr);

/*==================================================================================================
* GLOBAL FUNCTIONS
==================================================================================================*/

/**
 * @brief Initialize INA219 and load calibration values
 */
Std_ReturnType CDD_INA219_Init(void)
{
    Std_ReturnType Status = E_OK;

    /* 1. Reset the device first */
    Status = CDD_INA219_Reset();

    if (E_OK == Status)
    {
        /* 2. Write Configuration Register */
        Status = INA219_WriteRegister(INA219_REG_CONFIG, INA219_CONFIG_DEFAULT);
    }

    if (E_OK == Status)
    {
        /* 3. Write Calibration Register to enable current/power measurements */
        Status = INA219_WriteRegister(INA219_REG_CALIBRATION, INA219_CALIBRATION_VALUE);
    }

    return Status;
}

/**
 * @brief Read current value in MiliAmperes
 */
Std_ReturnType CDD_INA219_ReadCurrent_mA(float32 *CurrentPtr)
{
    Std_ReturnType Status = E_OK;
    uint16 RawCurrent = 0;

    if (NULL_PTR == CurrentPtr)
    {
        Status = E_NOT_OK;
    }
    else
    {
        Status = INA219_ReadRegister(INA219_REG_CURRENT, &RawCurrent);
        if (E_OK == Status)
        {
            /* Convert raw data to mA using the Actual LSB from Cfg */
            *CurrentPtr = (float32)((sint16)RawCurrent) * INA219_ACTUAL_CURRENT_LSB * 1000.0f;
        }
    }

    return Status;
}

/**
 * @brief Read bus voltage (Battery Voltage) in MiliVolts
 */
Std_ReturnType CDD_INA219_ReadBusVoltage_mV(float32 *VoltagePtr)
{
    Std_ReturnType Status = E_OK;
    uint16 RawVoltage = 0;

    if (NULL_PTR == VoltagePtr)
    {
        Status = E_NOT_OK;
    }
    else
    {
        Status = INA219_ReadRegister(INA219_REG_BUS_VOLTAGE, &RawVoltage);
        if (E_OK == Status)
        {
            /* Bus Voltage is stored in bits 3-15. Shift right by 3.
               Each LSB is 4mV as defined in Cfg */
            RawVoltage = (RawVoltage >> 3);
            *VoltagePtr = (float32)RawVoltage * INA219_VOLTAGE_LSB_MV;
        }
    }

    return Status;
}

/**
 * @brief Read power value in Watts
 */
Std_ReturnType CDD_INA219_ReadPower(float32 *PowerPtr)
{
    Std_ReturnType Status = E_OK;
    uint16 RawPower = 0;

    if (NULL_PTR == PowerPtr)
    {
        return E_NOT_OK;
    }

    Status = INA219_ReadRegister(INA219_REG_POWER, &RawPower);
    if (E_OK == Status)
    {
        /* Power = RawPower × Current_LSB × 32 (from datasheet) */
        *PowerPtr = (float32)RawPower * INA219_ACTUAL_CURRENT_LSB * 32.0f;
    }
    return Status;
}

/**
 * @brief Read shunt voltage value in Volts
 */
Std_ReturnType CDD_INA219_ReadShuntVoltage(float32 *ShuntVoltagePtr)
{
    Std_ReturnType Status = E_OK;
    uint16 RawShunt = 0;

    if (NULL_PTR == ShuntVoltagePtr)
    {
        return E_NOT_OK;
    }

    Status = INA219_ReadRegister(INA219_REG_SHUNT_VOLTAGE, &RawShunt);
    if (E_OK == Status)
    {
        /* Shunt Voltage: 10μV per LSB (from datasheet) */
        *ShuntVoltagePtr = (float32)((sint16)RawShunt) * 0.00001f;
    }
    return Status;
}

/**
 * @brief Read all values (voltage, current, power, shunt voltage)
 */
Std_ReturnType CDD_INA219_ReadAll(float32 *VoltagePtr, float32 *CurrentPtr,
                                   float32 *PowerPtr, float32 *ShuntVoltagePtr)
{
    Std_ReturnType Status = E_OK;

    /* Check all pointers first */
    if ((NULL_PTR == VoltagePtr) || (NULL_PTR == CurrentPtr) ||
        (NULL_PTR == PowerPtr)   || (NULL_PTR == ShuntVoltagePtr))
    {
        return E_NOT_OK;
    }

    /* Sequential read with error checking */
    Status = CDD_INA219_ReadBusVoltage_mV(VoltagePtr);

    if (E_OK == Status)
    {
        Status = CDD_INA219_ReadCurrent_mA(CurrentPtr);
    }

    if (E_OK == Status)
    {
        Status = CDD_INA219_ReadPower(PowerPtr);
    }

    if (E_OK == Status)
    {
        Status = CDD_INA219_ReadShuntVoltage(ShuntVoltagePtr);
    }

    return Status;
}

/**
 * @brief Reset device
 */
Std_ReturnType CDD_INA219_Reset(void)
{
    /* Write 1 to bit 15 of Config Register to trigger reset */
    return INA219_WriteRegister(INA219_REG_CONFIG, 0x8000U);
}

/**
 * @brief Check connection by reading the Config register
 */
boolean CDD_INA219_IsConnected(void)
{
    uint16 DummyValue = 0;
    return (INA219_ReadRegister(INA219_REG_CONFIG, &DummyValue) == E_OK);
}

/*==================================================================================================
* LOCAL FUNCTIONS (Async + Polling - giống code cũ)
==================================================================================================*/

/**
 * @brief Write a 16-bit value to an INA219 register (async + polling)
 */
static Std_ReturnType INA219_WriteRegister(uint8 RegAddr, uint16 Value)
{
    I2c_RequestType Request = {0};
    uint32 timeout = 0U;
    I2c_StatusType status = I2C_CH_IDLE;
    Std_ReturnType ret = E_OK;

    /* Prepare TX buffer: register address + MSB + LSB */
    s_ina219TxBuffer[0] = RegAddr;
    s_ina219TxBuffer[1] = (uint8)(Value >> 8U);
    s_ina219TxBuffer[2] = (uint8)(Value & 0xFFU);

    /* Setup I2C request */
    Request.SlaveAddress = (I2c_AddressType)INA219_I2C_ADDRESS;
    Request.BitsSlaveAddressSize = FALSE;
    Request.ExpectNack = FALSE;
    Request.RepeatedStart = FALSE;
    Request.BufferSize = INA219_TX_BUF_SIZE;
    Request.DataDirection = I2C_SEND_DATA;
    Request.DataBuffer = s_ina219TxBuffer;

    /* Start async transmission */
    (void)I2c_AsyncTransmit(INA219_I2C_CHANNEL, &Request);

    /* Poll until transmission completes or timeout */
    timeout = INA219_I2C_TIMEOUT;
    do
    {
        status = I2c_GetStatus(INA219_I2C_CHANNEL);
        timeout--;
    } while ((status == I2C_CH_SEND) && (timeout > 0U));

    if (timeout == 0U)
    {
        ret = E_NOT_OK;
    }

    return ret;
}

/**
 * @brief Read a 16-bit value from an INA219 register (async + polling)
 */
static Std_ReturnType INA219_ReadRegister(uint8 RegAddr, uint16 *ValuePtr)
{
    I2c_RequestType Request = {0};
    uint32 timeout = 0U;
    I2c_StatusType status = I2C_CH_IDLE;
    Std_ReturnType ret = E_OK;

    if (ValuePtr == NULL_PTR)
    {
        ret = E_NOT_OK;
    }
    else
    {
        /* ========== STEP 1: Write register pointer ========== */
        s_ina219TxBuffer[0] = RegAddr;

        Request.SlaveAddress = (I2c_AddressType)INA219_I2C_ADDRESS;
        Request.BitsSlaveAddressSize = FALSE;
        Request.ExpectNack = FALSE;
        Request.RepeatedStart = TRUE;  /* Use Repeated Start for read */
        Request.BufferSize = INA219_REG_PTR_SIZE;
        Request.DataDirection = I2C_SEND_DATA;
        Request.DataBuffer = s_ina219TxBuffer;

        (void)I2c_AsyncTransmit(INA219_I2C_CHANNEL, &Request);

        /* Poll until transmission completes */
        timeout = INA219_I2C_TIMEOUT;
        do
        {
            status = I2c_GetStatus(INA219_I2C_CHANNEL);
            timeout--;
        } while ((status == I2C_CH_SEND) && (timeout > 0U));

        if (timeout == 0U)
        {
            ret = E_NOT_OK;
        }
        else
        {
            /* ========== STEP 2: Read 2 bytes of data ========== */
            Request.RepeatedStart = FALSE;
            Request.BufferSize = INA219_RX_BUF_SIZE;
            Request.DataDirection = I2C_RECEIVE_DATA;
            Request.DataBuffer = s_ina219RxBuffer;

            (void)I2c_AsyncTransmit(INA219_I2C_CHANNEL, &Request);

            /* Poll until receive completes */
            timeout = INA219_I2C_TIMEOUT;
            do
            {
                status = I2c_GetStatus(INA219_I2C_CHANNEL);
                timeout--;
            } while ((status == I2C_CH_RECEIVE) && (timeout > 0U));

            if (timeout == 0U)
            {
                ret = E_NOT_OK;
            }
            else
            {
                /* Combine MSB and LSB into 16-bit value */
                *ValuePtr = (uint16)(((uint16)s_ina219RxBuffer[0] << 8U) | (uint16)s_ina219RxBuffer[1]);
            }
        }
    }

    return ret;
}
