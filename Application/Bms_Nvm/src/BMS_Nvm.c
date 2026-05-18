/**
 * @file    BMS_Nvm.c
 * @brief   Non-Volatile Memory management implementation for BMS SoC storage
 * @details Implements asynchronous write and synchronous read operations for
 *          storing State of Charge (SoC) in Flash memory using Mem_43_INFLS driver.
 *          Uses a simple state machine (IDLE → ERASING → WRITING) for non-blocking writes.
 * @project BMS AUTOSAR Demo
 * @platform S32K144
 * @date    2026-05-16
 */

/*==================================================================================================
 *                                        INCLUDE FILES
 *==================================================================================================*/
#include "BMS_Nvm.h"
#include "Mem_43_INFLS.h"
#include <math.h>

/*==================================================================================================
 *                                   INTERNAL TYPES
 *==================================================================================================*/

/**
 * @brief Internal states of the NVM state machine
 */
typedef enum {
    NVM_STATE_IDLE,     /**< No operation in progress, ready for new request */
    NVM_STATE_ERASING,  /**< Flash erase operation in progress */
    NVM_STATE_WRITING   /**< Flash write operation in progress */
} BMS_Nvm_StateInternalType;

/*==================================================================================================
 *                                   STATIC VARIABLES
 *==================================================================================================*/

/**
 * @brief Current state of the NVM state machine
 */
static BMS_Nvm_StateInternalType Nvm_CurrentState = NVM_STATE_IDLE;

/**
 * @brief SoC value pending to be written (held during erase operation)
 */
static float32 Pending_SoC_Value = 0.0f;

/**
 * @brief Transmit buffer for Flash write operation
 * @details Size defined by BMS_NVM_PAGE_SIZE (8 bytes)
 */
static uint8 txBuffer[BMS_NVM_PAGE_SIZE];

/*==================================================================================================
 *                                   GLOBAL FUNCTIONS
 *==================================================================================================*/

/**
 * @brief Initialize the NVM module and Mem_43_INFLS driver
 */
void BMS_Nvm_Init(void)
{
    Mem_43_INFLS_Init(NULL_PTR);
}

/**
 * @brief Read stored SoC value from Flash memory (blocking operation)
 * @return Current SoC value in percent (0.0 - 100.0). Returns 100.0 if read fails or data invalid
 */
float32 BMS_Nvm_ReadSoC(void)
{
    uint8 rxBuffer[BMS_NVM_PAGE_SIZE];
    float32 readValue = 100.0f;

    /* Initiate read operation */
    if (E_OK == Mem_43_INFLS_Read(MEM_43_INFLS_INSTANCE_0_ID, BMS_NVM_SOC_ADDR, rxBuffer, BMS_NVM_PAGE_SIZE))
    {
        /* Wait for read job to complete (blocking - acceptable during initialization only) */
        do {
            Mem_43_INFLS_MainFunction();
        } while (MEM_43_INFLS_JOB_PENDING == Mem_43_INFLS_GetJobResult(MEM_43_INFLS_INSTANCE_0_ID));

        /* Extract float32 value from buffer */
        readValue = *((float32*)rxBuffer);

        /* Validate read data: Check for NaN and valid range (0-100%) */
        if (isnan(readValue) || readValue > 100.0f || readValue < 0.0f)
        {
            readValue = 100.0f;  /* Return default value on invalid data */
        }
    }

    return readValue;
}

/**
 * @brief Request asynchronous write of SoC value to Flash memory
 * @param socValue SoC value in percent (0.0 - 100.0) to be stored
 * @return NVM_RET_OK if operation started successfully
 * @return NVM_RET_BUSY if previous operation still in progress
 * @return NVM_RET_ERROR if erase initiation failed
 */
BMS_Nvm_ReturnType BMS_Nvm_WriteSoC_Async(float32 socValue)
{
    /* Reject new request if previous operation still in progress */
    if (Nvm_CurrentState != NVM_STATE_IDLE)
    {
        return NVM_RET_BUSY;
    }

    /* Store value for later use during write phase */
    Pending_SoC_Value = socValue;

    /* Start erase operation (must erase full sector before write) */
    if (E_OK == Mem_43_INFLS_Erase(MEM_43_INFLS_INSTANCE_0_ID, BMS_NVM_SOC_ADDR, BMS_NVM_SECTOR_SIZE))
    {
        Nvm_CurrentState = NVM_STATE_ERASING;
        return NVM_RET_OK;
    }

    return NVM_RET_ERROR;
}

/**
 * @brief State machine handler for asynchronous NVM operations
 * @details Must be called periodically from main loop to process erase/write jobs
 */
void BMS_Nvm_MainFunction(void)
{
    /* Run the underlying driver's main function to process pending operations */
    Mem_43_INFLS_MainFunction();
    Mem_43_INFLS_JobResultType jobResult = Mem_43_INFLS_GetJobResult(MEM_43_INFLS_INSTANCE_0_ID);

    switch (Nvm_CurrentState)
    {
        case NVM_STATE_ERASING:
            if (jobResult == MEM_43_INFLS_JOB_OK)
            {
                /* Erase completed successfully - proceed to write phase */
                /* Prepare buffer with the pending SoC value */
                *((float32*)txBuffer) = Pending_SoC_Value;

                /* Start write operation */
                if (E_OK == Mem_43_INFLS_Write(MEM_43_INFLS_INSTANCE_0_ID, BMS_NVM_SOC_ADDR, txBuffer, BMS_NVM_PAGE_SIZE))
                {
                    Nvm_CurrentState = NVM_STATE_WRITING;
                }
                else
                {
                    /* Write initiation failed - abort sequence */
                    Nvm_CurrentState = NVM_STATE_IDLE;
                }
            }
            else if (jobResult == MEM_43_INFLS_JOB_FAILED)
            {
                /* Erase failed - abort sequence, keep previous data */
                Nvm_CurrentState = NVM_STATE_IDLE;
            }
            break;

        case NVM_STATE_WRITING:
            if (jobResult == MEM_43_INFLS_JOB_OK || jobResult == MEM_43_INFLS_JOB_FAILED)
            {
                /* Write completed (success or failure) - sequence finished */
                Nvm_CurrentState = NVM_STATE_IDLE;
            }
            break;

        default:
            /* IDLE state - nothing to process */
            break;
    }
}

/**
 * @brief Get current status of NVM module
 * @return NVM_RET_OK if idle and ready for new operation
 * @return NVM_RET_PENDING if an asynchronous operation is in progress
 */
BMS_Nvm_ReturnType BMS_Nvm_GetStatus(void)
{
    if (Nvm_CurrentState == NVM_STATE_IDLE)
    {
        return NVM_RET_OK;
    }
    return NVM_RET_PENDING;
}
