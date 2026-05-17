/*==================================================================================================
*   Project              : RTD AUTOSAR 4.7
*   Platform             : CORTEXM
*   Peripheral           : Ftm_Srtc_Lptmr_LPit
*   Dependencies         : none
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

#ifndef SRTC_IP_TRUSTEDFUNCTIONS_H
#define SRTC_IP_TRUSTEDFUNCTIONS_H

/**
*   @file SRtc_Ip_TrustedFunctions.h
*
*   @addtogroup srtc_ip sRtc IPL
*   @{
*/

#ifdef __cplusplus
extern "C"
{
#endif

/*==================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
==================================================================================================*/
#include "SRtc_Ip_Cfg.h"
#if (STD_ON == SRTC_IP_ENABLE_USER_MODE_SUPPORT)
#include "Reg_eSys.h"
#endif
/*==================================================================================================
                               SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define SRTC_IP_TRUSTEDFUNCTIONS_VENDOR_ID                           43
#define SRTC_IP_TRUSTEDFUNCTIONS_AR_RELEASE_MAJOR_VERSION            4
#define SRTC_IP_TRUSTEDFUNCTIONS_AR_RELEASE_MINOR_VERSION            7
#define SRTC_IP_TRUSTEDFUNCTIONS_AR_RELEASE_REVISION_VERSION         0
#define SRTC_IP_TRUSTEDFUNCTIONS_SW_MAJOR_VERSION                    3
#define SRTC_IP_TRUSTEDFUNCTIONS_SW_MINOR_VERSION                    0
#define SRTC_IP_TRUSTEDFUNCTIONS_SW_PATCH_VERSION                    0

/*==================================================================================================
                                      FILE VERSION CHECKS
==================================================================================================*/
/* Check if this header file and Define header file are of the same vendor */
#if (SRTC_IP_TRUSTEDFUNCTIONS_VENDOR_ID != SRTC_IP_VENDOR_ID_CFG)
    #error "SRtc_Ip_TrustedFunctions.h and SRtc_Ip_Cfg.h have different vendor IDs"
#endif
/* Check if this header  file and Define header file are of the same AutoSar version */
#if ((SRTC_IP_TRUSTEDFUNCTIONS_AR_RELEASE_MAJOR_VERSION      != SRTC_IP_AR_RELEASE_MAJOR_VERSION_CFG) || \
     (SRTC_IP_TRUSTEDFUNCTIONS_AR_RELEASE_MINOR_VERSION      != SRTC_IP_AR_RELEASE_MINOR_VERSION_CFG) || \
     (SRTC_IP_TRUSTEDFUNCTIONS_AR_RELEASE_REVISION_VERSION   != SRTC_IP_AR_RELEASE_REVISION_VERSION_CFG) \
    )
    #error "AutoSar Version Numbers of SRtc_Ip_TrustedFunctions.h and SRtc_Ip_Cfg.h are different"
#endif
/* Check if source file and Define header file are of the same Software version */
#if ((SRTC_IP_TRUSTEDFUNCTIONS_SW_MAJOR_VERSION  != SRTC_IP_SW_MAJOR_VERSION_CFG) || \
     (SRTC_IP_TRUSTEDFUNCTIONS_SW_MINOR_VERSION  != SRTC_IP_SW_MINOR_VERSION_CFG) || \
     (SRTC_IP_TRUSTEDFUNCTIONS_SW_PATCH_VERSION  != SRTC_IP_SW_PATCH_VERSION_CFG)    \
    )
    #error "Software Version Numbers of SRtc_Ip_TrustedFunctions.h and SRtc_Ip_Cfg.h are different"
#endif

#ifndef DISABLE_MCAL_INTERMODULE_ASR_CHECK
    #if (STD_ON == SRTC_IP_ENABLE_USER_MODE_SUPPORT)
        /* Check if header file and Reg_eSys.h file are of the same Autosar version */
        #if ((SRTC_IP_TRUSTEDFUNCTIONS_AR_RELEASE_MAJOR_VERSION != REG_ESYS_AR_RELEASE_MAJOR_VERSION) || \
             (SRTC_IP_TRUSTEDFUNCTIONS_AR_RELEASE_MINOR_VERSION != REG_ESYS_AR_RELEASE_MINOR_VERSION)    \
            )
            #error "AutoSar Version Numbers of SRtc_Ip_TrustedFunctions.h and Reg_eSys.h are different"
        #endif
    #endif
#endif
/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/

/*==================================================================================================
*                                        GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                             ENUMS
==================================================================================================*/

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/


/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
#define GPT_START_SEC_CODE
#include "Gpt_MemMap.h"

/**
 * @brief      SRtc_Ip_SetUserAccessAllowed
 * @details    This function allow registers are accessible in user mode is called externally by OS Application
 * @param[in]  none
 */
#if (defined(MCAL_SRTC_REG_PROT_AVAILABLE))
#if ((STD_ON == MCAL_SRTC_REG_PROT_AVAILABLE) && (STD_ON == SRTC_IP_ENABLE_USER_MODE_SUPPORT))
    extern void Srtc_Ip_SetUserAccessAllowed(void);
#endif
#endif

#define GPT_STOP_SEC_CODE
#include "Gpt_MemMap.h"


#ifdef __cplusplus
}
#endif

/** @} */

#endif  /* RTC_IP_TRUSTEDFUNCTIONS_H */
