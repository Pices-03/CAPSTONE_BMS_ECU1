/**
 * @file       CDD_INA219_Types.h
 * @version    1.0.0
 * @brief      Public type definitions for INA219 driver.
 *
 * @details    Defines return code enum used by every CDD_INA219_Read*() and
 *             CDD_INA219_Init() function. Separated from CDD_INA219.h so
 *             upper-layer modules (e.g. BmsFault) can depend only on types
 *             without pulling the full driver API.
 */

#ifndef _CDD_INA219_TYPES_H_
#define _CDD_INA219_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Std_Types.h"

/*******************************************************************************
* Definitions
*******************************************************************************/

/**
 * @brief   Return codes for INA219 driver functions.
 * @details Values are ordered by error severity (0 = success).
 *          Use INA219_OK to mean transaction completed and reading is valid.
 */
typedef enum
{
    INA219_OK            = 0U,  /**< I2C transaction completed, data valid.       */
    INA219_ERR_TIMEOUT   = 1U,  /**< I2C bus did not complete within timeout.     */
    INA219_ERR_PARAM     = 2U   /**< NULL_PTR passed to an output parameter.      */
} INA219_ReturnType;

#ifdef __cplusplus
}
#endif

#endif /* _CDD_INA219_TYPES_H_ */
