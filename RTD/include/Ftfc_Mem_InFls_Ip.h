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

#ifndef FTFC_MEM_INFLS_IP_H
#define FTFC_MEM_INFLS_IP_H

/**
 * @file       Ftfc_Mem_InFls_Ip.h
 *
 * @addtogroup FTFC_MEM_INFLS_IP
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
#include "Ftfc_Mem_InFls_Ip_Cfg.h"

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define FTFC_MEM_INFLS_IP_VENDOR_ID                       43
#define FTFC_MEM_INFLS_IP_AR_RELEASE_MAJOR_VERSION        4
#define FTFC_MEM_INFLS_IP_AR_RELEASE_MINOR_VERSION        7
#define FTFC_MEM_INFLS_IP_AR_RELEASE_REVISION_VERSION     0
#define FTFC_MEM_INFLS_IP_SW_MAJOR_VERSION                3
#define FTFC_MEM_INFLS_IP_SW_MINOR_VERSION                0
#define FTFC_MEM_INFLS_IP_SW_PATCH_VERSION                0

/*==================================================================================================
                                      FILE VERSION CHECKS
==================================================================================================*/

/* Check if current file and Ftfc_Mem_InFls_Ip_Cfg.h header file are of the same vendor */
#if (FTFC_MEM_INFLS_IP_VENDOR_ID_CFG != FTFC_MEM_INFLS_IP_VENDOR_ID)
    #error "Ftfc_Mem_InFls_Ip.h and Ftfc_Mem_InFls_Ip_Cfg.h have different vendor ids"
#endif
/* Check if current file and Ftfc_Mem_InFls_Ip_Cfg.h header file are of the same Autosar version */
#if ((FTFC_MEM_INFLS_IP_AR_RELEASE_MAJOR_VERSION_CFG    != FTFC_MEM_INFLS_IP_AR_RELEASE_MAJOR_VERSION) || \
     (FTFC_MEM_INFLS_IP_AR_RELEASE_MINOR_VERSION_CFG    != FTFC_MEM_INFLS_IP_AR_RELEASE_MINOR_VERSION) || \
     (FTFC_MEM_INFLS_IP_AR_RELEASE_REVISION_VERSION_CFG != FTFC_MEM_INFLS_IP_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of Ftfc_Mem_InFls_Ip.h and Ftfc_Mem_InFls_Ip_Cfg.h are different"
#endif
/* Check if current file and Ftfc_Mem_InFls_Ip_Cfg.h header file are of the same Software version */
#if ((FTFC_MEM_INFLS_IP_SW_MAJOR_VERSION_CFG != FTFC_MEM_INFLS_IP_SW_MAJOR_VERSION) || \
     (FTFC_MEM_INFLS_IP_SW_MINOR_VERSION_CFG != FTFC_MEM_INFLS_IP_SW_MINOR_VERSION) || \
     (FTFC_MEM_INFLS_IP_SW_PATCH_VERSION_CFG != FTFC_MEM_INFLS_IP_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of Ftfc_Mem_InFls_Ip.h and Ftfc_Mem_InFls_Ip_Cfg.h are different"
#endif

/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/



/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
#define MEM_43_INFLS_START_SEC_VAR_CLEARED_32
#include "Mem_43_INFLS_MemMap.h"

#if (STD_ON == FTFC_TIMEOUT_SUPERVISION_ENABLED)
extern uint32 Ftfc_Mem_InFls_Ip_u32ElapsedTicks;
extern uint32 Ftfc_Mem_InFls_Ip_u32TimeoutTicks;
extern uint32 Ftfc_Mem_InFls_Ip_u32CurrentTicks;
#endif

#define MEM_43_INFLS_STOP_SEC_VAR_CLEARED_32
#include "Mem_43_INFLS_MemMap.h"


#define MEM_43_INFLS_START_SEC_CONST_UNSPECIFIED
#include "Mem_43_INFLS_MemMap.h"

/* Base address for Flash Memory Module */
extern FTFx_HARDWARE_TYPE * const FTFx_BaseAddress;

#define MEM_43_INFLS_STOP_SEC_CONST_UNSPECIFIED
#include "Mem_43_INFLS_MemMap.h"


/*==================================================================================================
*                                     FUNCTION PROTOTYPES
==================================================================================================*/
#define MEM_43_INFLS_START_SEC_CODE
#include "Mem_43_INFLS_MemMap.h"

/**
 * @brief        Initializes the FTCF module
 *
 * @details      This function will initialize ftfc module and clear all error flags.
 *
 * @param[in]    Ftfc_Mem_InFls_Ip_pInitConfig   Pointer to the driver configuration structure.
 * @return       Ftfc_Mem_InFls_Ip_StatusType
 * @retval       STATUS_FTFC_MEM_INFLS_IP_SUCCESS            Initialization is success
 * @retval       STATUS_FTFC_MEM_INFLS_IP_ERROR_TIMEOUT      Errors Timeout because wait for the Done bit long time
 *
 */
Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_Init(const Ftfc_Mem_InFls_Ip_ConfigType * Ftfc_Mem_InFls_Ip_pInitConfig);

/*!
 * @brief        Abort a program or erase operation
 *
 * @details      This function will abort a program or erase operation in user
 *               mode and clear all PGM, APGM, ERS, AERS, EHV, AEHV bits in MCR,AMCRS registers
 *
 * @return       Ftfc_Mem_InFls_Ip_StatusType
 * @retval       STATUS_FTFC_MEM_INFLS_IP_SUCCESS : The operation is successful.
 * @retval       STATUS_FTFC_MEM_INFLS_IP_ERROR_TIMEOUT: the operation error because wait for the Done bit long time
 *
 */
Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_Abort(void);

/*!
 * @brief        Abort an erase suspend operation
 *
 * @details      This function will abort an erase suspend operation.
 *
 * @return        Ftfc_Mem_InFls_Ip_StatusType
 * @retval        STATUS_FTFC_MEM_INFLS_IP_SUCCESS : The operation is successful.
 * @retval        STATUS_FTFC_MEM_INFLS_IP_ERROR   : Operation failure status.
 *
 */
Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_AbortSuspended(void);

/**
 * @brief        This function fills data to pDestAddressPtr
 *
 * @details      This function fills data to pDestAddressPtr with data from the specified address
 *
 * @param[in]    u32SrcAddress       The start address of the area to be read.
 * @param[in]    pDestAddressPtr     Pointer to the destination of the read.
 * @param[in]    u32Length           Read size
 *
 * @return       Ftfc_Mem_InFls_Ip_StatusType
 * @retval       STATUS_FTFC_MEM_INFLS_IP_SUCCESS             Read performed successfully.
 * @retval       STATUS_FTFC_MEM_INFLS_IP_ERROR_INPUT_PARAM   Input parameters are invalid.
 * @retval       STATUS_FTFC_MEM_INFLS_IP_ERROR               There was an error while reading.
 *
 * @pre          The module has to be initialized and not busy.
 *
 */
Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_Read(Ftfc_Mem_InFls_Ip_AddressType u32SrcAddress,
                                uint8              *pDestAddressPtr,
                                Ftfc_Mem_InFls_Ip_LengthType  u32Length
                                );
/**
 * @brief        Checks that there is the desired data at the specified address
 *
 * @details      Checks that there is the desired data at the specified address.
 *               If the compare is intented to be a blank check, the pSourceAddressPtr should be NULL.
 *
 * @param[in]    u32SrcAddress        The start address of the area to be checked.
 * @param[in]    pCompareAddressPtr   Pointer to the data expected to be read.
 * @param[in]    u32Length            Check size
 *
 * @return       Ftfc_Mem_InFls_Ip_StatusType
 * @retval       STATUS_FTFC_MEM_INFLS_IP_SUCCESS               Read performed successfully.
 * @retval       STATUS_FTFC_MEM_INFLS_IP_ERROR_INPUT_PARAM     Input parameters are invalid.
 * @retval       STATUS_FTFC_MEM_INFLS_IP_ERROR                 There was an error while reading.
 * @retval       STATUS_FTFC_MEM_INFLS_IP_ERROR_PROGRAM_VERIFY  The expected data was not found completely at the specified address
 *
 * @pre          The module has to be initialized and not busy.
 *
 */
Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_Compare(Ftfc_Mem_InFls_Ip_AddressType u32SrcAddress,
                                   const uint8        *pCompareAddressPtr,
                                   Ftfc_Mem_InFls_Ip_LengthType  u32Length
                                   );

/**
 * @brief         Get block number from target address
 *
 * @details       Get block number from target address
 *
 * @param[in]    u32TargetAddress   target address
 *
 * @return       Ftfc_Mem_InFls_Ip_GetBlockNumberFromAddress
 * @retval       The block number which contains the target address.
 *
 */
Ftfc_Mem_InFls_Ip_FlashBlocksNumberType Ftfc_Mem_InFls_Ip_GetBlockNumberFromAddress(uint32 u32TargetAddress);

/**
 * @brief         Accepts and erases a selected program flash or data flash sector if possible
 *
 * @details       Accepts an erase job over one of the sectors if possible.
 *                Starts the high voltage erase and then exits. The status of the hardware erase must
 *                be verified by calling asynchronously the Ftfc_Mem_InFls_Ip_SectorEraseStatus function.
 *                 The Ftfc_Mem_InFls_Ip_SectorErase function shall cover all the available sectors.
 *
 * @param[in]    u32SectorStartAddress   The start address of the sector to be erased.
 *
 * @return       Ftfc_Mem_InFls_Ip_StatusType
 * @retval       STATUS_FTFC_MEM_INFLS_IP_SUCCESS             Hardware erase started successfully
 * @retval       STATUS_FTFC_MEM_INFLS_IP_ERROR_INPUT_PARAM   The selected sector is out of bound
 * @retval       STATUS_FTFC_MEM_INFLS_IP_ERROR               There is another job configured or in progress or
 *                                                 @p The sector is locked by another core or couldn't be unlocked.
 * @retval       STATUS_FTFC_MEM_INFLS_IP_ERROR_TIMEOUT       The erase operation exceeded the timeout - Status value available only if the timeout feature is enabled
 *
 * @pre          The module has to be initialized.
 *
 */
Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_SectorErase(Ftfc_Mem_InFls_Ip_AddressType u32SectorStartAddress);

/**
 * @brief         Checks the status of the hardware erase started by the Ftfc_Mem_InFls_Ip_SectorErase function.
 *
 * @details       Checks the status of the hardware erase started by the Ftfc_Mem_InFls_Ip_SectorErase function.
 *
 * @return       Ftfc_Mem_InFls_Ip_StatusType
 * @retval       STATUS_FTFC_MEM_INFLS_IP_SUCCESS             Erase performed successfully
 * @retval       STATUS_Ftfc_Mem_InFls_Ip_BUSY                Hardware erase is still in progress
 * @retval       STATUS_FTFC_MEM_INFLS_IP_ERROR               There was an error during the hardware erase.
 * @retval       STATUS_FTFC_MEM_INFLS_IP_ERROR_TIMEOUT       The erase operation exceeded the timeout - Status value available only if the timeout feature is enabled.
 * @retval       STATUS_FTFC_MEM_INFLS_IP_ERROR_BLANK_CHECK   The sector was not erased correctly - Status value available only if the blank check feature is enabled
 *
 * @pre          The module has to be initialized.
 *
 */
Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_SectorEraseStatus(void);

/**
 * @brief         Suspend a current operation of Flash erase sector command.
 *
 * @details       Suspend a current operation of Flash erase sector command.
 *
 * @return       Ftfc_Mem_InFls_Ip_StatusType
 * @retval       STATUS_FTFC_MEM_INFLS_IP_SUCCESS             Operation of Flash erase sector suspended successfully
 * @retval       STATUS_FTFC_MEM_INFLS_IP_ERROR               Operation of Flash erase sector suspended fail
 *
 * @pre          The module has to be initialized.
 *
 */
Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_EraseSuspend(void);

/**
 * @brief         Resume a current operation of Flash erase sector command.
 *
 * @details       Resume a current operation of Flash erase sector command.
 *
 * @return       Ftfc_Mem_InFls_Ip_StatusType
 * @retval       STATUS_FTFC_MEM_INFLS_IP_SUCCESS             Operation of Flash erase sector resumed successfully
 * @retval       STATUS_FTFC_MEM_INFLS_IP_ERROR               Operation of Flash erase sector resumed fail
 *
 * @pre          The module has to be initialized.
 *
 */
Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_EraseResume(void);

/**
 * @brief         Writes data into the memory array using the main interface. Initiates the hardware write and then exits.
 *
 * @details       Writes data into the memory array using the main interface. Initiates the hardware write and then exits.
 *                the status of the hardware erase must be verified by calling asynchronously the Ftfc_Mem_InFls_Ip_WriteStatus function.
 *
 * @param[in]    u32DestAddress      The start address of the write, must be aligned with 8 bytes.
 * @param[in]    pSourceAddressPtr   Source program buffer address.
 * @param[in]    u32Length           Size in bytes of the flash region to be programed, must be aligned with 8 bytes and the maximum value is 128 bytes.
 *
 * @return       Ftfc_Mem_InFls_Ip_StatusType
 * @retval       STATUS_FTFC_MEM_INFLS_IP_SUCCESS             Program performed successfully
 * @retval       STATUS_FTFC_MEM_INFLS_IP_ERROR_INPUT_PARAM   The input parameters are invaid.
 * @retval       STATUS_FTFC_MEM_INFLS_IP_ERROR               There is another job configured or in progress or
 *                                                 @p The sector is locked by another core or couldn't be unlocked.
 * @retval       STATUS_FTFC_MEM_INFLS_IP_ERROR_TIMEOUT       The erase operation exceeded the timeout - Status value available only if the timeout feature is enabled
 *
 * @pre          The module has to be initialized.
 *
 */
Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_Write(uint32       u32DestAddress,
                                 const uint8 *pSourceAddressPtr,
                                 uint32       u32Length);

/**
 * @brief         Checks the status of the hardware program started by the FTFC_Ip_Write function.
 *
 * @details       Checks the status of the hardware program started by the FTFC_Ip_Write function.
 *
 * @return       Ftfc_Mem_InFls_Ip_StatusType
 * @retval       STATUS_FTFC_MEM_INFLS_IP_SUCCESS                Program performed successfully
 * @retval       STATUS_Ftfc_Mem_InFls_Ip_BUSY                   Hardware program is still in progress
 * @retval       STATUS_FTFC_MEM_INFLS_IP_ERROR                  There was an error during the hardware program.
 * @retval       STATUS_FTFC_MEM_INFLS_IP_ERROR_TIMEOUT          The program operation exceeded the timeout - Status value available only if the timeout feature is enabled.
 * @retval       STATUS_FTFC_MEM_INFLS_IP_ERROR_PROGRAM_VERIFY   The data was not written corectly into the memory - Status available only of program verify feature is enabled
 *
 * @pre          The module has to be initialized.
 *
 */
Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_WriteStatus(void);

Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_CtrlStatus(void);

void Ftfc_Mem_InFls_Ip_ReportEccUnCorrectedError(void);



/**
 * @brief       Set Value for Ftfc_Mem_InFls_Ip_bAsync
 *
 * @param[in]   bAsync
 */
void Ftfc_Mem_InFls_Ip_SetAsyncMode(const boolean bAsync);


#define MEM_43_INFLS_STOP_SEC_CODE
#include "Mem_43_INFLS_MemMap.h"


#ifdef __cplusplus
}
#endif

/** @} */

#endif /* FTFC_MEM_INFLS_IP_H */
