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

/**
 * @file       Ftfc_Mem_InFls_Ip_Ac.c
 *
 * @addtogroup FTFC_MEM_INFLS_IP
 *
 * implement   Ftfc_Mem_InFls_Ip_Ac.c_Artifact
 * @{
 */

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
==================================================================================================*/
#include "Ftfc_Mem_InFls_Ip.h"
#include "Ftfc_Mem_InFls_Ip_Ac.h"

/*==================================================================================================
 *                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define FTFC_MEM_INFLS_IP_AC_VENDOR_ID_C                      43
#define FTFC_MEM_INFLS_IP_AC_AR_RELEASE_MAJOR_VERSION_C       4
#define FTFC_MEM_INFLS_IP_AC_AR_RELEASE_MINOR_VERSION_C       7
#define FTFC_MEM_INFLS_IP_AC_AR_RELEASE_REVISION_VERSION_C    0
#define FTFC_MEM_INFLS_IP_AC_SW_MAJOR_VERSION_C               3
#define FTFC_MEM_INFLS_IP_AC_SW_MINOR_VERSION_C               0
#define FTFC_MEM_INFLS_IP_AC_SW_PATCH_VERSION_C               0

/*==================================================================================================
 *                                     FILE VERSION CHECKS
==================================================================================================*/
/* Check if current file and Ftfc_Mem_InFls_Ip.h header file are of the same vendor */
#if (FTFC_MEM_INFLS_IP_AC_VENDOR_ID_C != FTFC_MEM_INFLS_IP_VENDOR_ID)
    #error "Fls_Ac.c and Ftfc_Mem_InFls_Ip.h have different vendor ids"
#endif
/* Check if current file and Ftfc_Mem_InFls_Ip.h header file are of the same Autosar version */
#if ((FTFC_MEM_INFLS_IP_AC_AR_RELEASE_MAJOR_VERSION_C    != FTFC_MEM_INFLS_IP_AR_RELEASE_MAJOR_VERSION) || \
     (FTFC_MEM_INFLS_IP_AC_AR_RELEASE_MINOR_VERSION_C    != FTFC_MEM_INFLS_IP_AR_RELEASE_MINOR_VERSION) || \
     (FTFC_MEM_INFLS_IP_AC_AR_RELEASE_REVISION_VERSION_C != FTFC_MEM_INFLS_IP_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of Ftfc_Mem_InFls_Ip_Ac.c and Ftfc_Mem_InFls_Ip.h are different"
#endif
/* Check if current file and Ftfc_Mem_InFls_Ip.h header file are of the same Software version */
#if ((FTFC_MEM_INFLS_IP_AC_SW_MAJOR_VERSION_C != FTFC_MEM_INFLS_IP_SW_MAJOR_VERSION) || \
     (FTFC_MEM_INFLS_IP_AC_SW_MINOR_VERSION_C != FTFC_MEM_INFLS_IP_SW_MINOR_VERSION) || \
     (FTFC_MEM_INFLS_IP_AC_SW_PATCH_VERSION_C != FTFC_MEM_INFLS_IP_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of Ftfc_Mem_InFls_Ip_Ac.c and Ftfc_Mem_InFls_Ip.h are different"
#endif

/* Check if current file and Ftfc_Mem_InFls_Ip_Ac.h header file have the same Vendor ID */
#if (FTFC_MEM_INFLS_IP_AC_VENDOR_ID_C != FTFC_MEM_INFLS_IP_AC_VENDOR_ID)
    #error "Ftfc_Mem_InFls_Ip_Ac.c and Ftfc_Mem_InFls_Ip_Ac.h have different vendor ids"
#endif
/* Check if current file and Ftfc_Mem_InFls_Ip_Ac.h  header file are of the same Autosar version */
#if ((FTFC_MEM_INFLS_IP_AC_AR_RELEASE_MAJOR_VERSION_C    != FTFC_MEM_INFLS_IP_AC_AR_RELEASE_MAJOR_VERSION) || \
     (FTFC_MEM_INFLS_IP_AC_AR_RELEASE_MINOR_VERSION_C    != FTFC_MEM_INFLS_IP_AC_AR_RELEASE_MINOR_VERSION) || \
     (FTFC_MEM_INFLS_IP_AC_AR_RELEASE_REVISION_VERSION_C != FTFC_MEM_INFLS_IP_AC_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of Ftfc_Mem_InFls_Ip_Ac.c and Ftfc_Mem_InFls_Ip_Ac.h are different"
#endif
/* Check if current file and Ftfc_Mem_InFls_Ip_Ac.h  header file are of the same Software version */
#if ((FTFC_MEM_INFLS_IP_AC_SW_MAJOR_VERSION_C != FTFC_MEM_INFLS_IP_AC_SW_MAJOR_VERSION) || \
     (FTFC_MEM_INFLS_IP_AC_SW_MINOR_VERSION_C != FTFC_MEM_INFLS_IP_AC_SW_MINOR_VERSION) || \
     (FTFC_MEM_INFLS_IP_AC_SW_PATCH_VERSION_C != FTFC_MEM_INFLS_IP_AC_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of Ftfc_Mem_InFls_Ip_Ac.c and Ftfc_Mem_InFls_Ip_Ac.h are different"
#endif

/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
                                      GLOBAL FUNCTIONS
==================================================================================================*/
#define MEM_43_INFLS_START_SEC_CODE_AC
#include "Mem_43_INFLS_MemMap.h"

/* Position independent erase access code */
void Ftfc_Mem_InFls_Ip_AccessCode(void (*CallBack)(void))
{
#if (FTFC_TIMEOUT_SUPERVISION_ENABLED == STD_ON)
    uint32 u32ValueTimeOut = Ftfc_Mem_InFls_Ip_u32TimeoutTicks;
    uint32 u32WaitedTicks;
#endif

    /* Start internal erase/program sequence */
    FTFx_BaseAddress->FSTAT = FTFx_FSTAT_CCIF_MASK;
    /* wait until operation finishes or write/erase timeout is reached */
    while (0U == ((FTFx_BaseAddress->FSTAT) & FTFx_FSTAT_CCIF_MASK))
    {
#if (FTFC_TIMEOUT_SUPERVISION_ENABLED == STD_ON)
        u32ValueTimeOut--;
        if (0U == u32ValueTimeOut)
        {
            break;
        }
#endif
        if (NULL_PTR != CallBack)
        {
            CallBack();
        }
    }

#if (FTFC_TIMEOUT_SUPERVISION_ENABLED == STD_ON)
    /* Update the timeout counter */
    u32WaitedTicks = Ftfc_Mem_InFls_Ip_u32TimeoutTicks - u32ValueTimeOut;
    Ftfc_Mem_InFls_Ip_u32ElapsedTicks += u32WaitedTicks;
    Ftfc_Mem_InFls_Ip_u32CurrentTicks += u32WaitedTicks;
#endif
}

#define MEM_43_INFLS_STOP_SEC_CODE_AC
#include "Mem_43_INFLS_MemMap.h"

#ifdef __cplusplus
}
#endif

/** @} */
