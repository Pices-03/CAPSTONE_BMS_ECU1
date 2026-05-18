/*==================================================================================================
*   Project              : RTD AUTOSAR 4.7
*   Platform             : CORTEXM
*   Peripheral           : FTFC
*   Dependencies         : None
*
*   Autosar Version      : 4.7.0
*   Autosar Revision     : ASR_REL_4_7_REV_0000
*   Autosar Conf.Variant :
*   SW Version           : 3.0.0
*   Build Version        : S32K1_RTD_3_0_0_QLP06_D2603_ASR_REL_4_7_REV_0000_20260320
*
*   Copyright 2020-2026 NXP
*
*   NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be
*   used strictly in accordance with the applicable license terms. By expressly
*   accepting such terms or by downloading, installing, activating and/or otherwise
*   using the software, you are agreeing that you have read, and that you agree to
*   comply with and are bound by, such license terms. If you do not agree to be
*   bound by the applicable license terms, then you may not retain, install,
*   activate or otherwise use the software.
==================================================================================================*/

#ifndef FTFC_MEM_INFLS_IP_TYPES_H
#define FTFC_MEM_INFLS_IP_TYPES_H

/**
 * @file       Ftfc_Mem_InFls_Ip_Types.h
 *
 * @defgroup   FTFC_MEM_INFLS_IP FTFC IP Driver
 *
 * implement   Ftfc_Mem_InFls_Ip_Types.h_Artifact
 * @{
 */

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "Std_Types.h"

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define FTFC_MEM_INFLS_IP_TYPES_VENDOR_ID                    43
#define FTFC_MEM_INFLS_IP_TYPES_AR_RELEASE_MAJOR_VERSION     4
#define FTFC_MEM_INFLS_IP_TYPES_AR_RELEASE_MINOR_VERSION     7
#define FTFC_MEM_INFLS_IP_TYPES_AR_RELEASE_REVISION_VERSION  0
#define FTFC_MEM_INFLS_IP_TYPES_SW_MAJOR_VERSION             3
#define FTFC_MEM_INFLS_IP_TYPES_SW_MINOR_VERSION             0
#define FTFC_MEM_INFLS_IP_TYPES_SW_PATCH_VERSION             0

/*==================================================================================================
                                      FILE VERSION CHECKS
==================================================================================================*/
#ifndef DISABLE_MCAL_INTERMODULE_ASR_CHECK
    /* Check if current file and Std_Types.h file are of the same version */
    #if ((FTFC_MEM_INFLS_IP_TYPES_AR_RELEASE_MAJOR_VERSION != STD_AR_RELEASE_MAJOR_VERSION) || \
         (FTFC_MEM_INFLS_IP_TYPES_AR_RELEASE_MINOR_VERSION != STD_AR_RELEASE_MINOR_VERSION)\
        )
        #error "AutoSar Version Numbers of Ftfc_Mem_InFls_Ip_Types.h and Std_Types.h are different"
    #endif
#endif

/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/

/**
 * @brief FCCOB commands IDs
 */
#define FLASH_CMD_PROGRAM_PHRASE   (0x07U)
#define FLASH_CMD_ERASE_SECTOR     (0x09U)

/**
 * @brief  Program alignment
 */
#define FTFC_MEM_INFLS_IP_WRITE_DOUBLE_WORD     8U

/**
* @brief the number of bytes uses to compare (1 byte).
*
*/
#define FTFC_MEM_INFLS_IP_SIZE_1BYTE             1U

/**
* @brief the number of bytes uses to compare (2 bytes).
*
*/
#define FTFC_MEM_INFLS_IP_SIZE_2BYTE             2U

/**
* @brief the number of bytes uses to compare (4 bytes).
*
*/
#define FTFC_MEM_INFLS_IP_SIZE_4BYTE             4U

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

typedef uint32 Ftfc_Mem_InFls_Ip_AddressType;
typedef uint32 Ftfc_Mem_InFls_Ip_LengthType;


/**
    @brief Enumeration of Blocks of memory flash .
*/
typedef enum
{
    FTFC_MEM_INFLS_IP_CODE_BLOCK_0    = 0x00U,  /*!< code block 0 */
    FTFC_MEM_INFLS_IP_CODE_BLOCK_1    = 0x01U,  /*!< code block 1 */
    FTFC_MEM_INFLS_IP_CODE_BLOCK_2    = 0x02U,  /*!< code block 2 */
    FTFC_MEM_INFLS_IP_DATA_BLOCK    = 0x03U,  /*!< data block */
    FTFC_MEM_INFLS_IP_BLOCK_INVALID = 0xFFU   /*!< invalid block */
} Ftfc_Mem_InFls_Ip_FlashBlocksNumberType;

/**
* @brief          Start Flash Access Notification Pointer Type
* @details        Pointer type of Ftfc_Mem_InFls_Ip_StartFlashAccessNotifPtrType function
*
*/
typedef void (*Ftfc_Mem_InFls_Ip_StartFlashAccessNotifPtrType)(void);

/**
* @brief          Finished Flash Access Notification Pointer Type
* @details        Pointer type of Ftfc_Mem_InFls_Ip_FinishedFlashAccessNotifPtrType function
*
*/
typedef void (*Ftfc_Mem_InFls_Ip_FinishedFlashAccessNotifPtrType)(void);

/*!
 * @brief Ftfc Configuration Structure
 *
 * Implements : Ftfc_Mem_InFls_Ip_ConfigType_Class
 */
typedef struct
{
    Ftfc_Mem_InFls_Ip_StartFlashAccessNotifPtrType       StartFlashAccessNotifPtr;            /*!< Pointer to start flash access callout  */
    Ftfc_Mem_InFls_Ip_FinishedFlashAccessNotifPtrType    FinishedFlashAccessNotifPtr;         /*!< Pointer to finish flash access callout */
} Ftfc_Mem_InFls_Ip_ConfigType;

/**
    @brief Enumeration of checking status errors or not.
*/
typedef enum
{
    STATUS_FTFC_MEM_INFLS_IP_SUCCESS                   = 0x00U,    /*!< Successful job */
    STATUS_FTFC_MEM_INFLS_IP_BUSY                      = 0x01U,    /*!< IP is performing an operation */
    STATUS_FTFC_MEM_INFLS_IP_ERROR                     = 0x02U,    /*!< Error - general code */
    STATUS_FTFC_MEM_INFLS_IP_ERROR_TIMEOUT             = 0x03U,    /*!< Error - exceeded timeout */
    STATUS_FTFC_MEM_INFLS_IP_ERROR_INPUT_PARAM         = 0x04U,    /*!< Error - wrong input parameter */
    STATUS_FTFC_MEM_INFLS_IP_ERROR_BLANK_CHECK         = 0x05U,    /*!< Error - selected memory area is not erased */
    STATUS_FTFC_MEM_INFLS_IP_ERROR_PROGRAM_VERIFY      = 0x06U,    /*!< Error - selected memory area doesn't contain desired value */
    STATUS_FTFC_MEM_INFLS_IP_ERROR_USER_TEST_BREAK_SBC = 0x07U,    /*!< Error - single bit correction */
    STATUS_FTFC_MEM_INFLS_IP_ERROR_USER_TEST_BREAK_DBD = 0x08U,    /*!< Error - double bit detection */
    STATUS_FTFC_MEM_INFLS_IP_SECTOR_UNPROTECTED        = 0x09U,    /*!< Checked sector is unlocked */
    STATUS_FTFC_MEM_INFLS_IP_SECTOR_PROTECTED          = 0x0AU,    /*!< Checked sector is locked */
    STATUS_FTFC_MEM_INFLS_IP_ECC_UNCORRECTED           = 0x0BU,    /*!< Ecc uncorrected error */
    STATUS_FTFC_MEM_INFLS_IP_ECC_CORRECTED             = 0x0CU     /*!< Ecc corrected error */
} Ftfc_Mem_InFls_Ip_StatusType;

#ifdef __cplusplus
}
#endif

/** @}*/
#endif /* FTFC_MEM_INFLS_IP_TYPES_H */

