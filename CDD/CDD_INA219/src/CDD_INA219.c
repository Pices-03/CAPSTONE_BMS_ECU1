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
            /* Convert raw data to mA using the LSB from Cfg */
            *CurrentPtr = (float32)((sint16)RawCurrent) * INA219_CURRENT_LSB * 1000.0f;
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
        *PowerPtr = (float32)RawPower * INA219_CURRENT_LSB * 32.0f;
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
