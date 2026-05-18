/**
 * @file    BMS_Nvm.h
 * @brief   Non-Volatile Memory management for BMS SoC storage
 * @details Provides asynchronous read/write operations for storing State of Charge (SoC)
 *          in Flash memory using Mem_43_INFLS driver. Supports non-blocking write operations
 *          to avoid disrupting the main control loop.
 * @project BMS AUTOSAR Demo
 * @platform S32K144
 * @date    2026-05-16
 */

#ifndef BMS_NVM_H
#define BMS_NVM_H

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
 *                                        INCLUDE FILES
 *==================================================================================================*/
#include "Std_Types.h"

/*==================================================================================================
 *                                   MEMORY ADDRESS CONFIGURATION
 *==================================================================================================*/

/**
 * @brief Flash memory address for SoC storage
 * @note  Must be aligned to sector boundary and not overlap with code section
 *        Current address 0x10000000 is for testing only - Update to safe region before production
 *        Recommended: Use dedicated EEPROM emulation area or last sectors of Flash
 */
#define BMS_NVM_SOC_ADDR        0x10000000U

/**
 * @brief Flash sector size in bytes
 * @note  S32K144 Flash sector size is typically 2KB (2048 bytes)
 *        Erase operation must be performed on full sector boundaries
 */
#define BMS_NVM_SECTOR_SIZE     2048U

/**
 * @brief Flash page size for write operation in bytes
 * @note  Minimum writable unit. Write operation requires this alignment
 */
#define BMS_NVM_PAGE_SIZE       8U

/*==================================================================================================
 *                                   RETURN CODES ENUMERATION
 *==================================================================================================*/

/**
 * @brief Return status codes for NVM operations
 */
typedef enum {
    NVM_RET_OK = 0,         /**< Operation completed successfully or request accepted */
    NVM_RET_BUSY,           /**< Driver is busy processing previous job */
    NVM_RET_ERROR,          /**< Hardware or driver error occurred */
    NVM_RET_PENDING         /**< Asynchronous job is currently in progress */
} BMS_Nvm_ReturnType;

/*==================================================================================================
 *                                   FUNCTION PROTOTYPES
 *==================================================================================================*/

/**
 * @brief Initialize the NVM module and Mem_43_INFLS driver
 * @details Must be called once at system startup before any other NVM operations
 * @return void
 */
void BMS_Nvm_Init(void);

/**
 * @brief Read stored SoC value from Flash memory (blocking operation)
 * @details Reads 8 bytes from Flash at configured address and interprets as float32.
 *          This function blocks until the read operation completes.
 *          Called only during system initialization, so blocking is acceptable.
 * @return Current SoC value in percent (0.0 - 100.0). Returns 100.0 if read fails or data invalid
 */
float32 BMS_Nvm_ReadSoC(void);

/**
 * @brief Request asynchronous write of SoC value to Flash memory
 * @details Non-blocking function that initiates erase and write sequence.
 *          Returns immediately after starting the operation.
 *          Call BMS_Nvm_MainFunction periodically to process the operation.
 * @param socValue SoC value in percent (0.0 - 100.0) to be stored
 * @return NVM_RET_OK if operation started successfully
 * @return NVM_RET_BUSY if previous operation still in progress
 * @return NVM_RET_ERROR if erase initiation failed
 */
BMS_Nvm_ReturnType BMS_Nvm_WriteSoC_Async(float32 socValue);

/**
 * @brief State machine handler for asynchronous NVM operations
 * @details Must be called periodically from main loop to:
 *          - Check completion status of erase/write jobs
 *          - Transition between ERASING and WRITING states
 *          - Handle job completion and errors
 * @return void
 */
void BMS_Nvm_MainFunction(void);

/**
 * @brief Get current status of NVM module
 * @return NVM_RET_OK if idle and ready for new operation
 * @return NVM_RET_PENDING if an asynchronous operation is in progress
 */
BMS_Nvm_ReturnType BMS_Nvm_GetStatus(void);

#ifdef __cplusplus
}
#endif

#endif /* BMS_NVM_H */
