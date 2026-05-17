/**
 * @file       CDD_STM32Temp_Types.h
 * @version    1.0.0
 * @brief      Public type definitions for STM32 temperature sensor driver.
 *
 * @details    Defines the return code enum used by CDD_STM32Temp_Read().
 *             Separated from CDD_STM32Temp.h so upper-layer modules can
 *             include only types without pulling the full driver API.
 */

#ifndef _CDD_STM32TEMP_TYPES_H_
#define _CDD_STM32TEMP_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Std_Types.h"

/*******************************************************************************
* Definitions
*******************************************************************************/

/**
 * @brief   Return codes for STM32 temperature sensor driver.
 * @details Values are ordered by error severity (0 = success).
 *          The numeric values are also exposed via fault payload byte 2 of
 *          BMS_Fault (CAN 0x101) when a temperature read fails.
 */
typedef enum
{
    STM32_TEMP_OK            = 0U,  /**< Packet received and all checks passed.            */
    STM32_TEMP_ERR_TIMEOUT   = 1U,  /**< I2C bus did not respond within timeout.           */
    STM32_TEMP_ERR_HEADER    = 2U,  /**< Byte[0] != 0xAA -- slave desync or noise.         */
    STM32_TEMP_ERR_CRC       = 3U,  /**< XOR of Byte[0..4] != Byte[5] -- data corrupted.   */
    STM32_TEMP_ERR_RANGE     = 4U,  /**< STATUS byte == 0x01 -- sensor out of range.       */
    STM32_TEMP_ERR_PARAM     = 5U   /**< NULL_PTR passed for output temperature.           */
} STM32_Temp_ReturnType;

#ifdef __cplusplus
}
#endif

#endif /* _CDD_STM32TEMP_TYPES_H_ */
