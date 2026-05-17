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

#ifndef FTM_GPT_IP_IRQ_H
#define FTM_GPT_IP_IRQ_H

/**
*   @file       Ftm_Gpt_Ip_Irq.h
*
*   @addtogroup ftm_ip Ftm IPL
*   @{
*/

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
*                                          INCLUDE FILES
*  1) system and project includes
*  2) needed interfaces from external units
*  3) internal and external interfaces from this unit
==================================================================================================*/
#include "OsIf.h"
#include "Ftm_Gpt_Ip_Cfg.h"
#include "Ftm_Gpt_Ip_Types.h"

/*==================================================================================================
*                                SOURCE FILE VERSION INFORMATION
==================================================================================================*/

#define FTM_GPT_IP_IRQ_VENDOR_ID                       43
#define FTM_GPT_IP_IRQ_AR_RELEASE_MAJOR_VERSION        4
#define FTM_GPT_IP_IRQ_AR_RELEASE_MINOR_VERSION        7
#define FTM_GPT_IP_IRQ_AR_RELEASE_REVISION_VERSION     0
#define FTM_GPT_IP_IRQ_SW_MAJOR_VERSION                3
#define FTM_GPT_IP_IRQ_SW_MINOR_VERSION                0
#define FTM_GPT_IP_IRQ_SW_PATCH_VERSION                0

/*==================================================================================================
*                                       FILE VERSION CHECKS
==================================================================================================*/

/* Check if header file and OsIf.h are of the same AUTOSAR version */
#ifndef DISABLE_MCAL_INTERMODULE_ASR_CHECK
    #if ((FTM_GPT_IP_IRQ_AR_RELEASE_MAJOR_VERSION != OSIF_AR_RELEASE_MAJOR_VERSION) || \
         (FTM_GPT_IP_IRQ_AR_RELEASE_MINOR_VERSION != OSIF_AR_RELEASE_MINOR_VERSION))
        #error "AUTOSAR version numbers of Ftm_Gpt_Ip_Irq.h and OsIf.h are different."
    #endif
#endif

/* Check if header file and Ftm_Gpt_Ip_Cfg.h header file are of the same vendor */
#if (FTM_GPT_IP_IRQ_VENDOR_ID != FTM_GPT_IP_VENDOR_ID_CFG)
    #error "Vendor IDs of Ftm_Gpt_Ip_Irq.h and Ftm_Gpt_Ip_Cfg.h are different."
#endif

/* Check if header file and Ftm_Gpt_Ip_Cfg.h header file are of the same AUTOSAR version */
#if ((FTM_GPT_IP_IRQ_AR_RELEASE_MAJOR_VERSION    != FTM_GPT_IP_AR_RELEASE_MAJOR_VERSION_CFG) || \
     (FTM_GPT_IP_IRQ_AR_RELEASE_MINOR_VERSION    != FTM_GPT_IP_AR_RELEASE_MINOR_VERSION_CFG) || \
     (FTM_GPT_IP_IRQ_AR_RELEASE_REVISION_VERSION != FTM_GPT_IP_AR_RELEASE_REVISION_VERSION_CFG))
    #error "AUTOSAR version numbers of Ftm_Gpt_Ip_Irq.h and Ftm_Gpt_Ip_Cfg.h are different."
#endif

/* Check if header file and Ftm_Gpt_Ip_Cfg.h header file are of the same software version */
#if ((FTM_GPT_IP_IRQ_SW_MAJOR_VERSION != FTM_GPT_IP_SW_MAJOR_VERSION_CFG) || \
     (FTM_GPT_IP_IRQ_SW_MINOR_VERSION != FTM_GPT_IP_SW_MINOR_VERSION_CFG) || \
     (FTM_GPT_IP_IRQ_SW_PATCH_VERSION != FTM_GPT_IP_SW_PATCH_VERSION_CFG))
    #error "Software version numbers of Ftm_Gpt_Ip_Irq.h and Ftm_Gpt_Ip_Cfg.h are different."
#endif

/* Check if header file and Ftm_Gpt_Ip_Types.h header file are of the same vendor */
#if (FTM_GPT_IP_IRQ_VENDOR_ID != FTM_GPT_IP_TYPES_VENDOR_ID)
    #error "Vendor IDs of Ftm_Gpt_Ip_Irq.h and Ftm_Gpt_Ip_Types.h are different."
#endif

/* Check if header file and Ftm_Gpt_Ip_Types.h header file are of the same AUTOSAR version */
#if ((FTM_GPT_IP_IRQ_AR_RELEASE_MAJOR_VERSION    != FTM_GPT_IP_TYPES_AR_RELEASE_MAJOR_VERSION) || \
     (FTM_GPT_IP_IRQ_AR_RELEASE_MINOR_VERSION    != FTM_GPT_IP_TYPES_AR_RELEASE_MINOR_VERSION) || \
     (FTM_GPT_IP_IRQ_AR_RELEASE_REVISION_VERSION != FTM_GPT_IP_TYPES_AR_RELEASE_REVISION_VERSION))
    #error "AUTOSAR version numbers of Ftm_Gpt_Ip_Irq.h and Ftm_Gpt_Ip_Types.h are different."
#endif

/* Check if header file and Ftm_Gpt_Ip_Types.h header file are of the same software version */
#if ((FTM_GPT_IP_IRQ_SW_MAJOR_VERSION != FTM_GPT_IP_TYPES_SW_MAJOR_VERSION) || \
     (FTM_GPT_IP_IRQ_SW_MINOR_VERSION != FTM_GPT_IP_TYPES_SW_MINOR_VERSION) || \
     (FTM_GPT_IP_IRQ_SW_PATCH_VERSION != FTM_GPT_IP_TYPES_SW_PATCH_VERSION))
    #error "Software version numbers of Ftm_Gpt_Ip_Irq.h and Ftm_Gpt_Ip_Types.h are different."
#endif
/*==================================================================================================
*                                            CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                        DEFINES AND MACROS
==================================================================================================*/

/*==================================================================================================
*                                              ENUMS
==================================================================================================*/

/*==================================================================================================
*                                  STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
*                                  GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
*                                      FUNCTION PROTOTYPES
==================================================================================================*/
#if (FTM_GPT_IP_USED == STD_ON)

/**
* @internal
* @brief MemMap section
*/
#define GPT_START_SEC_CODE
#include "Gpt_MemMap.h"

#if (STD_ON == FTM_GPT_IP_MODULE_SINGLE_AND_MULTIPLE_INTERRUPTS)

#if defined(FSS_HKI_FTM_0_ISR_USED)
ISR(FSS_HKI_FTM_0_ISR);
#endif

#if defined(FSS_HKI_FTM_1_ISR_USED)
ISR(FSS_HKI_FTM_1_ISR);
#endif

#if defined(FSS_COSS_FTM_0_CH_0_ISR_USED)
ISR(FSS_COSS_FTM_0_CH_0_ISR);
#endif

#if defined(FSS_COSS_FTM_0_CH_1_ISR_USED)
ISR(FSS_COSS_FTM_0_CH_1_ISR);
#endif

#if defined(FSS_COSS_FTM_0_CH_2_ISR_USED)
ISR(FSS_COSS_FTM_0_CH_2_ISR);
#endif

#if defined(FSS_COSS_FTM_0_CH_3_ISR_USED)
ISR(FSS_COSS_FTM_0_CH_3_ISR);
#endif

#if defined(FSS_COSS_FTM_0_CH_4_ISR_USED)
ISR(FSS_COSS_FTM_0_CH_4_ISR);
#endif

#if defined(FSS_COSS_FTM_0_CH_5_ISR_USED)
ISR(FSS_COSS_FTM_0_CH_5_ISR);
#endif

#if defined(FSS_COSS_FTM_1_CH_0_ISR_USED)
ISR(FSS_COSS_FTM_1_CH_0_ISR);
#endif

#if defined(FSS_COSS_FTM_1_CH_1_ISR_USED)
ISR(FSS_COSS_FTM_1_CH_1_ISR);
#endif

#if defined(FSS_COSS_FTM_1_CH_2_ISR_USED)
ISR(FSS_COSS_FTM_1_CH_2_ISR);
#endif

#if defined(FSS_COSS_FTM_1_CH_3_ISR_USED)
ISR(FSS_COSS_FTM_1_CH_3_ISR);
#endif

#if defined(FSS_COSS_FTM_1_CH_4_ISR_USED)
ISR(FSS_COSS_FTM_1_CH_4_ISR);
#endif

#if defined(FSS_COSS_FTM_1_CH_5_ISR_USED)
ISR(FSS_COSS_FTM_1_CH_5_ISR);
#endif

#if defined(FSS_COSS_FTM_2_CH_0_ISR_USED)
ISR(FSS_COSS_FTM_2_CH_0_ISR);
#endif

#if defined(FSS_COSS_FTM_2_CH_1_ISR_USED)
ISR(FSS_COSS_FTM_2_CH_1_ISR);
#endif

#if defined(FSS_COSS_FTM_2_CH_2_ISR_USED)
ISR(FSS_COSS_FTM_2_CH_2_ISR);
#endif

#if defined(FSS_COSS_FTM_2_CH_3_ISR_USED)
ISR(FSS_COSS_FTM_2_CH_3_ISR);
#endif

#if defined(FSS_COSS_FTM_2_CH_4_ISR_USED)
ISR(FSS_COSS_FTM_2_CH_4_ISR);
#endif

#if defined(FSS_COSS_FTM_2_CH_5_ISR_USED)
ISR(FSS_COSS_FTM_2_CH_5_ISR);
#endif

#if defined(FSS_COSS_FTM_3_CH_0_ISR_USED)
ISR(FSS_COSS_FTM_3_CH_0_ISR);
#endif

#if defined(FSS_COSS_FTM_3_CH_1_ISR_USED)
ISR(FSS_COSS_FTM_3_CH_1_ISR);
#endif

#if defined(FSS_COSS_FTM_3_CH_2_ISR_USED)
ISR(FSS_COSS_FTM_3_CH_2_ISR);
#endif

#if defined(FSS_COSS_FTM_3_CH_3_ISR_USED)
ISR(FSS_COSS_FTM_3_CH_3_ISR);
#endif

#if defined(FSS_COSS_FTM_3_CH_4_ISR_USED)
ISR(FSS_COSS_FTM_3_CH_4_ISR);
#endif

#if defined(FSS_COSS_FTM_3_CH_5_ISR_USED)
ISR(FSS_COSS_FTM_3_CH_5_ISR);
#endif

#else /* STD_OFF == FTM_GPT_IP_MODULE_SINGLE_AND_MULTIPLE_INTERRUPTS */

#if(STD_ON == FTM_GPT_IP_MODULE_SINGLE_INTERRUPT)

#ifdef FTM_0_ISR_USED
ISR(FTM_0_ISR);
#endif
#ifdef FTM_1_ISR_USED
ISR(FTM_1_ISR);
#endif

#else

#ifdef FTM_0_CH_0_CH_1_ISR_USED
ISR(FTM_0_CH_0_CH_1_ISR);
#endif
#ifdef FTM_0_CH_2_CH_3_ISR_USED
ISR(FTM_0_CH_2_CH_3_ISR);
#endif
#ifdef FTM_0_CH_4_CH_5_ISR_USED
ISR(FTM_0_CH_4_CH_5_ISR);
#endif
#ifdef FTM_0_CH_6_CH_7_ISR_USED
ISR(FTM_0_CH_6_CH_7_ISR);
#endif
#ifdef FTM_1_CH_0_CH_1_ISR_USED
ISR(FTM_1_CH_0_CH_1_ISR);
#endif
#ifdef FTM_1_CH_2_CH_3_ISR_USED
ISR(FTM_1_CH_2_CH_3_ISR);
#endif
#ifdef FTM_1_CH_4_CH_5_ISR_USED
ISR(FTM_1_CH_4_CH_5_ISR);
#endif
#ifdef FTM_1_CH_6_CH_7_ISR_USED
ISR(FTM_1_CH_6_CH_7_ISR);
#endif
#ifdef FTM_2_CH_0_CH_1_ISR_USED
ISR(FTM_2_CH_0_CH_1_ISR);
#endif
#ifdef FTM_2_CH_2_CH_3_ISR_USED
ISR(FTM_2_CH_2_CH_3_ISR);
#endif
#ifdef FTM_2_CH_4_CH_5_ISR_USED
ISR(FTM_2_CH_4_CH_5_ISR);
#endif
#ifdef FTM_2_CH_6_CH_7_ISR_USED
ISR(FTM_2_CH_6_CH_7_ISR);
#endif
#ifdef FTM_3_CH_0_CH_1_ISR_USED
ISR(FTM_3_CH_0_CH_1_ISR);
#endif
#ifdef FTM_3_CH_2_CH_3_ISR_USED
ISR(FTM_3_CH_2_CH_3_ISR);
#endif
#ifdef FTM_3_CH_4_CH_5_ISR_USED
ISR(FTM_3_CH_4_CH_5_ISR);
#endif
#ifdef FTM_3_CH_6_CH_7_ISR_USED
ISR(FTM_3_CH_6_CH_7_ISR);
#endif
#ifdef FTM_4_CH_0_CH_1_ISR_USED
ISR(FTM_4_CH_0_CH_1_ISR);
#endif
#ifdef FTM_4_CH_2_CH_3_ISR_USED
ISR(FTM_4_CH_2_CH_3_ISR);
#endif
#ifdef FTM_4_CH_4_CH_5_ISR_USED
ISR(FTM_4_CH_4_CH_5_ISR);
#endif
#ifdef FTM_4_CH_6_CH_7_ISR_USED
ISR(FTM_4_CH_6_CH_7_ISR);
#endif
#ifdef FTM_5_CH_0_CH_1_ISR_USED
ISR(FTM_5_CH_0_CH_1_ISR);
#endif
#ifdef FTM_5_CH_2_CH_3_ISR_USED
ISR(FTM_5_CH_2_CH_3_ISR);
#endif
#ifdef FTM_5_CH_4_CH_5_ISR_USED
ISR(FTM_5_CH_4_CH_5_ISR);
#endif
#ifdef FTM_5_CH_6_CH_7_ISR_USED
ISR(FTM_5_CH_6_CH_7_ISR);
#endif
#ifdef FTM_6_CH_0_CH_1_ISR_USED
ISR(FTM_6_CH_0_CH_1_ISR);
#endif
#ifdef FTM_6_CH_2_CH_3_ISR_USED
ISR(FTM_6_CH_2_CH_3_ISR);
#endif
#ifdef FTM_6_CH_4_CH_5_ISR_USED
ISR(FTM_6_CH_4_CH_5_ISR);
#endif
#ifdef FTM_6_CH_6_CH_7_ISR_USED
ISR(FTM_6_CH_6_CH_7_ISR);
#endif
#ifdef FTM_7_CH_0_CH_1_ISR_USED
ISR(FTM_7_CH_0_CH_1_ISR);
#endif
#ifdef FTM_7_CH_2_CH_3_ISR_USED
ISR(FTM_7_CH_2_CH_3_ISR);
#endif
#ifdef FTM_7_CH_4_CH_5_ISR_USED
ISR(FTM_7_CH_4_CH_5_ISR);
#endif
#ifdef FTM_7_CH_6_CH_7_ISR_USED
ISR(FTM_7_CH_6_CH_7_ISR);
#endif

#endif /* FTM_GPT_IP_MODULE_SINGLE_INTERRUPT */

#endif /* STD_ON == FTM_GPT_IP_MODULE_SINGLE_AND_MULTIPLE_INTERRUPTS */

/**
* @internal
* @brief MemMap section
*/
#define GPT_STOP_SEC_CODE
#include "Gpt_MemMap.h"
#endif /* FTM_GPT_IP_USED == STD_ON */
#ifdef __cplusplus
}
#endif
/** @} */

#endif  /* FTM_GPT_IP_IRQ_H */
