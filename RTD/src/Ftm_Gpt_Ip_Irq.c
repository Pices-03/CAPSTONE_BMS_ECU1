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

/**
*     @file
*
*     @addtogroup ftm_ip IPL Driver
*     @{
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
#include "Ftm_Gpt_Ip_Irq.h"
#include "Ftm_Gpt_Ip.h"
#include "Ftm_Gpt_Ip_HwAccess.h"
#include "Mcal.h"
/*==================================================================================================
*                                SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define FTM_GPT_IP_IRQ_VENDOR_ID_C                      43
#define FTM_GPT_IP_IRQ_AR_RELEASE_MAJOR_VERSION_C       4
#define FTM_GPT_IP_IRQ_AR_RELEASE_MINOR_VERSION_C       7
#define FTM_GPT_IP_IRQ_AR_RELEASE_REVISION_VERSION_C    0
#define FTM_GPT_IP_IRQ_SW_MAJOR_VERSION_C               3
#define FTM_GPT_IP_IRQ_SW_MINOR_VERSION_C               0
#define FTM_GPT_IP_IRQ_SW_PATCH_VERSION_C               0

/*==================================================================================================
*                                       FILE VERSION CHECKS
==================================================================================================*/
/* Check if source file and Ftm_Gpt_Ip_Irq.h header file are of the same vendor */
#if (FTM_GPT_IP_IRQ_VENDOR_ID_C != FTM_GPT_IP_IRQ_VENDOR_ID)
    #error "Vendor IDs of Ftm_Gpt_Ip_Irq.c and Ftm_Gpt_Ip_Irq.h are different."
#endif

/* Check if source file and Ftm_Gpt_Ip_Irq.h header file are of the same AUTOSAR version */
#if ((FTM_GPT_IP_IRQ_AR_RELEASE_MAJOR_VERSION_C    != FTM_GPT_IP_IRQ_AR_RELEASE_MAJOR_VERSION) || \
     (FTM_GPT_IP_IRQ_AR_RELEASE_MINOR_VERSION_C    != FTM_GPT_IP_IRQ_AR_RELEASE_MINOR_VERSION) || \
     (FTM_GPT_IP_IRQ_AR_RELEASE_REVISION_VERSION_C != FTM_GPT_IP_IRQ_AR_RELEASE_REVISION_VERSION))
    #error "AUTOSAR version numbers of Ftm_Gpt_Ip_Irq.c and Ftm_Gpt_Ip_Irq.h are different."
#endif

/* Check if header file and Ftm_Gpt_Ip_Irq.h header file are of the same software version */
#if ((FTM_GPT_IP_IRQ_SW_MAJOR_VERSION_C != FTM_GPT_IP_IRQ_SW_MAJOR_VERSION) || \
     (FTM_GPT_IP_IRQ_SW_MINOR_VERSION_C != FTM_GPT_IP_IRQ_SW_MINOR_VERSION) || \
     (FTM_GPT_IP_IRQ_SW_PATCH_VERSION_C != FTM_GPT_IP_IRQ_SW_PATCH_VERSION))
    #error "Software version numbers of Ftm_Gpt_Ip_Irq.c and Ftm_Gpt_Ip_Irq.h are different."
#endif


/* Check if source file and Ftm_Gpt_Ip_HwAccess.h header file are of the same vendor */
#if (FTM_GPT_IP_IRQ_VENDOR_ID_C != FTM_GPT_IP_HWACCESS_VENDOR_ID)
    #error "Vendor IDs of Ftm_Gpt_Ip_Irq.c and Ftm_Gpt_Ip_HwAccess.h are different."
#endif

/* Check if source file and Ftm_Gpt_Ip_HwAccess.h header file are of the same AUTOSAR version */
#if ((FTM_GPT_IP_IRQ_AR_RELEASE_MAJOR_VERSION_C    != FTM_GPT_IP_HWACCESS_AR_RELEASE_MAJOR_VERSION) || \
     (FTM_GPT_IP_IRQ_AR_RELEASE_MINOR_VERSION_C    != FTM_GPT_IP_HWACCESS_AR_RELEASE_MINOR_VERSION) || \
     (FTM_GPT_IP_IRQ_AR_RELEASE_REVISION_VERSION_C != FTM_GPT_IP_HWACCESS_AR_RELEASE_REVISION_VERSION))
    #error "AUTOSAR version numbers of Ftm_Gpt_Ip_Irq.c and Ftm_Gpt_Ip_HwAccess.h are different."
#endif

/* Check if header file and Ftm_Gpt_Ip_HwAccess.h header file are of the same software version */
#if ((FTM_GPT_IP_IRQ_SW_MAJOR_VERSION_C != FTM_GPT_IP_HWACCESS_SW_MAJOR_VERSION) || \
     (FTM_GPT_IP_IRQ_SW_MINOR_VERSION_C != FTM_GPT_IP_HWACCESS_SW_MINOR_VERSION) || \
     (FTM_GPT_IP_IRQ_SW_PATCH_VERSION_C != FTM_GPT_IP_HWACCESS_SW_PATCH_VERSION))
    #error "Software version numbers of Ftm_Gpt_Ip_Irq.c and Ftm_Gpt_Ip_HwAccess.h are different."
#endif

/* Check if source file and Ftm_Gpt_Ip.h header file are of the same vendor */
#if (FTM_GPT_IP_IRQ_VENDOR_ID_C != FTM_GPT_IP_VENDOR_ID)
    #error "Vendor IDs of Ftm_Gpt_Ip_Irq.c and Ftm_Gpt_Ip.h are different."
#endif

/* Check if source file and Ftm_Gpt_Ip.h header file are of the same AUTOSAR version */
#if ((FTM_GPT_IP_IRQ_AR_RELEASE_MAJOR_VERSION_C    != FTM_GPT_IP_AR_RELEASE_MAJOR_VERSION) || \
     (FTM_GPT_IP_IRQ_AR_RELEASE_MINOR_VERSION_C    != FTM_GPT_IP_AR_RELEASE_MINOR_VERSION) || \
     (FTM_GPT_IP_IRQ_AR_RELEASE_REVISION_VERSION_C != FTM_GPT_IP_AR_RELEASE_REVISION_VERSION))
    #error "AUTOSAR version numbers of Ftm_Gpt_Ip_Irq.c and Ftm_Gpt_Ip.h are different."
#endif

/* Check if header file and Ftm_Gpt_Ip.h header file are of the same software version */
#if ((FTM_GPT_IP_IRQ_SW_MAJOR_VERSION_C != FTM_GPT_IP_SW_MAJOR_VERSION) || \
     (FTM_GPT_IP_IRQ_SW_MINOR_VERSION_C != FTM_GPT_IP_SW_MINOR_VERSION) || \
     (FTM_GPT_IP_IRQ_SW_PATCH_VERSION_C != FTM_GPT_IP_SW_PATCH_VERSION))
    #error "Software version numbers of Ftm_Gpt_Ip_Irq.c and Ftm_Gpt_Ip.h are different."
#endif

#ifndef DISABLE_MCAL_INTERMODULE_ASR_CHECK
    /* Check if source file and Mcal.h are of the same AUTOSAR version */
    #if ((FTM_GPT_IP_IRQ_AR_RELEASE_MAJOR_VERSION_C != MCAL_AR_RELEASE_MAJOR_VERSION) || \
         (FTM_GPT_IP_IRQ_AR_RELEASE_MINOR_VERSION_C != MCAL_AR_RELEASE_MINOR_VERSION))
        #error "AUTOSAR version numbers of Ftm_Gpt_Ip_Irq.c and Mcal.h are different."
    #endif
#endif
/*==================================================================================================
*                                        GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                        GLOBAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
*                                        GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
#define GPT_START_SEC_CODE
#include "Gpt_MemMap.h"

#if (FTM_GPT_IP_USED == STD_ON)
#if (   defined(FTM_0_ISR_USED) || defined(FTM_1_ISR_USED) || \
        defined(FTM_0_CH_0_CH_1_ISR_USED) || defined(FTM_0_CH_2_CH_3_ISR_USED) || defined(FTM_0_CH_4_CH_5_ISR_USED) || \
        defined(FTM_0_CH_6_CH_7_ISR_USED) || defined(FTM_1_CH_0_CH_1_ISR_USED) || defined(FTM_1_CH_2_CH_3_ISR_USED) || \
        defined(FTM_1_CH_4_CH_5_ISR_USED) || defined(FTM_1_CH_6_CH_7_ISR_USED) || defined(FTM_2_CH_0_CH_1_ISR_USED) || \
        defined(FTM_2_CH_2_CH_3_ISR_USED) || defined(FTM_2_CH_4_CH_5_ISR_USED) || defined(FTM_2_CH_6_CH_7_ISR_USED) || \
        defined(FTM_3_CH_0_CH_1_ISR_USED) || defined(FTM_3_CH_2_CH_3_ISR_USED) || defined(FTM_3_CH_4_CH_5_ISR_USED) || \
        defined(FTM_3_CH_6_CH_7_ISR_USED) || defined(FTM_4_CH_0_CH_1_ISR_USED) || defined(FTM_4_CH_2_CH_3_ISR_USED) || \
        defined(FTM_4_CH_4_CH_5_ISR_USED) || defined(FTM_4_CH_6_CH_7_ISR_USED) || defined(FTM_5_CH_0_CH_1_ISR_USED) || \
        defined(FTM_5_CH_2_CH_3_ISR_USED) || defined(FTM_5_CH_4_CH_5_ISR_USED) || defined(FTM_5_CH_6_CH_7_ISR_USED) || \
        defined(FTM_6_CH_0_CH_1_ISR_USED) || defined(FTM_6_CH_2_CH_3_ISR_USED) || defined(FTM_6_CH_4_CH_5_ISR_USED) || \
        defined(FTM_6_CH_6_CH_7_ISR_USED) || defined(FTM_7_CH_0_CH_1_ISR_USED) || defined(FTM_7_CH_2_CH_3_ISR_USED) || \
        defined(FTM_7_CH_4_CH_5_ISR_USED) || defined(FTM_7_CH_6_CH_7_ISR_USED) || \
        defined(FSS_HKI_FTM_0_ISR_USED) || defined(FSS_HKI_FTM_1_ISR_USED) || \
        defined(FSS_COSS_FTM_0_CH_0_ISR_USED) || defined(FSS_COSS_FTM_0_CH_1_ISR_USED) || defined(FSS_COSS_FTM_0_CH_2_ISR_USED) || \
        defined(FSS_COSS_FTM_0_CH_3_ISR_USED) || defined(FSS_COSS_FTM_0_CH_4_ISR_USED) || defined(FSS_COSS_FTM_0_CH_5_ISR_USED) || \
        defined(FSS_COSS_FTM_1_CH_0_ISR_USED) || defined(FSS_COSS_FTM_1_CH_1_ISR_USED) || defined(FSS_COSS_FTM_1_CH_2_ISR_USED) || \
        defined(FSS_COSS_FTM_1_CH_3_ISR_USED) || defined(FSS_COSS_FTM_1_CH_4_ISR_USED) || defined(FSS_COSS_FTM_1_CH_5_ISR_USED) || \
        defined(FSS_COSS_FTM_2_CH_0_ISR_USED) || defined(FSS_COSS_FTM_2_CH_1_ISR_USED) || defined(FSS_COSS_FTM_2_CH_2_ISR_USED) || \
        defined(FSS_COSS_FTM_2_CH_3_ISR_USED) || defined(FSS_COSS_FTM_2_CH_4_ISR_USED) || defined(FSS_COSS_FTM_2_CH_5_ISR_USED) || \
        defined(FSS_COSS_FTM_3_CH_0_ISR_USED) || defined(FSS_COSS_FTM_3_CH_1_ISR_USED) || defined(FSS_COSS_FTM_3_CH_2_ISR_USED) || \
        defined(FSS_COSS_FTM_3_CH_3_ISR_USED) || defined(FSS_COSS_FTM_3_CH_4_ISR_USED) || defined(FSS_COSS_FTM_3_CH_5_ISR_USED) )
static void Ftm_Gpt_Ip_ProcessCommonInterrupt(uint8 instance, uint8 channel);
#endif

/*==================================================================================================
*                                        LOCAL FUNCTIONS
==================================================================================================*/

#if (STD_ON == FTM_GPT_IP_MODULE_SINGLE_AND_MULTIPLE_INTERRUPTS)

#if defined(FSS_HKI_FTM_0_ISR_USED)
/**
* @brief    Interrupt handler for FSS_HKI_FTM channels.
* @details  Interrupt Service Routine corresponding to common FSS_HKI_FTM_0 module.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FSS_HKI_FTM_0_ISR){
    uint8 channel;
    boolean chEvStatus;
#if defined(FSS_HKI_FTM_0_IP_EXISTS)
    uint8 instance = FSS_HKI_FTM_0_IP_INSTANCE_NUMBER;
#else
    #error "undefined FSS_HKI_FTM instance number"
#endif
    for (channel = 0U; channel < FSS_HKI_FTM_0_IP_CHANNELS_NUMBER; ++channel)
    {
        chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
        if (chEvStatus)
        {
            Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
        }
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FSS_HKI_FTM_1_ISR_USED)
/**
* @brief    Interrupt handler for FSS_HKI_FTM channels.
* @details  Interrupt Service Routine corresponding to common FSS_HKI_FTM_1 module.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FSS_HKI_FTM_1_ISR){
    uint8 channel;
    boolean chEvStatus;
#if defined(FSS_HKI_FTM_1_IP_EXISTS)
    uint8 instance = FSS_HKI_FTM_1_IP_INSTANCE_NUMBER;
#else
    #error "undefined FSS_HKI_FTM instance number"
#endif
    for (channel = 0U; channel < FSS_HKI_FTM_1_IP_CHANNELS_NUMBER; ++channel)
    {
        chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
        if (chEvStatus)
        {
            Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
        }
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FSS_COSS_FTM_0_CH_0_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 0, channel 0
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FSS_COSS_FTM_0_CH_0_ISR){
#if defined(FSS_COSS_FTM_0_IP_EXISTS)
    uint8 instance = FSS_COSS_FTM_0_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 0U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }
    EXIT_INTERRUPT();
}
#endif

#if defined(FSS_COSS_FTM_0_CH_1_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 0, channel 1
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FSS_COSS_FTM_0_CH_1_ISR){
#if defined(FSS_COSS_FTM_0_IP_EXISTS)
    uint8 instance = FSS_COSS_FTM_0_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 1U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }
    EXIT_INTERRUPT();
}
#endif

#if defined(FSS_COSS_FTM_0_CH_2_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 0, channel 2
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FSS_COSS_FTM_0_CH_2_ISR){
#if defined(FSS_COSS_FTM_0_IP_EXISTS)
    uint8 instance = FSS_COSS_FTM_0_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 2U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }
    EXIT_INTERRUPT();
}
#endif

#if defined(FSS_COSS_FTM_0_CH_3_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 0, channel 3
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FSS_COSS_FTM_0_CH_3_ISR){
#if defined(FSS_COSS_FTM_0_IP_EXISTS)
    uint8 instance = FSS_COSS_FTM_0_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 3U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }
    EXIT_INTERRUPT();
}
#endif

#if defined(FSS_COSS_FTM_0_CH_4_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 0, channel 4
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FSS_COSS_FTM_0_CH_4_ISR){
#if defined(FSS_COSS_FTM_0_IP_EXISTS)
    uint8 instance = FSS_COSS_FTM_0_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 4U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }
    EXIT_INTERRUPT();
}
#endif

#if defined(FSS_COSS_FTM_0_CH_5_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 0, channel 5
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FSS_COSS_FTM_0_CH_5_ISR){
#if defined(FSS_COSS_FTM_0_IP_EXISTS)
    uint8 instance = FSS_COSS_FTM_0_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 5U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }
    EXIT_INTERRUPT();
}
#endif

#if defined(FSS_COSS_FTM_1_CH_0_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 1, channel 0
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FSS_COSS_FTM_1_CH_0_ISR){
#if defined(FSS_COSS_FTM_1_IP_EXISTS)
    uint8 instance = FSS_COSS_FTM_1_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 0U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }
    EXIT_INTERRUPT();
}
#endif

#if defined(FSS_COSS_FTM_1_CH_1_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 1, channel 1
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FSS_COSS_FTM_1_CH_1_ISR){
#if defined(FSS_COSS_FTM_1_IP_EXISTS)
    uint8 instance = FSS_COSS_FTM_1_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 1U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }
    EXIT_INTERRUPT();
}
#endif

#if defined(FSS_COSS_FTM_1_CH_2_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 1, channel 2
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FSS_COSS_FTM_1_CH_2_ISR){
#if defined(FSS_COSS_FTM_1_IP_EXISTS)
    uint8 instance = FSS_COSS_FTM_1_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 2U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }
    EXIT_INTERRUPT();
}
#endif

#if defined(FSS_COSS_FTM_1_CH_3_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 1, channel 3
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FSS_COSS_FTM_1_CH_3_ISR){
#if defined(FSS_COSS_FTM_1_IP_EXISTS)
    uint8 instance = FSS_COSS_FTM_1_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 3U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }
    EXIT_INTERRUPT();
}
#endif

#if defined(FSS_COSS_FTM_1_CH_4_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 1, channel 4
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FSS_COSS_FTM_1_CH_4_ISR){
#if defined(FSS_COSS_FTM_1_IP_EXISTS)
    uint8 instance = FSS_COSS_FTM_1_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 4U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }
    EXIT_INTERRUPT();
}
#endif

#if defined(FSS_COSS_FTM_1_CH_5_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 1, channel 5
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FSS_COSS_FTM_1_CH_5_ISR){
#if defined(FSS_COSS_FTM_1_IP_EXISTS)
    uint8 instance = FSS_COSS_FTM_1_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 5U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }
    EXIT_INTERRUPT();
}
#endif

#if defined(FSS_COSS_FTM_2_CH_0_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 2, channel 0
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FSS_COSS_FTM_2_CH_0_ISR){
#if defined(FSS_COSS_FTM_2_IP_EXISTS)
    uint8 instance = FSS_COSS_FTM_2_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 0U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }
    EXIT_INTERRUPT();
}
#endif

#if defined(FSS_COSS_FTM_2_CH_1_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 2, channel 1
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FSS_COSS_FTM_2_CH_1_ISR){
#if defined(FSS_COSS_FTM_2_IP_EXISTS)
    uint8 instance = FSS_COSS_FTM_2_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 1U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }
    EXIT_INTERRUPT();
}
#endif

#if defined(FSS_COSS_FTM_2_CH_2_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 2, channel 2
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FSS_COSS_FTM_2_CH_2_ISR){
#if defined(FSS_COSS_FTM_2_IP_EXISTS)
    uint8 instance = FSS_COSS_FTM_2_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 2U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }
    EXIT_INTERRUPT();
}
#endif

#if defined(FSS_COSS_FTM_2_CH_3_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 2, channel 3
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FSS_COSS_FTM_2_CH_3_ISR){
#if defined(FSS_COSS_FTM_2_IP_EXISTS)
    uint8 instance = FSS_COSS_FTM_2_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 3U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }
    EXIT_INTERRUPT();
}
#endif

#if defined(FSS_COSS_FTM_2_CH_4_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 2, channel 4
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FSS_COSS_FTM_2_CH_4_ISR){
#if defined(FSS_COSS_FTM_2_IP_EXISTS)
    uint8 instance = FSS_COSS_FTM_2_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 4U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }
    EXIT_INTERRUPT();
}
#endif

#if defined(FSS_COSS_FTM_2_CH_5_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 2, channel 5
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FSS_COSS_FTM_2_CH_5_ISR){
#if defined(FSS_COSS_FTM_2_IP_EXISTS)
    uint8 instance = FSS_COSS_FTM_2_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 5U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }
    EXIT_INTERRUPT();
}
#endif

#if defined(FSS_COSS_FTM_3_CH_0_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 3, channel 0
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FSS_COSS_FTM_3_CH_0_ISR){
#if defined(FSS_COSS_FTM_3_IP_EXISTS)
    uint8 instance = FSS_COSS_FTM_3_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 0U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }
    EXIT_INTERRUPT();
}
#endif

#if defined(FSS_COSS_FTM_3_CH_1_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 3, channel 1
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FSS_COSS_FTM_3_CH_1_ISR){
#if defined(FSS_COSS_FTM_3_IP_EXISTS)
    uint8 instance = FSS_COSS_FTM_3_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 1U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }
    EXIT_INTERRUPT();
}
#endif

#if defined(FSS_COSS_FTM_3_CH_2_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 3, channel 2
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FSS_COSS_FTM_3_CH_2_ISR){
#if defined(FSS_COSS_FTM_3_IP_EXISTS)
    uint8 instance = FSS_COSS_FTM_3_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 2U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }
    EXIT_INTERRUPT();
}
#endif

#if defined(FSS_COSS_FTM_3_CH_3_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 3, channel 3
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FSS_COSS_FTM_3_CH_3_ISR){
#if defined(FSS_COSS_FTM_3_IP_EXISTS)
    uint8 instance = FSS_COSS_FTM_3_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 3U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }
    EXIT_INTERRUPT();
}
#endif

#if defined(FSS_COSS_FTM_3_CH_4_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 3, channel 4
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FSS_COSS_FTM_3_CH_4_ISR){
#if defined(FSS_COSS_FTM_3_IP_EXISTS)
    uint8 instance = FSS_COSS_FTM_3_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 4U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }
    EXIT_INTERRUPT();
}
#endif

#if defined(FSS_COSS_FTM_3_CH_5_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 3, channel 5
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FSS_COSS_FTM_3_CH_5_ISR){
#if defined(FSS_COSS_FTM_3_IP_EXISTS)
    uint8 instance = FSS_COSS_FTM_3_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 5U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }
    EXIT_INTERRUPT();
}
#endif

#else /* STD_OFF == FTM_GPT_IP_MODULE_SINGLE_AND_MULTIPLE_INTERRUPTS */

#if (STD_ON == FTM_GPT_IP_MODULE_SINGLE_INTERRUPT)

#if defined(FTM_0_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine corresponding to common FTM_0 module.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_0_ISR){
    uint8 channel;
    boolean chEvStatus;
#if defined(FTM_0_IP_EXISTS)
    uint8 instance = FTM_0_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    for (channel = 0U; channel < FTM_0_IP_CHANNELS_NUMBER; ++channel)
    {
        chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
        if (chEvStatus)
        {
            Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
        }
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_1_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine corresponding to common FTM_1 module.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_1_ISR){
    uint8 channel;
    boolean chEvStatus;
#if defined(FTM_1_IP_EXISTS)
    uint8 instance = FTM_1_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    for (channel = 0U; channel < FTM_1_IP_CHANNELS_NUMBER; ++channel)
    {
        chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
        if (chEvStatus)
        {
            Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
        }
    }

    EXIT_INTERRUPT();
}
#endif

#else /* FTM_GPT_IP_MODULE_SINGLE_INTERRUPT */

#if defined(FTM_0_CH_0_CH_1_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 0, channels 0 and 1.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_0_CH_0_CH_1_ISR){
#if defined(FTM_0_IP_EXISTS)
    uint8 instance = FTM_0_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 0U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_0_CH_2_CH_3_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 0, channels 2 and 3.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_0_CH_2_CH_3_ISR){
#if defined(FTM_0_IP_EXISTS)
    uint8 instance = FTM_0_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 2U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_0_CH_4_CH_5_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 0, channels 4 and 5.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_0_CH_4_CH_5_ISR){
#if defined(FTM_0_IP_EXISTS)
    uint8 instance = FTM_0_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 4U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_0_CH_6_CH_7_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 0, channels 6 and 7.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_0_CH_6_CH_7_ISR){
#if defined(FTM_0_IP_EXISTS)
    uint8 instance = FTM_0_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 6U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_1_CH_0_CH_1_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 1, channels 0 and 1.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_1_CH_0_CH_1_ISR){
#if defined(FTM_1_IP_EXISTS)
    uint8 instance = FTM_1_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 0U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_1_CH_2_CH_3_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 1, channels 2 and 3.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_1_CH_2_CH_3_ISR){
#if defined(FTM_1_IP_EXISTS)
    uint8 instance = FTM_1_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 2U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_1_CH_4_CH_5_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 1, channels 4 and 5.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_1_CH_4_CH_5_ISR){
#if defined(FTM_1_IP_EXISTS)
    uint8 instance = FTM_1_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 4U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_1_CH_6_CH_7_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 1, channels 6 and 7.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_1_CH_6_CH_7_ISR){
#if defined(FTM_1_IP_EXISTS)
    uint8 instance = FTM_1_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 6U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_2_CH_0_CH_1_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 2, channels 0 and 1.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_2_CH_0_CH_1_ISR){
#if defined(FTM_2_IP_EXISTS)
    uint8 instance = FTM_2_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 0U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_2_CH_2_CH_3_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 2, channels 2 and 3.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_2_CH_2_CH_3_ISR){
#if defined(FTM_2_IP_EXISTS)
    uint8 instance = FTM_2_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 2U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_2_CH_4_CH_5_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 2, channels 4 and 5.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_2_CH_4_CH_5_ISR){
#if defined(FTM_2_IP_EXISTS)
    uint8 instance = FTM_2_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 4U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_2_CH_6_CH_7_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 2, channels 6 and 7.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_2_CH_6_CH_7_ISR){
#if defined(FTM_2_IP_EXISTS)
    uint8 instance = FTM_2_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 6U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_3_CH_0_CH_1_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 3, channels 0 and 1.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_3_CH_0_CH_1_ISR){
#if defined(FTM_3_IP_EXISTS)
    uint8 instance = FTM_3_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 0U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_3_CH_2_CH_3_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 3, channels 2 and 3.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_3_CH_2_CH_3_ISR){
#if defined(FTM_3_IP_EXISTS)
    uint8 instance = FTM_3_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 2U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_3_CH_4_CH_5_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 3, channels 4 and 5.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_3_CH_4_CH_5_ISR){
#if defined(FTM_3_IP_EXISTS)
    uint8 instance = FTM_3_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 4U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_3_CH_6_CH_7_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 3, channels 6 and 7.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_3_CH_6_CH_7_ISR){
#if defined(FTM_3_IP_EXISTS)
    uint8 instance = FTM_3_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 6U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_4_CH_0_CH_1_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 4, channels 0 and 1.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_4_CH_0_CH_1_ISR){
#if defined(FTM_4_IP_EXISTS)
    uint8 instance = FTM_4_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 0U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_4_CH_2_CH_3_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 4, channels 2 and 3.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_4_CH_2_CH_3_ISR){
#if defined(FTM_4_IP_EXISTS)
    uint8 instance = FTM_4_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 2U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_4_CH_4_CH_5_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 4, channels 4 and 5.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_4_CH_4_CH_5_ISR){
#if defined(FTM_4_IP_EXISTS)
    uint8 instance = FTM_4_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 4U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_4_CH_6_CH_7_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 4, channels 6 and 7.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_4_CH_6_CH_7_ISR){
#if defined(FTM_4_IP_EXISTS)
    uint8 instance = FTM_4_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 6U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_5_CH_0_CH_1_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 5, channels 0 and 1.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_5_CH_0_CH_1_ISR){
#if defined(FTM_5_IP_EXISTS)
    uint8 instance = FTM_5_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 0U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_5_CH_2_CH_3_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 5, channels 2 and 3.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_5_CH_2_CH_3_ISR){
#if defined(FTM_5_IP_EXISTS)
    uint8 instance = FTM_5_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 2U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_5_CH_4_CH_5_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 5, channels 4 and 5.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_5_CH_4_CH_5_ISR){
#if defined(FTM_5_IP_EXISTS)
    uint8 instance = FTM_5_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 4U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_5_CH_6_CH_7_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 5, channels 6 and 7.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_5_CH_6_CH_7_ISR){
#if defined(FTM_5_IP_EXISTS)
    uint8 instance = FTM_5_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 6U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_6_CH_0_CH_1_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 6, channels 0 and 1.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_6_CH_0_CH_1_ISR){
#if defined(FTM_6_IP_EXISTS)
    uint8 instance = FTM_6_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 0U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_6_CH_2_CH_3_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 6, channels 2 and 3.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_6_CH_2_CH_3_ISR){
#if defined(FTM_6_IP_EXISTS)
    uint8 instance = FTM_6_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 2U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_6_CH_4_CH_5_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 6, channels 4 and 5.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_6_CH_4_CH_5_ISR){
#if defined(FTM_6_IP_EXISTS)
    uint8 instance = FTM_6_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 4U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_6_CH_6_CH_7_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 6, channels 6 and 7.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_6_CH_6_CH_7_ISR){
#if defined(FTM_6_IP_EXISTS)
    uint8 instance = FTM_6_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 6U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_7_CH_0_CH_1_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 7, channels 0 and 1.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_7_CH_0_CH_1_ISR){
#if defined(FTM_7_IP_EXISTS)
    uint8 instance = FTM_7_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 0U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_7_CH_2_CH_3_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 7, channels 2 and 3.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_7_CH_2_CH_3_ISR){
#if defined(FTM_7_IP_EXISTS)
    uint8 instance = FTM_7_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 2U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_7_CH_4_CH_5_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 7, channels 4 and 5.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_7_CH_4_CH_5_ISR){
#if defined(FTM_7_IP_EXISTS)
    uint8 instance = FTM_7_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 4U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#if defined(FTM_7_CH_6_CH_7_ISR_USED)
/**
* @brief    Interrupt handler for FTM channels.
* @details  Interrupt Service Routine for FTM module 7, channels 6 and 7.
* @param[in]    none
* @return       none
* @isr
* @pre  The driver needs to be initialized.
*/
ISR(FTM_7_CH_6_CH_7_ISR){
#if defined(FTM_7_IP_EXISTS)
    uint8 instance = FTM_7_IP_INSTANCE_NUMBER;
#else
    #error "undefined FTM instance number"
#endif
    uint8 channel = 6U;
    boolean chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    channel++;
    chEvStatus = Ftm_Gpt_Ip_GetChannelEventStatus(instance, channel);
    if (chEvStatus)
    {
        Ftm_Gpt_Ip_ProcessCommonInterrupt(instance, channel);
    }

    EXIT_INTERRUPT();
}
#endif

#endif /* STD_ON == FTM_GPT_IP_MODULE_SINGLE_INTERRUPT */

#endif /* STD_ON == FTM_GPT_IP_MODULE_SINGLE_AND_MULTIPLE_INTERRUPTS */
/*================================================================================================*/
#if (   defined(FTM_0_ISR_USED) || defined(FTM_1_ISR_USED) || \
        defined(FTM_0_CH_0_CH_1_ISR_USED) || defined(FTM_0_CH_2_CH_3_ISR_USED) || defined(FTM_0_CH_4_CH_5_ISR_USED) || \
        defined(FTM_0_CH_6_CH_7_ISR_USED) || defined(FTM_1_CH_0_CH_1_ISR_USED) || defined(FTM_1_CH_2_CH_3_ISR_USED) || \
        defined(FTM_1_CH_4_CH_5_ISR_USED) || defined(FTM_1_CH_6_CH_7_ISR_USED) || defined(FTM_2_CH_0_CH_1_ISR_USED) || \
        defined(FTM_2_CH_2_CH_3_ISR_USED) || defined(FTM_2_CH_4_CH_5_ISR_USED) || defined(FTM_2_CH_6_CH_7_ISR_USED) || \
        defined(FTM_3_CH_0_CH_1_ISR_USED) || defined(FTM_3_CH_2_CH_3_ISR_USED) || defined(FTM_3_CH_4_CH_5_ISR_USED) || \
        defined(FTM_3_CH_6_CH_7_ISR_USED) || defined(FTM_4_CH_0_CH_1_ISR_USED) || defined(FTM_4_CH_2_CH_3_ISR_USED) || \
        defined(FTM_4_CH_4_CH_5_ISR_USED) || defined(FTM_4_CH_6_CH_7_ISR_USED) || defined(FTM_5_CH_0_CH_1_ISR_USED) || \
        defined(FTM_5_CH_2_CH_3_ISR_USED) || defined(FTM_5_CH_4_CH_5_ISR_USED) || defined(FTM_5_CH_6_CH_7_ISR_USED) || \
        defined(FTM_6_CH_0_CH_1_ISR_USED) || defined(FTM_6_CH_2_CH_3_ISR_USED) || defined(FTM_6_CH_4_CH_5_ISR_USED) || \
        defined(FTM_6_CH_6_CH_7_ISR_USED) || defined(FTM_7_CH_0_CH_1_ISR_USED) || defined(FTM_7_CH_2_CH_3_ISR_USED) || \
        defined(FTM_7_CH_4_CH_5_ISR_USED) || defined(FTM_7_CH_6_CH_7_ISR_USED) || \
        defined(FSS_HKI_FTM_0_ISR_USED) || defined(FSS_HKI_FTM_1_ISR_USED) || \
        defined(FSS_COSS_FTM_0_CH_0_ISR_USED) || defined(FSS_COSS_FTM_0_CH_1_ISR_USED) || defined(FSS_COSS_FTM_0_CH_2_ISR_USED) || \
        defined(FSS_COSS_FTM_0_CH_3_ISR_USED) || defined(FSS_COSS_FTM_0_CH_4_ISR_USED) || defined(FSS_COSS_FTM_0_CH_5_ISR_USED) || \
        defined(FSS_COSS_FTM_1_CH_0_ISR_USED) || defined(FSS_COSS_FTM_1_CH_1_ISR_USED) || defined(FSS_COSS_FTM_1_CH_2_ISR_USED) || \
        defined(FSS_COSS_FTM_1_CH_3_ISR_USED) || defined(FSS_COSS_FTM_1_CH_4_ISR_USED) || defined(FSS_COSS_FTM_1_CH_5_ISR_USED) || \
        defined(FSS_COSS_FTM_2_CH_0_ISR_USED) || defined(FSS_COSS_FTM_2_CH_1_ISR_USED) || defined(FSS_COSS_FTM_2_CH_2_ISR_USED) || \
        defined(FSS_COSS_FTM_2_CH_3_ISR_USED) || defined(FSS_COSS_FTM_2_CH_4_ISR_USED) || defined(FSS_COSS_FTM_2_CH_5_ISR_USED) || \
        defined(FSS_COSS_FTM_3_CH_0_ISR_USED) || defined(FSS_COSS_FTM_3_CH_1_ISR_USED) || defined(FSS_COSS_FTM_3_CH_2_ISR_USED) || \
        defined(FSS_COSS_FTM_3_CH_3_ISR_USED) || defined(FSS_COSS_FTM_3_CH_4_ISR_USED) || defined(FSS_COSS_FTM_3_CH_5_ISR_USED) )
/**
* @brief   Driver routine to process all the interrupts of FTM.
* @details Support function used by interrupt service routines to implement FTM specific operations
*          and call the upper layer handler to implement non-hardware specific operations.
*
* @param[in]     instance     FTM hardware instance number
* @param[in]     channel      FTM hardware channel number
* @implements    Ftm_Gpt_Ip_ProcessCommonInterrupt_Activity
*/
static void Ftm_Gpt_Ip_ProcessCommonInterrupt(uint8 instance, uint8 channel)
{
#if (FTM_GPT_IP_DEV_ERROR_DETECT == STD_ON)
    DevAssert(FTM_INSTANCE_COUNT > instance);
    DevAssert(FTM_CONTROLS_COUNT > channel);
#endif
    boolean HasChEvOccurred;
    boolean IsChEvEnabled;
    boolean                     chInit;
    Ftm_Gpt_Ip_CallbackType     callback;
    uint8                       callbackParam;
    Ftm_Gpt_Ip_ChannelModeType  channelMode;

    uint32 targetValue;
    uint32 oldCmpValue = Ftm_Gpt_Ip_GetCompareValue(instance, channel);

    /* get the driver status */
    chInit = Ftm_Gpt_Ip_u32ChState[instance][channel].chInit;
    /* enter critical section */
    SchM_Enter_Gpt_GPT_EXCLUSIVE_AREA_25();
    {
        /* check if channel event is enabled */
        IsChEvEnabled = Ftm_Gpt_Ip_GetChInterruptEnableFlag(instance, channel);

        /* check if channel event has occurred */
        HasChEvOccurred = Ftm_Gpt_Ip_GetChInterruptStatusFlag(instance, channel);

        if (TRUE == HasChEvOccurred)
        {
            /* Check if driver is initialized */
            if (TRUE == chInit)
            {
                /* Check for spurious interrupts */
                if (TRUE == IsChEvEnabled)
                {
                    /* Clear pending interrupts */
                    Ftm_Gpt_Ip_ClearChInterruptStatusFlag(instance, channel);
                }
            }
            else
            {
                /* Driver isn't initialized and just clear pending interrupts */
                Ftm_Gpt_Ip_ClearChInterruptStatusFlag(instance, channel);
            }
        }
    }
    /* exit critical section */
    SchM_Exit_Gpt_GPT_EXCLUSIVE_AREA_25();

    /* Check if driver is initialized */
    if (TRUE == chInit)
    {
        /* Check for spurious interrupts */
        if (IsChEvEnabled && HasChEvOccurred)
        {
            callback        = Ftm_Gpt_Ip_u32ChState[instance][channel].callback;
            callbackParam   = Ftm_Gpt_Ip_u32ChState[instance][channel].callbackParam;

            /* Call GPT upper layer handler */
            if (NULL_PTR != callback)
            {
                callback(callbackParam);
            }

#if (FTM_GPT_IP_CHANGE_NEXT_TIMEOUT_VALUE == STD_ON)
            if(Ftm_Gpt_Ip_u32NextTargetValue[instance][channel] != 0x0U)
            {
                Ftm_Gpt_Ip_u32TargetValue[instance][channel] = Ftm_Gpt_Ip_u32NextTargetValue[instance][channel];
                Ftm_Gpt_Ip_u32NextTargetValue[instance][channel] = 0x0U;
            }
#endif
            /* Check if channel mode is ONE-SHOT */
            channelMode = Ftm_Gpt_Ip_u32ChState[instance][channel].channelMode;
            if(FTM_GPT_IP_CH_MODE_ONESHOT == channelMode)
            {
                Ftm_Gpt_Ip_DisableChannelInterrupt(instance, channel);
            }
            else
            {
                targetValue = Ftm_Gpt_Ip_u32TargetValue[instance][channel];
                if ((uint32)(oldCmpValue + targetValue) > FTM_CNT_MAX_VALUE)
                {
                    Ftm_Gpt_Ip_SetCompareValue(instance, channel, (uint32)(targetValue - (FTM_CNT_MAX_VALUE - oldCmpValue)));
                }
                else
                {
                    /* Set new CMP value */
                    Ftm_Gpt_Ip_SetCompareValue(instance, channel, (uint32)(oldCmpValue + targetValue));
                }
            }
        }
    }
}

#endif

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

