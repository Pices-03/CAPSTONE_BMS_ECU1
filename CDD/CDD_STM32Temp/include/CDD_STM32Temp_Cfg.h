/**
 * @file       CDD_STM32Temp_Cfg.h
 * @version    1.0.0
 * @brief      Configuration constants for STM32 temperature-sensor slave driver.
 *
 * @details    The STM32 slave streams a fixed 6-byte packet on every read.
 *             Packet layout (1-based indexing matches §2 of project_context.md):
 *               Byte[0] = 0xAA       Header
 *               Byte[1] = 0x01       Mode
 *               Byte[2] = TEMP_H     Temperature x 10, MSB
 *               Byte[3] = TEMP_L     Temperature x 10, LSB
 *               Byte[4] = STATUS     0x00 = OK, 0x01 = OutOfRange
 *               Byte[5] = CRC        XOR of Byte[0..4]
 *
 * @note       Hardware: NXP S32K144, LPI2C0 PTA2/PTA3 @ 100 kHz.
 *             Slave address 0x10 = OwnAddress1 (32) >> 1.
 */

#ifndef _CDD_STM32TEMP_CFG_
#define _CDD_STM32TEMP_CFG_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Definitions
*******************************************************************************/

/**
 * @brief   I2C master channel index (LPI2C0 -- shared with INA219).
 */
#define STM32_TEMP_I2C_CHANNEL       (0U)

/**
 * @brief   STM32 slave 7-bit address (OwnAddress1 = 32 in slave firmware).
 */
#define STM32_TEMP_SLAVE_ADDR        (0x10U)

/**
 * @brief   Total bytes in one STM32 packet (header + mode + temp + status + crc).
 */
#define STM32_TEMP_PACKET_SIZE       (6U)

/**
 * @brief   Number of bytes covered by the CRC (everything except the CRC byte).
 */
#define STM32_TEMP_CRC_RANGE         (5U)

/**
 * @brief   Indices inside the received STM32 packet.
 */
#define STM32_TEMP_IDX_HEADER        (0U)
#define STM32_TEMP_IDX_MODE          (1U)
#define STM32_TEMP_IDX_TEMP_H        (2U)
#define STM32_TEMP_IDX_TEMP_L        (3U)
#define STM32_TEMP_IDX_STATUS        (4U)
#define STM32_TEMP_IDX_CRC           (5U)

/**
 * @brief   Expected static values inside the STM32 packet.
 */
#define STM32_TEMP_HEADER_VAL        (0xAAU)
#define STM32_TEMP_MODE_VAL          (0x01U)

/**
 * @brief   STATUS byte values used by the STM32 slave firmware.
 */
#define STM32_TEMP_STATUS_OK         (0x00U)
#define STM32_TEMP_STATUS_RANGE      (0x01U)

/**
 * @brief   I2C polling timeout (loop iterations).
 * @details Same value as INA219 to keep behaviour consistent on the shared bus.
 */
#define STM32_TEMP_I2C_TIMEOUT       (0xFFFFU)

/**
 * @brief   Conversion divisor: STM32 sends temperature x 10 (deg C).
 */
#define STM32_TEMP_SCALE             (10.0f)

#ifdef __cplusplus
}
#endif

#endif /* _CDD_STM32TEMP_CFG_ */
