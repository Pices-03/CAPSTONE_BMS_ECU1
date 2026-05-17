/**
 * @file       CDD_INA219.c
 * @version    1.0.0
 * @brief      CDD driver implementation for INA219 power monitor via I2C.
 *
 * @details    Provides register-level read/write access to INA219 for:
 *               - Bus voltage measurement (0-26 V)
 *               - Shunt voltage measurement (+/- 40 mV)
 *               - Current measurement (signed)
 *               - Power measurement
 *             Hardware: LPI2C0 PTA2/PTA3, 100 kHz, slave 0x40.
 *             Platform: NXP S32K144, AUTOSAR RTD, bare-metal.
 *
 * @note       Static buffers are placed in the I2C MemMap section because
 *             AUTOSAR I2c LLD performs DMA / aligned access on them.
 *             I2c_RequestType uses positional initialization to avoid
 *             dependency on struct member names (which differ between RTD
 *             versions: SlaveAddrSize10bits vs BitsSlaveAddressSize, etc.).
 */

/*******************************************************************************
* Includes
*******************************************************************************/
#include "Std_Types.h"
#include "CDD_INA219.h"
#include "CDD_I2c.h"

/*******************************************************************************
* Definitions
*******************************************************************************/

/**
 * @brief   Size of the TX register-write transaction (1 reg-ptr + 2 data).
 */
#define INA219_TX_BUF_SIZE           (3U)

/**
 * @brief   Size of the RX register-read transaction (2 data bytes, MSB first).
 */
#define INA219_RX_BUF_SIZE           (2U)

/**
 * @brief   Size of the register-pointer write before a read (1 byte).
 */
#define INA219_REG_PTR_SIZE          (1U)

/*******************************************************************************
* Prototypes
*******************************************************************************/

static INA219_ReturnType INA219_WriteReg(uint8 regAddr, uint16 value);
static INA219_ReturnType INA219_ReadReg(uint8 regAddr, uint16 *pOutRaw);

/*******************************************************************************
* Variables
*******************************************************************************/

#define I2C_START_SEC_VAR_INIT_UNSPECIFIED
#include "I2c_MemMap.h"

/**
 * @brief   Shared TX buffer for INA219 I2C transactions.
 * @details Holds register pointer (byte 0) and optional 16-bit value (bytes 1-2).
 */
static I2c_DataType s_ina219TxBuf[INA219_TX_BUF_SIZE] = {0U, 0U, 0U};

/**
 * @brief   RX buffer for register-read transactions (big-endian, MSB first).
 */
static I2c_DataType s_ina219RxBuf[INA219_RX_BUF_SIZE] = {0U, 0U};

#define I2C_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "I2c_MemMap.h"

/*******************************************************************************
* Code
*******************************************************************************/

/**
 * @brief   Write a 16-bit value to an INA219 register.
 *
 * @details Builds a 3-byte transaction (reg pointer + MSB + LSB) and sends
 *          asynchronously through the AUTOSAR I2c driver, then polls
 *          I2c_GetStatus() until the channel is no longer in I2C_CH_SEND.
 *          Returns INA219_ERR_TIMEOUT if the polling counter expires.
 *
 * @param[in] regAddr  Target INA219 register (see INA219_REG_xxx in _Cfg.h).
 * @param[in] value    16-bit value to write (big-endian on the wire).
 *
 * @return  INA219_OK           Send completed before timeout.
 * @return  INA219_ERR_TIMEOUT  Bus did not finish within INA219_I2C_TIMEOUT.
 */
static INA219_ReturnType INA219_WriteReg(uint8 regAddr, uint16 value)
{
    /* Positional init: SlaveAddr, AddrSize10bit, ExpectNack, RepStart, BufSize, Direction, Buffer */
    I2c_RequestType req =
    {
        INA219_I2C_ADDR, FALSE, FALSE, FALSE, INA219_TX_BUF_SIZE, I2C_SEND_DATA, s_ina219TxBuf
    };
    uint32            timeout;
    I2c_StatusType    status;
    INA219_ReturnType ret;

    timeout = INA219_I2C_TIMEOUT;
    ret     = INA219_OK;

    s_ina219TxBuf[0U] = regAddr;
    s_ina219TxBuf[1U] = (uint8)(value >> 8U);
    s_ina219TxBuf[2U] = (uint8)(value & 0xFFU);

    (void)I2c_AsyncTransmit(INA219_I2C_CHANNEL, &req);

    do
    {
        status = I2c_GetStatus(INA219_I2C_CHANNEL);
        timeout--;
    } while ((status == I2C_CH_SEND) && (timeout > 0U));

    if (timeout == 0U)
    {
        ret = INA219_ERR_TIMEOUT;
    }

    return ret;
}

/**
 * @brief   Read a 16-bit value from an INA219 register.
 *
 * @details Two-step transaction:
 *            Step 1 -- write 1-byte register pointer.
 *            Step 2 -- read 2 bytes (big-endian raw value).
 *          Both steps are polled with the same INA219_I2C_TIMEOUT budget
 *          and either step's timeout returns INA219_ERR_TIMEOUT.
 *
 * @param[in]  regAddr   Target INA219 register.
 * @param[out] pOutRaw   Pointer to receive the 16-bit raw register value.
 *                        Must not be NULL_PTR.
 *
 * @return  INA219_OK           Both steps completed before timeout.
 * @return  INA219_ERR_TIMEOUT  Either write-pointer or read step timed out.
 * @return  INA219_ERR_PARAM    pOutRaw == NULL_PTR.
 */
static INA219_ReturnType INA219_ReadReg(uint8 regAddr, uint16 *pOutRaw)
{
    I2c_RequestType reqPtr =
    {
        INA219_I2C_ADDR, FALSE, FALSE, FALSE, INA219_REG_PTR_SIZE, I2C_SEND_DATA, s_ina219TxBuf
    };
    I2c_RequestType reqRead =
    {
        INA219_I2C_ADDR, FALSE, FALSE, FALSE, INA219_RX_BUF_SIZE, I2C_RECEIVE_DATA, s_ina219RxBuf
    };
    uint32            timeout;
    I2c_StatusType    status;
    INA219_ReturnType ret;

    ret = INA219_OK;

    if (pOutRaw == NULL_PTR)
    {
        ret = INA219_ERR_PARAM;
    }
    else
    {
        /* Step 1: write register pointer */
        s_ina219TxBuf[0U] = regAddr;

        timeout = INA219_I2C_TIMEOUT;
        (void)I2c_AsyncTransmit(INA219_I2C_CHANNEL, &reqPtr);
        do
        {
            status = I2c_GetStatus(INA219_I2C_CHANNEL);
            timeout--;
        } while ((status == I2C_CH_SEND) && (timeout > 0U));

        if (timeout == 0U)
        {
            ret = INA219_ERR_TIMEOUT;
        }
        else
        {
            /* Step 2: read 2 bytes */
            timeout = INA219_I2C_TIMEOUT;
            (void)I2c_AsyncTransmit(INA219_I2C_CHANNEL, &reqRead);
            do
            {
                status = I2c_GetStatus(INA219_I2C_CHANNEL);
                timeout--;
            } while ((status == I2C_CH_RECEIVE) && (timeout > 0U));

            if (timeout == 0U)
            {
                ret = INA219_ERR_TIMEOUT;
            }
            else
            {
                *pOutRaw = (uint16)(((uint16)s_ina219RxBuf[0U] << 8U) | (uint16)s_ina219RxBuf[1U]);
            }
        }
    }

    return ret;
}

/**
 * @brief   See CDD_INA219.h.
 */
void CDD_INA219_Init(void)
{
    /* Best-effort init -- error path is reported on first read. */
    (void)CDD_INA219_Reset();
    (void)INA219_WriteReg(INA219_REG_CALIBRATION, INA219_CALIB_VAL);
    (void)INA219_WriteReg(INA219_REG_CONFIG, INA219_CONFIG_VAL);
}

/**
 * @brief   See CDD_INA219.h.
 */
INA219_ReturnType CDD_INA219_Reset(void)
{
    return INA219_WriteReg(INA219_REG_CONFIG, INA219_RESET_VAL);
}

/**
 * @brief   See CDD_INA219.h.
 */
boolean CDD_INA219_IsConnected(void)
{
    INA219_ReturnType ret;
    uint16            dummy;
    boolean           alive;

    dummy = 0U;
    alive = FALSE;

    ret = INA219_ReadReg(INA219_REG_CONFIG, &dummy);
    if (ret == INA219_OK)
    {
        alive = TRUE;
    }

    return alive;
}

/**
 * @brief   See CDD_INA219.h.
 */
INA219_ReturnType CDD_INA219_ReadAll(float32 *pOutVolt_V,
                                     float32 *pOutCurr_mA,
                                     float32 *pOutPower_mW,
                                     float32 *pOutShunt_mV)
{
    INA219_ReturnType ret;

    ret = INA219_OK;

    if ((pOutVolt_V   == NULL_PTR) || (pOutCurr_mA  == NULL_PTR) ||
        (pOutPower_mW == NULL_PTR) || (pOutShunt_mV == NULL_PTR))
    {
        ret = INA219_ERR_PARAM;
    }
    else
    {
        ret = CDD_INA219_ReadBusVoltage_V(pOutVolt_V);
        if (ret == INA219_OK)
        {
            ret = CDD_INA219_ReadCurrent_mA(pOutCurr_mA);
        }
        if (ret == INA219_OK)
        {
            ret = CDD_INA219_ReadPower_mW(pOutPower_mW);
        }
        if (ret == INA219_OK)
        {
            ret = CDD_INA219_ReadShuntVoltage_mV(pOutShunt_mV);
        }
    }

    return ret;
}

/**
 * @brief   See CDD_INA219.h.
 */
INA219_ReturnType CDD_INA219_ReadCurrent_mA(float32 *pOutCurrent_mA)
{
    INA219_ReturnType ret;
    uint16            raw;
    sint16            signedRaw;

    raw = 0U;

    if (pOutCurrent_mA == NULL_PTR)
    {
        ret = INA219_ERR_PARAM;
    }
    else
    {
        ret = INA219_ReadReg(INA219_REG_CURRENT, &raw);
        if (ret == INA219_OK)
        {
            signedRaw       = (sint16)raw;
            *pOutCurrent_mA = (float32)signedRaw * INA219_CURRENT_LSB_MA;
        }
    }

    return ret;
}

/**
 * @brief   See CDD_INA219.h.
 */
INA219_ReturnType CDD_INA219_ReadBusVoltage_V(float32 *pOutVoltage_V)
{
    INA219_ReturnType ret;
    uint16            raw;

    raw = 0U;

    if (pOutVoltage_V == NULL_PTR)
    {
        ret = INA219_ERR_PARAM;
    }
    else
    {
        ret = INA219_ReadReg(INA219_REG_BUS, &raw);
        if (ret == INA219_OK)
        {
            /* Bit 15:3 = value, bit 1 = CNVR, bit 0 = OVF */
            *pOutVoltage_V = (float32)(raw >> 3U) * INA219_BUS_LSB_V;
        }
    }

    return ret;
}

/**
 * @brief   See CDD_INA219.h.
 */
INA219_ReturnType CDD_INA219_ReadShuntVoltage_mV(float32 *pOutShunt_mV)
{
    INA219_ReturnType ret;
    uint16            raw;
    sint16            signedRaw;

    raw = 0U;

    if (pOutShunt_mV == NULL_PTR)
    {
        ret = INA219_ERR_PARAM;
    }
    else
    {
        ret = INA219_ReadReg(INA219_REG_SHUNT, &raw);
        if (ret == INA219_OK)
        {
            signedRaw     = (sint16)raw;
            *pOutShunt_mV = (float32)signedRaw * INA219_SHUNT_LSB_MV;
        }
    }

    return ret;
}

/**
 * @brief   See CDD_INA219.h.
 */
INA219_ReturnType CDD_INA219_ReadPower_mW(float32 *pOutPower_mW)
{
    INA219_ReturnType ret;
    uint16            raw;

    raw = 0U;

    if (pOutPower_mW == NULL_PTR)
    {
        ret = INA219_ERR_PARAM;
    }
    else
    {
        ret = INA219_ReadReg(INA219_REG_POWER, &raw);
        if (ret == INA219_OK)
        {
            *pOutPower_mW = (float32)raw * INA219_POWER_LSB_MW;
        }
    }

    return ret;
}
