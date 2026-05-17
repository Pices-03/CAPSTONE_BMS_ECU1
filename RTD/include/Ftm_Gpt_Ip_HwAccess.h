/*==================================================================================================
* Project              : RTD AUTOSAR 4.7
* Platform             : CORTEXM
* Peripheral           : Ftm_Srtc_Lptmr_LPit
* Dependencies         : none
*
* Autosar Version      : 4.7.0
* Autosar Revision     : ASR_REL_4_7_REV_0000
* Autosar Conf.Variant :
* SW Version           : 3.0.0
* Build Version        : S32K1_RTD_3_0_0_QLP06_D2603_ASR_REL_4_7_REV_0000_20260320
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

#ifndef FTM_GPT_IP_HWACCESS_H
#define FTM_GPT_IP_HWACCESS_H

/**
*   @file       Ftm_Gpt_Ip_HwAccess.h
*
*   @addtogroup ftm_ip Ftm IPL
*   @{
*/

#ifdef __cplusplus
extern "C"
{
#endif

/*==================================================================================================
*                                          INCLUDE FILES
*  1) system and project includes
*  2) needed interfaces from external units
*  3) internal and external interfaces from this unit
==================================================================================================*/

#include "Std_Types.h"
#include "Ftm_Gpt_Ip_Cfg.h"
#include "OsIf.h"
#include "Mcal.h"
#include "SchM_Gpt.h"

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define FTM_GPT_IP_HWACCESS_VENDOR_ID                    43
#define FTM_GPT_IP_HWACCESS_AR_RELEASE_MAJOR_VERSION     4
#define FTM_GPT_IP_HWACCESS_AR_RELEASE_MINOR_VERSION     7
#define FTM_GPT_IP_HWACCESS_AR_RELEASE_REVISION_VERSION  0
#define FTM_GPT_IP_HWACCESS_SW_MAJOR_VERSION             3
#define FTM_GPT_IP_HWACCESS_SW_MINOR_VERSION             0
#define FTM_GPT_IP_HWACCESS_SW_PATCH_VERSION             0


/*==================================================================================================
*                                       FILE VERSION CHECKS
==================================================================================================*/

/* Check if header file and Std_Types.h are of the same AUTOSAR version */
#ifndef DISABLE_MCAL_INTERMODULE_ASR_CHECK
    #if ((FTM_GPT_IP_HWACCESS_AR_RELEASE_MAJOR_VERSION != STD_AR_RELEASE_MAJOR_VERSION) || \
         (FTM_GPT_IP_HWACCESS_AR_RELEASE_MINOR_VERSION != STD_AR_RELEASE_MINOR_VERSION))
        #error "AUTOSAR version numbers of Ftm_Gpt_Ip_HwAccess.h and Std_Types.h are different."
    #endif
#endif

/* Check if header file and Ftm_Gpt_Ip_Cfg.h header file are of the same vendor */
#if (FTM_GPT_IP_HWACCESS_VENDOR_ID != FTM_GPT_IP_VENDOR_ID_CFG)
    #error "Vendor IDs of Ftm_Gpt_Ip_HwAccess.h and Ftm_Gpt_Ip_Cfg.h are different."
#endif

/* Check if header file and Ftm_Gpt_Ip_Cfg.h header file are of the same AUTOSAR version */
#if ((FTM_GPT_IP_HWACCESS_AR_RELEASE_MAJOR_VERSION    != FTM_GPT_IP_AR_RELEASE_MAJOR_VERSION_CFG) || \
     (FTM_GPT_IP_HWACCESS_AR_RELEASE_MINOR_VERSION    != FTM_GPT_IP_AR_RELEASE_MINOR_VERSION_CFG) || \
     (FTM_GPT_IP_HWACCESS_AR_RELEASE_REVISION_VERSION != FTM_GPT_IP_AR_RELEASE_REVISION_VERSION_CFG))
    #error "AUTOSAR version numbers of Ftm_Gpt_Ip_HwAccess.h and Ftm_Gpt_Ip_Cfg.h are different."
#endif

/* Check if header file and Ftm_Gpt_Ip_Cfg.h header file are of the same software version */
#if ((FTM_GPT_IP_HWACCESS_SW_MAJOR_VERSION != FTM_GPT_IP_SW_MAJOR_VERSION_CFG) || \
     (FTM_GPT_IP_HWACCESS_SW_MINOR_VERSION != FTM_GPT_IP_SW_MINOR_VERSION_CFG) || \
     (FTM_GPT_IP_HWACCESS_SW_PATCH_VERSION != FTM_GPT_IP_SW_PATCH_VERSION_CFG))
    #error "Software version numbers of Ftm_Gpt_Ip_HwAccess.h and Ftm_Gpt_Ip_Cfg.h are different."
#endif

#ifndef DISABLE_MCAL_INTERMODULE_ASR_CHECK
#if ((FTM_GPT_IP_HWACCESS_AR_RELEASE_MAJOR_VERSION != SCHM_GPT_AR_RELEASE_MAJOR_VERSION) || \
(FTM_GPT_IP_HWACCESS_AR_RELEASE_MINOR_VERSION != SCHM_GPT_AR_RELEASE_MINOR_VERSION))
#error "AutoSar Version Numbers of Ftm_Gpt_Ip_HwAccess.h and SchM_Gpt.h are different"
#endif
/* Check if this header file and OsIf.h file are of the same Autosar version */
#if ((FTM_GPT_IP_HWACCESS_AR_RELEASE_MAJOR_VERSION != OSIF_AR_RELEASE_MAJOR_VERSION) || \
(FTM_GPT_IP_HWACCESS_AR_RELEASE_MINOR_VERSION != OSIF_AR_RELEASE_MINOR_VERSION))
#error "AutoSar Version Numbers of Ftm_Gpt_Ip_HwAccess.h and OsIf.h are different"
#endif

/* Check if source file and Std_Types.h header file are of the same Autosar version */
#if ((FTM_GPT_IP_HWACCESS_AR_RELEASE_MAJOR_VERSION != MCAL_AR_RELEASE_MAJOR_VERSION) || \
     (FTM_GPT_IP_HWACCESS_AR_RELEASE_MINOR_VERSION != MCAL_AR_RELEASE_MINOR_VERSION))
    #error "AutoSar Version Numbers of Ftm_Gpt_Ip_HwAccess.h and Mcal.h  are different"
#endif
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
#if (FTM_GPT_IP_USED == STD_ON)

#define GPT_START_SEC_CONST_UNSPECIFIED
#include "Gpt_MemMap.h"

/* Table of base pointers for FTM instances. */
extern FTM_Type * const ftmGptBase[FTM_INSTANCE_COUNT];

#define GPT_STOP_SEC_CONST_UNSPECIFIED
#include "Gpt_MemMap.h"
/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL FUNCTION
==================================================================================================*/
/**
* @internal
* @brief MemMap section
*/
#define GPT_START_SEC_CODE
#include "Gpt_MemMap.h"
/**
 * @brief
 * Function Name : Ftm_Gpt_Ip_SetFreezeBits
 * Description   : Set BDMMODE[1:0]:    7-6 bits in Configuration (CONF) register
 *      BDMMODE: Selects the FTM behavior in Debug mode. See Debug mode.
 *          BDMMODE     FTM Channels Output
 *            00            Functional mode
 *            01            The channels outputs are forced to their safe value according to POLn bit
 *            10            The channels outputs are frozen when the chip enters in Debug mode
 *            11            Functional mode
 *
 * @param[in]   instance    FTM hardware instance number
 * @param[in]   freezeBits  true or false
 *
 * @return      void
 *
 */
static inline void Ftm_Gpt_Ip_SetFreezeBits(uint8 instance, boolean freezeBits)
{
    SchM_Enter_Gpt_GPT_EXCLUSIVE_AREA_17();
    ftmGptBase[instance]->CONF = (ftmGptBase[instance]->CONF & ~FTM_CONF_BDMMODE_MASK) | FTM_CONF_BDMMODE((TRUE == freezeBits) ? 0UL : 3UL);
    SchM_Exit_Gpt_GPT_EXCLUSIVE_AREA_17();
}

/*================================================================================================*/
/**
* @brief
* Function Name : Ftm_Gpt_Ip_SetCounterInitVal
* Description   : Set the CounterInitVal
*
* @param[in]     instance       FTM hardware instance number
* @param[in]     value
*
* @return        void
*
*/
static inline void Ftm_Gpt_Ip_SetCounterInitVal(uint8 instance, uint16 value)
{
    ftmGptBase[instance]->CNTIN = FTM_CNTIN_INIT(value);
}

/*================================================================================================*/
/**
* @brief
* Function Name : Ftm_Gpt_Ip_SetCounter
* Description   : Set the Ftm_Gpt_Ip_SetCounter
*
* @param[in]     instance       FTM hardware instance number
* @param[in]     value
*
* @return        void
*
*/
static inline void Ftm_Gpt_Ip_SetCounter(uint8 instance, uint16 value)
{
    ftmGptBase[instance]->CNT = FTM_CNT_COUNT(value);
}

/*================================================================================================*/
/**
 * @brief
 * Function Name : Ftm_Gpt_Ip_ClearChInterruptStatusFlag
 * Description   : Clear the CHF bit from CSC - Channel (n) Status and Control Register (C0SC - C7SC)
 *
 * @details Channel (n) Flag (CHF)
 *          CHF is cleared by reading the CnSC register while CHF is set and then writing a 0 to the CHF bit. Writing a 1 to CHF has no effect.
 *          If another event occurs between the read and write operations, the write operation has no effect; therefore,
 *          CHF remains set indicating an event has occurred. In this case a CHF interrupt request is not lost due to the
 *          clearing sequence for a previous CHF.
 *
 * @param[in]   instance    FTM hw instance number
 * @param[in]   channel     FTM hw channel number
 *
 * @return      void
 * @pre         The driver needs to be initialized.
 */
static inline void Ftm_Gpt_Ip_ClearChInterruptStatusFlag(uint8 instance, uint8 channel)
{
    ftmGptBase[instance]-> CONTROLS[channel].CSC &= ~(FTM_CSC_CHF_MASK);
}

/*================================================================================================*/
/**
 * @brief
 * Function Name : Ftm_Gpt_Ip_SetChannelEnableInterrupt
 * Description   : Set or Clear the CHIE bit from CSC - Channel (n) Status and Control Register (C0SC - C7SC)
 *
 * @param[in]   instance    FTM hw instance number
 * @param[in]   channel     FTM hw channel number
 * @param[in]   aboEn - Enables the FTM channel(n) interrupt
 *              - True : Enable the FTM channel(n) interrupt.
 *              - False: Disable the FTM channel(n) interrupt.
 */
static inline void Ftm_Gpt_Ip_SetChannelEnableInterrupt(uint8 instance, uint8 channel, boolean aboEn)
{
    ftmGptBase[instance]-> CONTROLS[channel].CSC = (ftmGptBase[instance]-> CONTROLS[channel].CSC & ~(FTM_CSC_CHIE_MASK)) | FTM_CSC_CHIE((TRUE == aboEn) ? 1UL : 0UL);
}

/*================================================================================================*/
/**
 * @brief
 * Function Name : Ftm_Gpt_Ip_SetClockSource
 * Description   : Set CLKS[1:0]:    4-3 bits in Status And Control (SC) register
 *      CLKS: Clock Source Selection
 *          Selects one of the three FTM counter clock sources.
 *          This field is write protected. It can be written only when MODE[WPDIS] = 1.
 *              00b - No clock selected. This in effect disables the FTM counter.
 *              01b - FTM input clock
 *              10b - Fixed frequency clock
 *              11b - External clock
 * @param[in]       instance    FTM hardware instance number
 *
 * @return      void
 */
static inline void Ftm_Gpt_Ip_SetClockSource(uint8 instance, Ftm_Gpt_Ip_ClockSource clocksource)
{
    SchM_Enter_Gpt_GPT_EXCLUSIVE_AREA_21();
    ftmGptBase[instance]->SC = (ftmGptBase[instance]->SC & ~FTM_SC_CLKS_MASK) | FTM_SC_CLKS(clocksource);
    SchM_Exit_Gpt_GPT_EXCLUSIVE_AREA_21();
}

/*================================================================================================*/
/**
 * @brief
 * Function Name : Ftm_Gpt_Ip_SetPrescaler
 * Description   : Set PS[2:0]:    2-0 bits in Status And Control (SC) register
 *      PS: Prescale Factor Selection
 *          Selects one of 8 division factors for the clock source selected by CLKS. The new prescaler factor affects the
 *          clock source on the next FTM input clock cycle after the new value is updated into the register bits.
 *          This field is write protected. It can be written only when MODE[WPDIS] = 1.
 *              000b - Divide by 1
 *              001b - Divide by 2
 *              010b - Divide by 4
 *              011b - Divide by 8
 *              100b - Divide by 16
 *              101b - Divide by 32
 *              110b - Divide by 64
 *              111b - Divide by 128
 * @param[in]       instance    FTM hardware instance number
 * @param[in]       prescalerValue
 *
 * @return        void
 *
 */
static inline void Ftm_Gpt_Ip_SetPrescaler(uint8 instance, uint8 prescalerValue)
{
    SchM_Enter_Gpt_GPT_EXCLUSIVE_AREA_22();
    ftmGptBase[instance]->SC = (ftmGptBase[instance]->SC & ~FTM_SC_PS_MASK) | FTM_SC_PS(prescalerValue);
    SchM_Exit_Gpt_GPT_EXCLUSIVE_AREA_22();
}

/*================================================================================================*/
/**
 * @brief
 * Function Name : Ftm_Gpt_Ip_SetEnFtmModule
 * Description   : Set FTMEN - bit 0 in Features Mode Selection (MODE) register
 *      FTMEN - FTM Enable
 *      This field is write protected. It can be written only when MODE[WPDIS] = 1.
 *          0b - TPM compatibility. Free running counter and synchronization compatible with TPM.
 *          1b - Free running counter and synchronization are different from TPM behavior.
 *
 * @param[in]   instance    FTM hardware instance number
 * @param[in]   enable
 *
 * @return      void
 */
static inline void Ftm_Gpt_Ip_SetEnFtmModule(uint8 instance, boolean enable)
{
    SchM_Enter_Gpt_GPT_EXCLUSIVE_AREA_23();
    {
        ftmGptBase[instance]->MODE &= ~(FTM_MODE_FTMEN_MASK);
        ftmGptBase[instance]->MODE |= FTM_MODE_FTMEN(enable ? 1UL : 0UL);
    }
    SchM_Exit_Gpt_GPT_EXCLUSIVE_AREA_23();
}

/*================================================================================================*/
/**
* @brief
* Function Name : Ftm_Gpt_Ip_SetCompareValue
* Description   : Set compare value (CV)
*
* @param[in]     instance       FTM hardware instance number
* @param[in]     channel
* @param[in]     value
*
* @return        void
*
*/
static inline void Ftm_Gpt_Ip_SetCompareValue(uint8 instance, uint8 channel, uint32 value)
{
    SchM_Enter_Gpt_GPT_EXCLUSIVE_AREA_18();
    ftmGptBase[instance]-> CONTROLS[channel].CV = FTM_CV_VAL(value);
    SchM_Exit_Gpt_GPT_EXCLUSIVE_AREA_18();
}

/*================================================================================================*/
/**
* @brief
* Function Name : Ftm_Gpt_Ip_WriteModulo
* Description   : Set value (MOD)
*
* @param[in]     instance       FTM hardware instance number
* @param[in]     channel
* @param[in]     value
*
* @return        void
*
*/
static inline void Ftm_Gpt_Ip_WriteModulo(uint8 instance, uint16 value)
{
    ftmGptBase[instance]-> MOD = value;

}

/*================================================================================================*/
/**
* @brief
* Function Name : Ftm_Gpt_Ip_ModeSelectA
* Description   : Select mode MSA
*
* @param[in]     instance       FTM hardware instance number
* @param[in]     channel        FTM hardware channel number
* @param[in]     enable
*
* @return        void
*
*/
static inline void Ftm_Gpt_Ip_ModeSelectA(uint8 instance, uint8 channel, boolean enable)
{
    SchM_Enter_Gpt_GPT_EXCLUSIVE_AREA_24();
    if (TRUE == enable)
    {
        ftmGptBase[instance]->CONTROLS[channel].CSC |= FTM_CSC_MSA_MASK;
    }
    else
    {
        ftmGptBase[instance]->CONTROLS[channel].CSC &= (~(FTM_CSC_MSA_MASK));
    }
    SchM_Exit_Gpt_GPT_EXCLUSIVE_AREA_24();
}

/*================================================================================================*/
/**
* @brief
* Function Name : Ftm_Gpt_Ip_SetHalfCycleValue
* Description   : Ftm_Gpt_Ip_SetHalfCycleValue
*
* @param[in]     value
*
* @return        void
*
*/
static inline void Ftm_Gpt_Ip_SetHalfCycleValue(FTM_Type * const base, uint16 value)
{
    ((base)->HCR) = value;
}
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
 * @brief
 * Function Name : Ftm_Gpt_Ip_GetChInterruptEnableFlag
 * Description   : Get the status of CHIE bit from CSC - Channel (n) Status and Control Register (C0SC - C7SC)
 *
 * @details Channel (n) Interrupt Enable flag (CHIE)
 *          Enables channel (n) interrupt.
 *              0b - Disable channel (n) interrupt. Use software polling.
 *              1b - Enable channel (n) interrupt.
 *
 * @param[in]   instance    FTM hw instance number
 * @param[in]   channel     FTM hw channel number
 *
 * @return  TRUE if a channel (n) event has occurred, FALSE otherwise
 * @pre     The driver needs to be initialized.
 */
static inline boolean Ftm_Gpt_Ip_GetChInterruptEnableFlag(uint8 instance, uint8 channel)
{
    return (0U != (ftmGptBase[instance]->CONTROLS[channel].CSC & FTM_CSC_CHIE_MASK)) ? TRUE : FALSE;
}
#endif

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
 * @brief
 * Function Name : Ftm_Gpt_Ip_GetChannelEventStatus
 * Description   : Get the status of CHF bit from STATUS register
 *
 * @details The STATUS register contains a copy of the status flag CHF bit in CnSC for each FTM channel for software convenience.
 *          Each CHF bit in STATUS is a mirror of CHF bit in CnSC. All CHF bits can be checked using only one read of STATUS. All CHF
 *          bits can be cleared by reading STATUS followed by writing 0x00 to STATUS.
 *          Hardware sets the individual channel flags when an event occurs on the channel. CHF is cleared by reading STATUS while CHF
 *          is set and then writing a 0 to the CHF bit. Writing a 1 to CHF has no effect.
 *          If another event occurs between the read and write operations, the write operation has no effect; therefore, CHF remains set
 *          indicating an event has occurred. In this case, a CHF interrupt request is not lost due to the clearing sequence for a previous CHF.
 *
 * @param[in]   instance    FTM hw instance number
 * @param[in]   channel     FTM hw channel number
 *
 * @return Channel event status
 *         - True : A channel event has occurred
 *         - False: No channel event has occurred
 *
 */
static inline boolean Ftm_Gpt_Ip_GetChannelEventStatus(uint8 instance, uint8 channel)
{
    return (0U != (ftmGptBase[instance]->STATUS & (1UL << channel))) ? TRUE : FALSE;
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
/*! @}*/

#endif /* FTM_GPT_IP_HWACCESS_H */
