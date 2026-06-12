/**
 * @file       CDD_STM32Temp.c
 * @version    1.0.0
 * @brief      CDD driver implementation for STM32 temperature-sensor slave.
 *
 * @details    Reads a fixed 6-byte packet over LPI2C0 from an STM32 slave and
 *             decodes header / CRC / status before returning the temperature
 *             value (deg C). The slave self-streams its TX buffer, so no
 *             register-pointer write is needed before the read.
 *
 * @note       Static buffer is placed in the I2C MemMap section.
 *             I2c_RequestType uses positional initialization to stay
 *             insensitive to RTD struct member renames.
 */

/*******************************************************************************
* Includes
*******************************************************************************/
#include "Std_Types.h"
#include "CDD_STM32Temp.h"
#include "CDD_I2c.h"

/*******************************************************************************
* Definitions
*******************************************************************************/

/* All macros are defined in CDD_STM32Temp_Cfg.h and CDD_STM32Temp_Types.h */

/*******************************************************************************
* Prototypes
*******************************************************************************/

static uint8                 STM32Temp_ComputeXorCrc(const I2c_DataType *pData, uint8 len);
static STM32_Temp_ReturnType STM32Temp_I2cRead(void);

/*******************************************************************************
* Variables
*******************************************************************************/

#define I2C_START_SEC_VAR_INIT_UNSPECIFIED
#include "I2c_MemMap.h"

/**
 * @brief   RX buffer that holds the last 6-byte packet from the STM32 slave.
 */
static I2c_DataType s_stm32RxBuf[STM32_TEMP_PACKET_SIZE] = {0U, 0U, 0U, 0U, 0U, 0U};

#define I2C_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "I2c_MemMap.h"

/*******************************************************************************
* Code
*******************************************************************************/

/**
 * @brief   Compute XOR-CRC over the first @p len bytes of @p pData.
 *
 * @details Matches the CRC algorithm running inside the STM32 slave firmware.
 *          Used to validate Byte[5] of the received packet.
 *
 * @param[in] pData  Pointer to byte buffer. Must not be NULL_PTR.
 * @param[in] len    Number of bytes to XOR (typically STM32_TEMP_CRC_RANGE = 5).
 *
 * @return  XOR of all @p len bytes.
 */
static uint8 STM32Temp_ComputeXorCrc(const I2c_DataType *pData, uint8 len)
{
    uint8 crc = 0x00U;
    uint8 i = 0U;

    for (i = 0U; i < len; i++)
    {
        crc ^= pData[i];
    }

    return crc;
}

/**
 * @brief   Issue an I2C read of STM32_TEMP_PACKET_SIZE bytes from the slave.
 *
 * @details Polls I2c_GetStatus() until the channel leaves I2C_CH_RECEIVE or
 *          STM32_TEMP_I2C_TIMEOUT iterations elapse.
 *
 * @return  STM32_TEMP_OK            Bus completed the transfer.
 * @return  STM32_TEMP_ERR_TIMEOUT   Bus did not complete within budget.
 */
static STM32_Temp_ReturnType STM32Temp_I2cRead(void)
{
    /* Positional init: SlaveAddr, AddrSize10bit, ExpectNack, RepStart, BufSize, Direction, Buffer */
    I2c_RequestType reqRead =
    {
        STM32_TEMP_SLAVE_ADDR, FALSE, FALSE, FALSE,
        STM32_TEMP_PACKET_SIZE, I2C_RECEIVE_DATA, s_stm32RxBuf
    };
    uint32                 timeout = STM32_TEMP_I2C_TIMEOUT;
    I2c_StatusType         status  = I2C_CH_IDLE;
    STM32_Temp_ReturnType  ret     = STM32_TEMP_OK;

    (void)I2c_AsyncTransmit(STM32_TEMP_I2C_CHANNEL, &reqRead);
    do
    {
        status = I2c_GetStatus(STM32_TEMP_I2C_CHANNEL);
        timeout--;
    } while ((status == I2C_CH_RECEIVE) && (timeout > 0U));

    if (timeout == 0U)
    {
        ret = STM32_TEMP_ERR_TIMEOUT;
    }

    return ret;
}

/**
 * @brief   See CDD_STM32Temp.h.
 */
void CDD_STM32Temp_Init(void)
{
    /* No slave-side init -- packet streaming is autonomous. */
}

/**
 * @brief   See CDD_STM32Temp.h.
 */
STM32_Temp_ReturnType CDD_STM32Temp_Read(float32 *pOutTemperature_C)
{
    STM32_Temp_ReturnType ret     = STM32_TEMP_OK;
    uint8                 crcCalc = 0U;
    uint16                tempX10 = 0U;

    if (pOutTemperature_C == NULL_PTR)
    {
        ret = STM32_TEMP_ERR_PARAM;
    }
    else
    {
        /* Step 1: I2C transfer */
        ret = STM32Temp_I2cRead();

        if (ret == STM32_TEMP_OK)
        {
            /* Step 2: header check */
            if (s_stm32RxBuf[STM32_TEMP_IDX_HEADER] != STM32_TEMP_HEADER_VAL)
            {
                ret = STM32_TEMP_ERR_HEADER;
            }
        }

        if (ret == STM32_TEMP_OK)
        {
            /* Step 3: XOR-CRC over byte [0..4] */
            crcCalc = STM32Temp_ComputeXorCrc(s_stm32RxBuf, STM32_TEMP_CRC_RANGE);
            if (crcCalc != s_stm32RxBuf[STM32_TEMP_IDX_CRC])
            {
                ret = STM32_TEMP_ERR_CRC;
            }
        }

        if (ret == STM32_TEMP_OK)
        {
            /* Step 4: STATUS byte */
            if (s_stm32RxBuf[STM32_TEMP_IDX_STATUS] != STM32_TEMP_STATUS_OK)
            {
                ret = STM32_TEMP_ERR_RANGE;
            }
        }

        if (ret == STM32_TEMP_OK)
        {
            /* Step 5: decode TEMP_H | TEMP_L -> deg C */
            tempX10 = (uint16)(((uint16)s_stm32RxBuf[STM32_TEMP_IDX_TEMP_H] << 8U)
                               | (uint16)s_stm32RxBuf[STM32_TEMP_IDX_TEMP_L]);
            *pOutTemperature_C = (float32)tempX10 / STM32_TEMP_SCALE;
        }
    }

    return ret;
}
