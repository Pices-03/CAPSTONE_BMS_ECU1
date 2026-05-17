/*==================================================================================================
* Project :             RTD AUTOSAR 4.7
* Platform :            CORTEXM
* Peripheral :          Ftm_Srtc_Lptmr_LPit
* Dependencies :        none
*
* Autosar Version :     4.7.0
* Autosar Revision :    ASR_REL_4_7_REV_0000
* Autosar Conf.Variant :
* SW Version :          3.0.0
* Build Version :       S32K1_RTD_3_0_0_QLP06_D2603_ASR_REL_4_7_REV_0000_20260320
*
* Copyright 2020-2026 NXP
*
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be
* used strictly in accordance with the applicable license terms. By expressly
* accepting such terms or by downloading, installing, activating and/or otherwise
* using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms. If you do not agree to be
* bound by the applicable license terms, then you may not retain, install,
* activate or otherwise use the software.
==================================================================================================*/


#ifndef FTM_GPT_IP_CFG_H
#define FTM_GPT_IP_CFG_H
/**
*   @file           Ftm_Gpt_Ip_Cfg.h
*
*   @addtogroup     Ftm_Gpt_Ip Ftm_Gpt_Ip IPL
*
*   @{
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
/* Include all variants header files. */
#include "Ftm_Gpt_Ip_PBcfg.h"

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define FTM_GPT_IP_VENDOR_ID_CFG                        43
#define FTM_GPT_IP_AR_RELEASE_MAJOR_VERSION_CFG         4
#define FTM_GPT_IP_AR_RELEASE_MINOR_VERSION_CFG         7
#define FTM_GPT_IP_AR_RELEASE_REVISION_VERSION_CFG      0
#define FTM_GPT_IP_SW_MAJOR_VERSION_CFG                 3
#define FTM_GPT_IP_SW_MINOR_VERSION_CFG                 0
#define FTM_GPT_IP_SW_PATCH_VERSION_CFG                 0

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/
#if (FTM_GPT_IP_VENDOR_ID_CFG != FTM_GPT_IP_VENDOR_ID_PBCFG_H)
    #error "Ftm_Gpt_Ip_Cfg.h and Ftm_Gpt_Ip_PBcfg.h have different vendor ids"
#endif
/* Check if this header file and header file are of the same Autosar version */
#if ((FTM_GPT_IP_AR_RELEASE_MAJOR_VERSION_CFG != FTM_GPT_IP_AR_RELEASE_MAJOR_VERSION_PBCFG_H) || \
     (FTM_GPT_IP_AR_RELEASE_MINOR_VERSION_CFG != FTM_GPT_IP_AR_RELEASE_MINOR_VERSION_PBCFG_H) || \
     (FTM_GPT_IP_AR_RELEASE_REVISION_VERSION_CFG != FTM_GPT_IP_AR_RELEASE_REVISION_VERSION_PBCFG_H) \
    )
    #error "AutoSar Version Numbers of Ftm_Gpt_Ip_Cfg.h and Ftm_Gpt_Ip_PBcfg.h are different"
#endif
/* Check if this header file and header file are of the same Software version */
#if ((FTM_GPT_IP_SW_MAJOR_VERSION_CFG != FTM_GPT_IP_SW_MAJOR_VERSION_PBCFG_H) || \
     (FTM_GPT_IP_SW_MINOR_VERSION_CFG != FTM_GPT_IP_SW_MINOR_VERSION_PBCFG_H) || \
     (FTM_GPT_IP_SW_PATCH_VERSION_CFG != FTM_GPT_IP_SW_PATCH_VERSION_PBCFG_H) \
    )
    #error "Software Version Numbers of Ftm_Gpt_Ip_Cfg.h and Ftm_Gpt_Ip_PBcfg.h are different"
#endif

/*==================================================================================================
*                                          CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/
/**
* @brief   Dev error detect switch
* @details Enable/disable GptDevErrorDetect
*/
#define FTM_GPT_IP_DEV_ERROR_DETECT (STD_OFF)

/*================================================================================================*/
/**
* @brief    FTM_GPT_IP_PREDEFTIMER_FUNCTIONALITY_API switch
* @details  Enable/disable GPT_PREDEFTIMER_FUNCTIONALITY_API.
*/
#define FTM_GPT_IP_PREDEFTIMER_FUNCTIONALITY_API (STD_OFF)

/*================================================================================================*/
/**
* @brief    FTM_GPT_IP_CHANGE_NEXT_TIMEOUT_VALUE switch
* @details  Enable/disable support for changing timeout value during timer running
*/
#define FTM_GPT_IP_CHANGE_NEXT_TIMEOUT_VALUE (STD_OFF)

/*================================================================================================*/
/**
* @brief    FTM_GPT_IP_ABSOLUTE_COUNTING_API switch
* @details  Enable/disable FtmAbsoluteCounting.
*/
#define FTM_GPT_IP_ABSOLUTE_COUNTING_API    (STD_OFF)
/*================================================================================================*/

/**
* @brief    FTM_GPT_IP_ENABLE_USER_MODE_SUPPORT switch
* @details  Enable/disable support usermode.If this parameter has been configured to TRUE the GPT driver code can be executed from both supervisor and user mode.
*/
#define FTM_GPT_IP_ENABLE_USER_MODE_SUPPORT     (STD_OFF)

#ifndef MCAL_ENABLE_USER_MODE_SUPPORT
    #ifdef FTM_GPT_IP_ENABLE_USER_MODE_SUPPORT
        #if (STD_ON == FTM_GPT_IP_ENABLE_USER_MODE_SUPPORT)
            #error MCAL_ENABLE_USER_MODE_SUPPORT is not enabled. For running Gpt in user mode the MCAL_ENABLE_USER_MODE_SUPPORT needs to be defined
        #endif /* (STD_ON == FTM_GPT_IP_ENABLE_USER_MODE_SUPPORT) */
    #endif /* ifdef FTM_GPT_IP_ENABLE_USER_MODE_SUPPORT */
#endif /* ifndef MCAL_ENABLE_USER_MODE_SUPPORT */

/*================================================================================================*/

/** Channels number instance FTM_0 */
#define FTM_0_IP_CHANNELS_NUMBER  (8U)
#define FTM_0_IP_EXISTS
#define FTM_0_IP_INSTANCE_NUMBER  (0U)
/** Channels number instance FTM_1 */
#define FTM_1_IP_CHANNELS_NUMBER  (8U)
#define FTM_1_IP_EXISTS
#define FTM_1_IP_INSTANCE_NUMBER  (1U)
/** Channels number instance FTM_2 */
#define FTM_2_IP_CHANNELS_NUMBER  (8U)
#define FTM_2_IP_EXISTS
#define FTM_2_IP_INSTANCE_NUMBER  (2U)
/** Channels number instance FTM_3 */
#define FTM_3_IP_CHANNELS_NUMBER  (8U)
#define FTM_3_IP_EXISTS
#define FTM_3_IP_INSTANCE_NUMBER  (3U)

/*==================================================================================================
*                                             ENUMS
==================================================================================================*/

/*==================================================================================================
*                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
*                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
*                                    FUNCTION PROTOTYPES
==================================================================================================*/

#ifdef __cplusplus
}
#endif
/** @} */

#endif  /* FTM_GPT_IP_CFG_H */

