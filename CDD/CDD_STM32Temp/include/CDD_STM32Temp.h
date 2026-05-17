/**
 * @file       CDD_STM32Temp.h
 * @version    1.0.0
 * @brief      Public API of CDD STM32 temperature sensor driver.
 *
 * @details    Reads a 6-byte packet from an STM32 slave that samples an NTC
 *             thermistor and reports temperature x 10 (deg C). Validates
 *             header, status byte and XOR-CRC before exposing the value.
 *             Hardware: NXP S32K144, LPI2C0 @ 100 kHz, slave 0x10.
 *
 * @note       Caller must ensure I2c_Init() has been called.
 *             Not interrupt-safe -- do not call from ISR context.
 */

#ifndef _CDD_STM32TEMP_H_
#define _CDD_STM32TEMP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Std_Types.h"
#include "CDD_STM32Temp_Types.h"
#include "CDD_STM32Temp_Cfg.h"

/*******************************************************************************
* API
*******************************************************************************/

/**
 * @brief   Initialize the STM32 temperature sensor driver.
 *
 * @details The STM32 slave self-updates its TX buffer every 50 ms and exposes
 *          no slave-side configuration register, so this function is empty.
 *          Kept for symmetry with CDD_INA219_Init() and as a hook for future
 *          slave-side handshaking.
 *
 * @pre     I2c_Init() has been called. LPI2C0 bus is idle.
 *
 * @param   void
 * @return  void
 *
 * @post    No hardware state change. Driver internal buffer is unchanged.
 */
void CDD_STM32Temp_Init(void);

/**
 * @brief   Read the next 6-byte packet from the STM32 slave and decode it.
 *
 * @details Single I2C read transaction (no register-pointer write -- the
 *          slave streams its packet on every read). On success, validates:
 *            - Byte[0] == STM32_TEMP_HEADER_VAL
 *            - XOR of Byte[0..4] == Byte[5]
 *            - Byte[4] == STM32_TEMP_STATUS_OK
 *          and writes (TEMP_H << 8 | TEMP_L) / 10.0f to *pOutTemperature_C.
 *
 * @pre     CDD_STM32Temp_Init() has been called. I2C bus is idle.
 *
 * @param[out] pOutTemperature_C  Pointer to float32 to receive result (deg C).
 *                                 On STM32_TEMP_OK : valid temperature written.
 *                                 Otherwise         : output unchanged.
 *                                 Must not be NULL_PTR.
 *
 * @return  STM32_TEMP_OK            Packet received, all checks passed.
 * @return  STM32_TEMP_ERR_TIMEOUT   I2C bus did not finish within timeout.
 * @return  STM32_TEMP_ERR_HEADER    Header byte mismatched 0xAA.
 * @return  STM32_TEMP_ERR_CRC       Computed CRC mismatched packet CRC byte.
 * @return  STM32_TEMP_ERR_RANGE     STATUS byte signalled OutOfRange.
 * @return  STM32_TEMP_ERR_PARAM     pOutTemperature_C == NULL_PTR.
 *
 * @post    Internal RX buffer always reflects the latest received bytes
 *          (visible to debugger), regardless of return code.
 */
STM32_Temp_ReturnType CDD_STM32Temp_Read(float32 *pOutTemperature_C);

#ifdef __cplusplus
}
#endif

#endif /* _CDD_STM32TEMP_H_ */
