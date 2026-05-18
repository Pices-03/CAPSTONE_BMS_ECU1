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

#ifndef FTFC_MEM_INFLS_IP_TRUSTED_FUNCTIONS_H
#define FTFC_MEM_INFLS_IP_TRUSTED_FUNCTIONS_H

/**
 * @file       Ftfc_Mem_InFls_Ip_TrustedFunctions.h
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
#define FTFC_MEM_INFLS_IP_TRUSTED_FUNCTIONS_VENDOR_ID                       43
#define FTFC_MEM_INFLS_IP_TRUSTED_FUNCTIONS_AR_RELEASE_MAJOR_VERSION        4
#define FTFC_MEM_INFLS_IP_TRUSTED_FUNCTIONS_AR_RELEASE_MINOR_VERSION        7
#define FTFC_MEM_INFLS_IP_TRUSTED_FUNCTIONS_AR_RELEASE_REVISION_VERSION     0
#define FTFC_MEM_INFLS_IP_TRUSTED_FUNCTIONS_SW_MAJOR_VERSION                3
#define FTFC_MEM_INFLS_IP_TRUSTED_FUNCTIONS_SW_MINOR_VERSION                0
#define FTFC_MEM_INFLS_IP_TRUSTED_FUNCTIONS_SW_PATCH_VERSION                0

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/

/* Check if current file and Ftfc_Mem_InFls_Ip_Cfg.h header file are of the same vendor */
#if (FTFC_MEM_INFLS_IP_VENDOR_ID_CFG != FTFC_MEM_INFLS_IP_TRUSTED_FUNCTIONS_VENDOR_ID)
    #error "Ftfc_Mem_InFls_Ip_TrustedFunctions.h and Ftfc_Mem_InFls_Ip_Cfg.h have different vendor ids"
#endif
/* Check if current file and Ftfc_Mem_InFls_Ip_Cfg.h header file are of the same Autosar version */
#if ((FTFC_MEM_INFLS_IP_AR_RELEASE_MAJOR_VERSION_CFG    != FTFC_MEM_INFLS_IP_TRUSTED_FUNCTIONS_AR_RELEASE_MAJOR_VERSION) || \
     (FTFC_MEM_INFLS_IP_AR_RELEASE_MINOR_VERSION_CFG    != FTFC_MEM_INFLS_IP_TRUSTED_FUNCTIONS_AR_RELEASE_MINOR_VERSION) || \
     (FTFC_MEM_INFLS_IP_AR_RELEASE_REVISION_VERSION_CFG != FTFC_MEM_INFLS_IP_TRUSTED_FUNCTIONS_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of Ftfc_Mem_InFls_Ip_TrustedFunctions.h and Ftfc_Mem_InFls_Ip_Cfg.h are different"
#endif
/* Check if current file and Ftfc_Mem_InFls_Ip_Cfg.h header file are of the same Software version */
#if ((FTFC_MEM_INFLS_IP_SW_MAJOR_VERSION_CFG != FTFC_MEM_INFLS_IP_TRUSTED_FUNCTIONS_SW_MAJOR_VERSION) || \
     (FTFC_MEM_INFLS_IP_SW_MINOR_VERSION_CFG != FTFC_MEM_INFLS_IP_TRUSTED_FUNCTIONS_SW_MINOR_VERSION) || \
     (FTFC_MEM_INFLS_IP_SW_PATCH_VERSION_CFG != FTFC_MEM_INFLS_IP_TRUSTED_FUNCTIONS_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of Ftfc_Mem_InFls_Ip_TrustedFunctions.h and Ftfc_Mem_InFls_Ip_Cfg.h are different"
#endif


/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/


/*==================================================================================================
*                                     FUNCTION PROTOTYPES
==================================================================================================*/

#if (FTFC_MEM_INFLS_IP_INVALID_PREBUF_FROM_RAM == STD_ON)
#define MEM_43_INFLS_START_SEC_RAMCODE
#else
#define MEM_43_INFLS_START_SEC_CODE
#endif
#include "Mem_43_INFLS_MemMap.h"

/**
 * @brief        Invalidate prefetch buffer before reading to make sure that the driver always reads the new data from flash
 *
 * @param[in]    void
 * @retval       none
 *
 */
extern void Ftfc_Mem_InFls_Ip_InvalidPrefetchBuff_Ram(void);

#if (FTFC_MEM_INFLS_IP_INVALID_PREBUF_FROM_RAM == STD_ON)
#define MEM_43_INFLS_STOP_SEC_RAMCODE
#else
#define MEM_43_INFLS_STOP_SEC_CODE
#endif
#include "Mem_43_INFLS_MemMap.h"


#ifdef __cplusplus
}
#endif

/** @} */

#endif /* FTFC_MEM_INFLS_IP_TRUSTED_FUNCTIONS_H */
