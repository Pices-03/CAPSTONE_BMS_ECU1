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
/**
*   @file       Ftm_Gpt_Ip.c
*
*   @addtogroup ftm_ip Ftm IPL
*
*   @{
*/

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
*                                         INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "Ftm_Gpt_Ip.h"

#if (STD_ON == FTM_GPT_IP_ENABLE_USER_MODE_SUPPORT)
#define USER_MODE_REG_PROT_ENABLED  STD_ON
#include "RegLockMacros.h"
#else
#define USER_MODE_REG_PROT_ENABLED  STD_OFF
#endif
/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/

#define FTM_GPT_IP_VENDOR_ID_C                    43
#define FTM_GPT_IP_AR_RELEASE_MAJOR_VERSION_C     4
#define FTM_GPT_IP_AR_RELEASE_MINOR_VERSION_C     7
#define FTM_GPT_IP_AR_RELEASE_REVISION_VERSION_C  0
#define FTM_GPT_IP_SW_MAJOR_VERSION_C             3
#define FTM_GPT_IP_SW_MINOR_VERSION_C             0
#define FTM_GPT_IP_SW_PATCH_VERSION_C             0

/*==================================================================================================
*                                      FILE VERSION CHECKS
==================================================================================================*/
#if (FTM_GPT_IP_VENDOR_ID != FTM_GPT_IP_VENDOR_ID_C)
    #error "Ftm_Gpt_Ip.h and Ftm_Gpt_Ip.c have different vendor ids"
#endif
/* Check if header file and Gpt header file are of the same Autosar version */
#if ((FTM_GPT_IP_AR_RELEASE_MAJOR_VERSION != FTM_GPT_IP_AR_RELEASE_MAJOR_VERSION_C) || \
     (FTM_GPT_IP_AR_RELEASE_MINOR_VERSION != FTM_GPT_IP_AR_RELEASE_MINOR_VERSION_C) || \
     (FTM_GPT_IP_AR_RELEASE_REVISION_VERSION != FTM_GPT_IP_AR_RELEASE_REVISION_VERSION_C) \
    )
    #error "AutoSar Version Numbers of Ftm_Gpt_Ip.h and Ftm_Gpt_Ip.c are different"
#endif
/* Check if source file and GPT header file are of the same Software version */
#if ((FTM_GPT_IP_SW_MAJOR_VERSION != FTM_GPT_IP_SW_MAJOR_VERSION_C) || \
     (FTM_GPT_IP_SW_MINOR_VERSION != FTM_GPT_IP_SW_MINOR_VERSION_C) || \
     (FTM_GPT_IP_SW_PATCH_VERSION != FTM_GPT_IP_SW_PATCH_VERSION_C) \
    )
    #error "Software Version Numbers of Ftm_Gpt_Ip.h and Ftm_Gpt_Ip.c are different"
#endif

#if (STD_ON == FTM_GPT_IP_ENABLE_USER_MODE_SUPPORT)
#ifndef DISABLE_MCAL_INTERMODULE_ASR_CHECK
    /* Check if source file and Platform_Types.h header file are of the same Autosar version */
    #if ((REGLOCKMACROS_AR_RELEASE_MAJOR_VERSION != FTM_GPT_IP_AR_RELEASE_MAJOR_VERSION_C) || \
         (REGLOCKMACROS_AR_RELEASE_MINOR_VERSION != FTM_GPT_IP_AR_RELEASE_MINOR_VERSION_C))
        #error "AutoSar Version Numbers of RegLockMacros.h and Ftm_Gpt_Ip.c are different"
    #endif
#endif
#endif
/*==================================================================================================
*                                          LOCAL DEFINIES
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
*                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/**
 * @brief       internal context enumeration
 * @details     This enumeration is used by the IPL driver for internal logic.
 *
 */
typedef enum
{
    FTM_GPT_IP_STATUS_UNINITIALIZED = 0U,   /**< @brief FTM instance status - uninitialized */
    FTM_GPT_IP_STATUS_INITIALIZED   = 1U,   /**< @brief FTM instance status - initialized */
} Ftm_Gpt_Ip_InstanceStatusType;

/*==================================================================================================
*                                       LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                       GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                       GLOBAL VARIABLES
==================================================================================================*/
#if (FTM_GPT_IP_USED == STD_ON)

#define GPT_START_SEC_VAR_CLEARED_32_NO_CACHEABLE
#include "Gpt_MemMap.h"
/** @brief Global array variable used to store the runtime target time value. */
uint32 Ftm_Gpt_Ip_u32TargetValue[FTM_INSTANCE_COUNT][FTM_CONTROLS_COUNT];
#define GPT_STOP_SEC_VAR_CLEARED_32_NO_CACHEABLE
#include "Gpt_MemMap.h"

#define GPT_START_SEC_CONST_UNSPECIFIED
#include "Gpt_MemMap.h"

/* Table of base pointers for FTM instances. */
FTM_Type * const ftmGptBase[FTM_INSTANCE_COUNT] = IP_FTM_BASE_PTRS;

#define GPT_STOP_SEC_CONST_UNSPECIFIED
#include "Gpt_MemMap.h"

#define GPT_START_SEC_VAR_CLEARED_UNSPECIFIED_NO_CACHEABLE
#include "Gpt_MemMap.h"

static Ftm_Gpt_Ip_InstanceStatusType Ftm_Gpt_Ip_u32InstanceState[FTM_INSTANCE_COUNT];

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
Ftm_Gpt_Ip_ChState Ftm_Gpt_Ip_u32ChState[FTM_INSTANCE_COUNT][FTM_CONTROLS_COUNT];
#endif
#if (FTM_GPT_IP_SET_CLOCK_MODE == STD_ON)
static Ftm_Gpt_Ip_InstancePrescalerType Ftm_Gpt_Ip_u32InstancePrescaler[FTM_INSTANCE_COUNT];
#endif
/** @brief Global array variable used to store clock source configuration for each channel. */
static Ftm_Gpt_Ip_ClockSource Ftm_Gpt_Ip_u32InstanceClockSource[FTM_INSTANCE_COUNT];
#define GPT_STOP_SEC_VAR_CLEARED_UNSPECIFIED_NO_CACHEABLE
#include "Gpt_MemMap.h"

#define GPT_START_SEC_VAR_CLEARED_32_NO_CACHEABLE
#include "Gpt_MemMap.h"
#if (   (FTM_GPT_IP_CHANGE_NEXT_TIMEOUT_VALUE == STD_ON) && \
        (   defined(FTM_0_ISR_USED) || defined(FTM_1_ISR_USED) || \
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
            defined(FSS_COSS_FTM_3_CH_3_ISR_USED) || defined(FSS_COSS_FTM_3_CH_4_ISR_USED) || defined(FSS_COSS_FTM_3_CH_5_ISR_USED) ) )
uint32 Ftm_Gpt_Ip_u32NextTargetValue[FTM_INSTANCE_COUNT][FTM_CONTROLS_COUNT];
#endif
#define GPT_STOP_SEC_VAR_CLEARED_32_NO_CACHEABLE
#include "Gpt_MemMap.h"
/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
#define GPT_START_SEC_CODE
#include "Gpt_MemMap.h"
#if (defined(MCAL_FTM_REG_PROT_AVAILABLE))
#if ((STD_ON == MCAL_FTM_REG_PROT_AVAILABLE) && (STD_ON == FTM_GPT_IP_ENABLE_USER_MODE_SUPPORT))
    void Ftm_Gpt_Ip_SetUserAccessAllowed(uint32 FtmBaseAddr);
#endif
#endif

#if (defined(MCAL_FTM_REG_PROT_AVAILABLE))
#if ((STD_ON == MCAL_FTM_REG_PROT_AVAILABLE) && (STD_ON == FTM_GPT_IP_ENABLE_USER_MODE_SUPPORT))
    #define Call_Ftm_Gpt_Ip_SetUserAccessAllowed(BaseAddr) OsIf_Trusted_Call1param(Ftm_Gpt_Ip_SetUserAccessAllowed,(BaseAddr))
#else
    #define Call_Ftm_Gpt_Ip_SetUserAccessAllowed(BaseAddr)
#endif
#else
    #define Call_Ftm_Gpt_Ip_SetUserAccessAllowed(BaseAddr)
#endif

/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/
#if (defined(MCAL_FTM_REG_PROT_AVAILABLE))
#if ((STD_ON == MCAL_FTM_REG_PROT_AVAILABLE) && (STD_ON == FTM_GPT_IP_ENABLE_USER_MODE_SUPPORT))
/**
 * @brief        Enables FTM registers writing in User Mode by configuring REG_PROT
 * @details      Sets the UAA (User Access Allowed) bit of the FTM IP allowing FTM registers writing in User Mode
 *
 * @param[in]    FtmBaseAddr
 *
 * @return       none
 *
 * @pre          Should be executed in supervisor mode
 */
void Ftm_Gpt_Ip_SetUserAccessAllowed(uint32 FtmBaseAddr)
{
    SET_USER_ACCESS_ALLOWED(FtmBaseAddr, FTM_PROT_MEM_U32);
}
#endif
#endif
/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/
/**
* @brief         Function Name : Ftm_Gpt_Ip_Init
* @details       Initializes the FTM instance. This functions is called for each FTM hardware Instance and:
*
* @param[in]     instance     FTM hardware instance number
* @param[in]     configPtr    Pointer to a selected configuration structure
* @return        void
* @pre           The data structure including the configuration set required for initializing the GPT driver
* @implements    Ftm_Gpt_Ip_Init_Activity
*/
void Ftm_Gpt_Ip_Init(uint8 instance, const Ftm_Gpt_Ip_InstanceConfigType *configPtr)
{
#if (STD_ON == FTM_GPT_IP_DEV_ERROR_DETECT)
    DevAssert(FTM_INSTANCE_COUNT > instance);
    DevAssert(NULL_PTR != configPtr);
#endif
    /* Enable register access from user mode, if enabled from configuration file */
    Call_Ftm_Gpt_Ip_SetUserAccessAllowed((uint32)ftmGptBase[instance]);

    /* Check the FTM counter modes */
    if (FTM_GPT_IP_MODE_UP_TIMER == configPtr->mode)
    {
        /* Set clock and prescalerValue FTM */
        Ftm_Gpt_Ip_SetPrescaler(instance, configPtr->clockPrescaler);
        /* Set FTM counter clock source */
        Ftm_Gpt_Ip_SetClockSource(instance, configPtr->clocksource);

        /* Set freeze bits */
        Ftm_Gpt_Ip_SetFreezeBits(instance, configPtr->freezeBits);

        /*Set Counter register and Initial Counter Value*/
        Ftm_Gpt_Ip_SetCounterInitVal(instance, 1U);

        /* write the Ftm Modulo Register with 0xFFFF */
        Ftm_Gpt_Ip_WriteModulo(instance, 0xFFFFU);

        /* set FTM Enable bit */
        Ftm_Gpt_Ip_SetEnFtmModule(instance, TRUE);
    }
    else
    {
        /* TODO: DO NOTHING */
    }
#if (FTM_GPT_IP_SET_CLOCK_MODE == STD_ON)
    Ftm_Gpt_Ip_u32InstancePrescaler[instance].clockPrescaler = configPtr->clockPrescaler;
    Ftm_Gpt_Ip_u32InstancePrescaler[instance].clockAlternatePrescaler = configPtr->clockAlternatePrescaler;
#endif
    /* save clock source config */
    Ftm_Gpt_Ip_u32InstanceClockSource[instance] = configPtr->clocksource;
    /* set FTM instance status to initialized */
    Ftm_Gpt_Ip_u32InstanceState[instance] = FTM_GPT_IP_STATUS_INITIALIZED;
}

/*================================================================================================*/
/**
* @brief         Function Name : Ftm_Gpt_Ip_InitChannel
* @details       Initializes the FTM channels. This functions is called for each FTM hardware channel and:
*
* @param[in]     instance     FTM hardware instance number
* @param[in]     configPtr    Pointer to a selected configuration structure
* @return        void
* @pre           The data structure including the configuration set required for initializing the GPT driver
* @implements    Ftm_Gpt_Ip_InitChannel_Activity
*/
void Ftm_Gpt_Ip_InitChannel(uint8 instance, const Ftm_Gpt_Ip_ChannelConfigType *configPtr)
{
#if (STD_ON == FTM_GPT_IP_DEV_ERROR_DETECT)
    DevAssert(NULL_PTR != configPtr);
    DevAssert(FTM_INSTANCE_COUNT > instance);
    DevAssert(FTM_CONTROLS_COUNT > configPtr->hwChannel);
#endif

    /* Disable channels interrupt bit */
    Ftm_Gpt_Ip_SetChannelEnableInterrupt(instance, configPtr->hwChannel, FALSE);
    /* Clear ClearInterruptFlag */
    Ftm_Gpt_Ip_ClearChInterruptStatusFlag(instance, configPtr->hwChannel);
    /* Set Compare Value register to 0xFFFF */
    Ftm_Gpt_Ip_SetCompareValue(instance, configPtr->hwChannel, 0xFFFFU);
    /* Set bit MSA */
    Ftm_Gpt_Ip_ModeSelectA(instance, configPtr->hwChannel, TRUE);
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
    Ftm_Gpt_Ip_u32ChState[instance][configPtr->hwChannel].chInit = TRUE;
    Ftm_Gpt_Ip_u32ChState[instance][configPtr->hwChannel].callback = configPtr->callback;
    Ftm_Gpt_Ip_u32ChState[instance][configPtr->hwChannel].callbackParam = configPtr->callbackParam;
    Ftm_Gpt_Ip_u32ChState[instance][configPtr->hwChannel].channelMode = configPtr->channelMode;
#endif
}

/*================================================================================================*/
/**
* @brief        Function Name : Ftm_Gpt_Ip_Deinit
* @details      De-Initializes the FTM module. This functions is called for each FTM hardware instance and:
*                    - resets all channels to default
*                    - disables the timer compare interrupts corresponding to Ftm channel
*                    - clears the timer compare interrupt flags corresponding to Ftm channel
*                    - resets the counter register and the counter initial value register.
*                    - resets the channel value register and the modulo register
*                    - disables the freeze mode
*
* @param[in]     instance     FTM hardware instance number
* @return        void
* @pre           The data structure including the configuration set required for initializing the GPT driver.
* @implements    Ftm_Gpt_Ip_Deinit_Activity
*/
void Ftm_Gpt_Ip_Deinit(uint8 instance)
{
#if (STD_ON == FTM_GPT_IP_DEV_ERROR_DETECT)
    DevAssert(FTM_INSTANCE_COUNT > instance);
#endif

    uint8 channelIndex;

    /* Reset all channels to default */
    for (channelIndex = 0; channelIndex < FTM_CONTROLS_COUNT; channelIndex++)
    {
        /* Disable interrupts */
        Ftm_Gpt_Ip_SetChannelEnableInterrupt(instance, channelIndex, FALSE );
        /*Clear interrupt flags*/
        Ftm_Gpt_Ip_ClearChInterruptStatusFlag(instance, channelIndex);
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
        /* Set state variab values to default value */
        Ftm_Gpt_Ip_u32ChState[instance][channelIndex].chInit = FALSE;
        Ftm_Gpt_Ip_u32ChState[instance][channelIndex].callback = NULL_PTR;
        Ftm_Gpt_Ip_u32ChState[instance][channelIndex].callbackParam = 0;
#endif
    }
    /* Set clock source as 'No clock'. This in effect disables the FTM counter */
    Ftm_Gpt_Ip_SetClockSource(instance, FTM_GPT_IP_CLOCK_SOURCE_NONE);
    /* clear FTM Enable bit */
    Ftm_Gpt_Ip_SetEnFtmModule(instance, FALSE);
    /* clearde freeze bits */
    Ftm_Gpt_Ip_SetFreezeBits(instance, FALSE);
    /* Set Counter register to 0 and Initial Counter Value to 0 */
    Ftm_Gpt_Ip_SetCounterInitVal(instance, 0U);
    Ftm_Gpt_Ip_SetCounter(instance, 0U);
    /* set FTM instance status to uninitialized */
    Ftm_Gpt_Ip_u32InstanceState[instance] = FTM_GPT_IP_STATUS_UNINITIALIZED;
    /* set FTM instance clock source to none */
    Ftm_Gpt_Ip_u32InstanceClockSource[instance] = FTM_GPT_IP_CLOCK_SOURCE_NONE;
}

/*================================================================================================*/
/**
* @brief        Function Name : Ftm_Gpt_Ip_StartCounting
* @details      This function is called for starting the Ftm timer channel
*
* @param[in]     instance        FTM hardware instance
* @param[in]     channel         FTM hardware channel
* @param[in]     compareValue    Compare value
* @return        void
* @pre           The driver needs to be initialized. This function is called for starting the FTM timer channel.
* @implements    Ftm_Gpt_Ip_StartCounting_Activity
*/
void Ftm_Gpt_Ip_StartCounting(uint8 instance, uint8 channel, uint16 compareValue)
{
#if (STD_ON == FTM_GPT_IP_DEV_ERROR_DETECT)
    DevAssert(FTM_INSTANCE_COUNT > instance);
    DevAssert(FTM_CONTROLS_COUNT > channel);
#endif
    uint32 counterValue;
    uint32 currentCntValue = Ftm_Gpt_Ip_GetCntValue(instance);

    if ((uint32)(currentCntValue + compareValue) > FTM_CNT_MAX_VALUE)
    {
        counterValue = (uint32)(compareValue - (FTM_CNT_MAX_VALUE - currentCntValue));
    }
    else
    {
        counterValue = (uint32)(currentCntValue + compareValue);
    }
    /* Set new compare value */
    Ftm_Gpt_Ip_SetCompareValue(instance, channel, counterValue);
    /* Save compare value */
    Ftm_Gpt_Ip_u32TargetValue[instance][channel] = compareValue;
    /* Clear CHF flag */
    Ftm_Gpt_Ip_ClearChInterruptStatusFlag(instance,channel);
    /* Enable interrupt */
    Ftm_Gpt_Ip_SetChannelEnableInterrupt(instance, channel, TRUE);

}

/*================================================================================================*/
#if FTM_GPT_IP_ABSOLUTE_COUNTING_API == STD_ON
/**
* @brief        Function Name : Ftm_Gpt_Ip_StartCountingAbsolute
* @details      This function is called for starting the Ftm timer channel
*                    - sets the compare value without adding the current counter value to the timeout value
*                    - enables the FTM channel
*
* @param[in]     instance        FTM hardware instance
* @param[in]     channel         FTM hardware channel
* @param[in]     compareValue    Compare value
* @return        void
* @pre           The driver needs to be initialized. This function is called for starting the FTM timer channel.
* @implements    Ftm_Gpt_Ip_StartCountingAbsolute_Activity
*/
void Ftm_Gpt_Ip_StartCountingAbsolute(uint8 instance, uint8 channel, uint16 compareValue)
{
#if (STD_ON == FTM_GPT_IP_DEV_ERROR_DETECT)
    DevAssert(FTM_INSTANCE_COUNT > instance);
    DevAssert(FTM_CONTROLS_COUNT > channel);
#endif

    Ftm_Gpt_Ip_u32TargetValue[instance][channel] = 0U;
    /* Set new compare value */
    Ftm_Gpt_Ip_SetCompareValue(instance, channel, compareValue);
    /* Clear CHF flag */
    Ftm_Gpt_Ip_ClearChInterruptStatusFlag(instance,channel);
    /* Enable interrupt */
    Ftm_Gpt_Ip_SetChannelEnableInterrupt(instance, channel, TRUE);

}
#endif

/*================================================================================================*/
/**
 * @brief       Function Name : Ftm_Gpt_Ip_StartTimer
 * @details     This function is called for setting a new start counter value and enables the FTM counter and
 *                  - sets the new counter value
 *                  - enables the FTM counter
 * @pre           The driver needs to be initialized.
 * @implements    Ftm_Gpt_Ip_StartTimer_Activity
 */
void Ftm_Gpt_Ip_StartTimer(uint8 instance, uint16 counterValue)
{
#if (STD_ON == FTM_GPT_IP_DEV_ERROR_DETECT)
    DevAssert(FTM_INSTANCE_COUNT > instance);
#endif
    if (instance < FTM_INSTANCE_COUNT)
    {
        if (FTM_GPT_IP_STATUS_INITIALIZED == Ftm_Gpt_Ip_u32InstanceState[instance])
        {
            /* Set the new value counter */
            Ftm_Gpt_Ip_SetCounterInitVal(instance, counterValue);
            /* set FTM Enable bit */
            Ftm_Gpt_Ip_SetEnFtmModule(instance, TRUE);
            /* Set FTM counter clock source */
            Ftm_Gpt_Ip_SetClockSource(instance, Ftm_Gpt_Ip_u32InstanceClockSource[instance]);
        }
    }
}

/*================================================================================================*/
/**
* @brief        Function Name : Ftm_Gpt_Ip_StopTimer
* @details      This function is callded for stopping the Ftm counter.
*                   - disables the FTM counter
*
* @param[in]     instance        FTM hardware instance
* @return        void
* @pre           The driver needs to be initialized. This function is called for stoping the FTM timer channel.
* @implements    Ftm_Gpt_Ip_StopTimer_Activity
*/
void Ftm_Gpt_Ip_StopTimer(uint8 instance)
{
#if (STD_ON == FTM_GPT_IP_DEV_ERROR_DETECT)
DevAssert(FTM_INSTANCE_COUNT > instance);
#endif
    /* clear FTM Enable bit */
    Ftm_Gpt_Ip_SetEnFtmModule(instance, FALSE);
    /* Set FTM counter clock source */
    Ftm_Gpt_Ip_SetClockSource(instance, FTM_GPT_IP_CLOCK_SOURCE_NONE);
}

/*================================================================================================*/
/**
* @brief        Function Name : Ftm_Gpt_Ip_EnableChannelInterrupt
* @details      This function allows enabling interrupt generation of timer channel
*               when time-out occurs
*
* @param[in]    instance        FTM hardware instance
* @param[in]    channel         FTM hardware channel
* @return       void
* @pre          The driver needs to be initialized.
* @implements   Ftm_Gpt_Ip_EnableChannelInterrupt_Activity
*/
void Ftm_Gpt_Ip_EnableChannelInterrupt(uint8 instance, uint8 channel)
{
#if (STD_ON == FTM_GPT_IP_DEV_ERROR_DETECT)
DevAssert(FTM_INSTANCE_COUNT > instance);
DevAssert(FTM_CONTROLS_COUNT > channel);
#endif

     /* Clear interrupt flag */
    Ftm_Gpt_Ip_ClearChInterruptStatusFlag(instance, channel);
    /* Enable channel interrupt */
    Ftm_Gpt_Ip_SetChannelEnableInterrupt(instance, channel, TRUE);
}

/*================================================================================================*/
/**
* @brief        Function Name : Ftm_Gpt_Ip_DisableChannelInterrupt
* @details      This function allows disabling interrupt generation of timer channel
*               when time-out occurs
*
* @param[in]     instance        FTM hardware instance
* @param[in]     channel         FTM hardware channel
* @return        void
* @pre           The driver needs to be initialized.
* @implements    Ftm_Gpt_Ip_DisableChannelInterrupt_Activity
*/
void Ftm_Gpt_Ip_DisableChannelInterrupt(uint8 instance, uint8 channel)
{
#if (STD_ON == FTM_GPT_IP_DEV_ERROR_DETECT)
DevAssert(FTM_INSTANCE_COUNT > instance);
DevAssert(FTM_CONTROLS_COUNT > channel);
#endif

    /* Disable interrupt */
    Ftm_Gpt_Ip_SetChannelEnableInterrupt(instance, channel, FALSE);
    /* Clear interrupt flag */
    Ftm_Gpt_Ip_ClearChInterruptStatusFlag(instance, channel);
}

/*================================================================================================*/
/**
* @brief         Function Name : Ftm_Gpt_Ip_SetHalfCycleReloadPoint
* @details       Configures the value of the counter with half cycle of reload point.
*
* @param[in]     instance                  FTM hardware instance
* @param[in]     reloadPoint               Reload value

* @return
* @pre           The driver needs to be initialized.
* @implements    Ftm_Gpt_Ip_SetHalfCycleReloadPoint_Activity
*/
void Ftm_Gpt_Ip_SetHalfCycleReloadPoint(uint8 instance, uint16 reloadPoint)
{
#if (STD_ON == FTM_GPT_IP_DEV_ERROR_DETECT)
DevAssert(FTM_INSTANCE_COUNT > instance);
#endif
    Ftm_Gpt_Ip_SetHalfCycleValue(ftmGptBase[instance], reloadPoint);
}

#if (FTM_GPT_IP_CHANGE_NEXT_TIMEOUT_VALUE == STD_ON)
/*================================================================================================*/
/**
* @brief      The function changes the Ftm compare register value.
* @details This function:
*          - Write next timeout to local variable
*
* @param[in]     instance        FTM hardware instance
* @param[in]     channel         Channel
* @param[in]     value           Channel timeout value
* @return        void
* @pre           The driver needs to be initialized.
* @implements    Ftm_Gpt_Ip_ChangeNextTimeoutValue_Activity
*/
void Ftm_Gpt_Ip_ChangeNextTimeoutValue(uint8 instance, uint8 channel, uint16 value)
{
#if (STD_ON == FTM_GPT_IP_DEV_ERROR_DETECT)
    DevAssert(FTM_INSTANCE_COUNT > instance);
    DevAssert(FTM_CONTROLS_COUNT > channel);
#endif
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

    /* Update the target time value to be used on next cycle */
    Ftm_Gpt_Ip_u32NextTargetValue[instance][channel] = (uint32)value;

#endif
}
#endif /*FTM_GPT_IP_CHANGE_NEXT_TIMEOUT_VALUE*/

#if (FTM_GPT_IP_SET_CLOCK_MODE == STD_ON)
/*================================================================================================*/
/**
* @brief        The function changes the FTM prescaler value.
* @details      This function sets the FTM prescaler based on the input mode.
*
* @param[in]    instance     FTM hardware instance
* @param[in]    prescalerMode    FTM_GPT_IP_CLOCKMODE_NORMAL or FTM_GPT_IP_CLOCKMODE_ALTERNATE
*
* @return       void
* @pre          The driver needs to be initialized.On/Off by the configuration parameter: GPT_DUAL_CLOCK_MODE
* @implements   Ftm_Gpt_Ip_SetClockMode_Activity
*/
void Ftm_Gpt_Ip_SetClockMode(uint8 instance, Ftm_Gpt_Ip_ClockModeType prescalerMode)
{
#if (STD_ON == FTM_GPT_IP_DEV_ERROR_DETECT)
    DevAssert(FTM_INSTANCE_COUNT > instance);
#endif

    /* Set Prescaler */
    if(FTM_GPT_IP_CLOCKMODE_NORMAL == prescalerMode)
    {
        Ftm_Gpt_Ip_SetPrescaler(instance, Ftm_Gpt_Ip_u32InstancePrescaler[instance].clockPrescaler);
    }
    else
    {
        Ftm_Gpt_Ip_SetPrescaler(instance, Ftm_Gpt_Ip_u32InstancePrescaler[instance].clockAlternatePrescaler);
    }
}
#endif/*FTM_GPT_IP_SET_CLOCK_MODE*/

#if(STD_ON == FTM_GPT_IP_PREDEFTIMER_FUNCTIONALITY_API)
/*================================================================================================*/
/**
* @brief      The function start the FTM channel.
* @details    This function start the FTM channel the input mode.
*
* @param[in]  instance        FTM hardware instance
* @param[in]  uPrescaler      Prescaler
* @param[in]  bFreezeEnable   enables/disables freeze bits
* @return     void
* @pre        The driver needs to be initialized
*/
void Ftm_Gpt_Ip_StartPredefTimer (uint8 instance,uint8 channel, uint8 uPrescaler, uint8 clocksource, boolean bFreezeEnable)
{
#if (STD_ON == FTM_GPT_IP_DEV_ERROR_DETECT)
    DevAssert(FTM_INSTANCE_COUNT > instance);
    DevAssert(FTM_CONTROLS_COUNT > channel);
#endif

    /* Enable register access from user mode, if enabled from configuration file */
    Call_Ftm_Gpt_Ip_SetUserAccessAllowed((uint32)ftmGptBase[instance]);

    /* Disable all interrupts on this channel */
    Ftm_Gpt_Ip_SetChannelEnableInterrupt(instance, channel, FALSE);
    /* Clear interrupt flags */
    Ftm_Gpt_Ip_ClearChInterruptStatusFlag(instance, channel);
    /* Disable TOIE flags */
    ftmGptBase[instance]-> SC &= (~(FTM_SC_TOIE_MASK));
    /* Configure FTM as a Free-Running Timer by setting FTMEN=1, CTNIN=0, MOD=0xFFFF, QUADEN=0, CPWMS=0*/
    Ftm_Gpt_Ip_SetEnFtmModule(instance, TRUE);
    /* Initial Counter Value to 0 */
    Ftm_Gpt_Ip_SetCounterInitVal(instance, 0U);
    /* write the Ftm Modulo Register with 0xFFFF */
    Ftm_Gpt_Ip_WriteModulo(instance, 0xFFFFU);
    /* Set Compare Value register to 0xFFFF */
    Ftm_Gpt_Ip_SetCompareValue(instance, channel, 0xFFFFU);
    /* Enter EA_28*/
    SchM_Enter_Gpt_GPT_EXCLUSIVE_AREA_26();
    /* Clear bit QUADEN */
    ftmGptBase[instance]->QDCTRL &= (~(FTM_QDCTRL_QUADEN_MASK));
    /* Clear bit CPWMS */
    ftmGptBase[instance]->SC &= (~(FTM_SC_CPWMS_MASK));
    /* Exit EA_28*/
    SchM_Exit_Gpt_GPT_EXCLUSIVE_AREA_26();

    /* Configure Predef Timer */
    Ftm_Gpt_Ip_SetPrescaler(instance, uPrescaler);
    /* Set FTM counter clock source */
    Ftm_Gpt_Ip_SetClockSource(instance, (Ftm_Gpt_Ip_ClockSource)clocksource);
    /* Set freeze bits */
    Ftm_Gpt_Ip_SetFreezeBits(instance, bFreezeEnable);
    /* Intialize FTM timer by writing any value to CNT register when CLKS not equal 0 */
    Ftm_Gpt_Ip_SetCounter(instance, 1U);
}
/*================================================================================================*/
/**
* @brief      The function stops the FTM channel.
* @details    This function stops the FTM channel.
*
* @param[in]  channel      hardware channel
* @param[in]  instance     FTM hardware instance
*
*
* @return     void
* @pre        The driver needs to be initialized
*
*/
void Ftm_Gpt_Ip_StopPredefTimer (uint8 instance, uint8 channel)
{
#if (STD_ON == FTM_GPT_IP_DEV_ERROR_DETECT)
    DevAssert(FTM_INSTANCE_COUNT > instance);
    DevAssert(FTM_CONTROLS_COUNT > channel);
#endif
    /* Set clock source as 'No clock'. This in effect disables the FTM counter */
    Ftm_Gpt_Ip_SetClockSource(instance, FTM_GPT_IP_CLOCK_SOURCE_NONE);
    /* Disable all interrupts on this channel */
    Ftm_Gpt_Ip_SetChannelEnableInterrupt(instance, channel, FALSE);
    /* Clear interrupt flags */
    Ftm_Gpt_Ip_ClearChInterruptStatusFlag(instance, channel);
    /* clear FTM Enable bit */
    Ftm_Gpt_Ip_SetEnFtmModule(instance, FALSE);
    /* Set Counter register to 0 and Initial Counter Value to 0 */
    Ftm_Gpt_Ip_SetCounterInitVal(instance, 0U);
    Ftm_Gpt_Ip_SetCounter(instance, 0U);
    Ftm_Gpt_Ip_SetCompareValue(instance, channel, 0U);
    Ftm_Gpt_Ip_WriteModulo(instance, 0U );
    /* Set bit MSA */
    Ftm_Gpt_Ip_ModeSelectA(instance, channel, FALSE);
}
#endif /* STD_ON == FTM_GPT_IP_PREDEFTIMER_FUNCTIONALITY_API */

#define GPT_STOP_SEC_CODE
#include "Gpt_MemMap.h"

#endif /* FTM_GPT_IP_USED == STD_ON */
#ifdef __cplusplus
}
#endif  /*FTM_GPT_IP_C*/
/** @} */

