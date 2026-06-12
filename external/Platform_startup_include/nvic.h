/*==================================================================================================
*   Project              : RTD AUTOSAR 4.7
*   Platform             : CORTEXM
*   Peripheral           : 
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
*   used strictly in accordance with the applicable license terms.  By expressly 
*   accepting such terms or by downloading, installing, activating and/or otherwise 
*   using the software, you are agreeing that you have read, and that you agree to 
*   comply with and are bound by, such license terms.  If you do not agree to be 
*   bound by the applicable license terms, then you may not retain, install,
*   activate or otherwise use the software.
*/
/*==================================================================================================
*   @file    nvic.h
*   @version 3.0.0
*
*   @brief   AUTOSAR Platform - nvic driver
*   @details nvic driver
*            This file contains sample code only. It is not part of the production code deliverables
==================================================================================================*/

#ifndef NVIC_H
#define NVIC_H

#ifdef __cplusplus
extern "C"
{
#endif

/*==================================================================================================
*                                         INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "Std_Types.h"
#include "Mcal.h"

#ifdef S32K116
    #include "S32K116.h"
#endif
#ifdef S32K116V
    #include "S32K116.h"
#endif
#ifdef S32K118
    #include "S32K118.h"
#endif
#ifdef S32K118V
    #include "S32K118.h"
#endif
#ifdef S32K142
    #include "S32K142.h"
#endif
#ifdef S32K142N
    #include "S32K142.h"
#endif
#ifdef S32K142W
    #include "S32K142W.h"
#endif
#ifdef S32K144
    #include "S32K144.h"
#endif
#ifdef S32K144N
    #include "S32K144.h"
#endif
#ifdef S32K144W
    #include "S32K144W.h"
#endif
#ifdef S32K146
    #include "S32K146.h"
#endif
#ifdef S32K148
    #include "S32K148.h"
#endif
#if (defined(S32M244) || defined(S32M242)|| defined(S32M243)|| defined(S32M241))
    #include "S32M24x.h"
#endif
/*==================================================================================================
*                               SOURCE FILE VERSION INFORMATION
==================================================================================================*/

/*==================================================================================================
*                                           CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                       DEFINES AND MACROS
==================================================================================================*/
#if defined(S32K116) || defined(S32K118)
    #define FEATURE_NVIC_PRIO_BITS (2U)
    #define FEATURE_NVIC_CORTEX_M0P
    #define FEATURE_NVIC_REGISTER_INDEX(x) (0U)
#else
    #define FEATURE_NVIC_PRIO_BITS (4U)
    #define FEATURE_NVIC_CORTEX_M4
    #define FEATURE_NVIC_REGISTER_INDEX(x) ((uint32)(x) >> 5U)
    #define FEATURE_NVIC_PRIORITY_GROUPING
#endif

/*==================================================================================================
*                                             ENUMS
==================================================================================================*/


/*==================================================================================================
*                                     FUNCTION PROTOTYPES
==================================================================================================*/

void NVIC_SetPriorityGrouping(uint32 PriorityGroup);
void NVIC_EnableIRQ(uint8 IRQn);
void NVIC_DisableIRQ(uint8 IRQn);
void NVIC_SetPriority(uint8 IRQn, uint8 priority);


#ifdef __cplusplus
}
#endif

#endif /*NVIC_H*/
