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
 * @file       Ftfc_Mem_InFls_Ip.c
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
#include "Ftfc_Mem_InFls_Ip.h"
#include "Ftfc_Mem_InFls_Ip_Ac.h"

/* Include file for Exclusive area */
#include "SchM_Mem_43_INFLS.h"

#if (STD_ON == FTFC_MEM_INFLS_IP_SYNCHRONIZE_CACHE)
#include "Cache_Ip.h"
#endif /* FTFC_MEM_INFLS_IP_SYNCHRONIZE_CACHE */

#if (STD_ON == FTFC_MEM_INFLS_IP_SUPPORT_USER_MODE)
#define USER_MODE_REG_PROT_ENABLED      STD_ON
#include "RegLockMacros.h"
#endif /* #if (STD_ON == FTFC_MEM_INFLS_IP_SUPPORT_USER_MODE) */

#if (STD_ON == FTFC_TIMEOUT_SUPERVISION_ENABLED)
#include "OsIf.h"
#endif

#include "Mcal.h"

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define FTFC_MEM_INFLS_IP_VENDOR_ID_C                    43
#define FTFC_MEM_INFLS_IP_AR_RELEASE_MAJOR_VERSION_C     4
#define FTFC_MEM_INFLS_IP_AR_RELEASE_MINOR_VERSION_C     7
#define FTFC_MEM_INFLS_IP_AR_RELEASE_REVISION_VERSION_C  0
#define FTFC_MEM_INFLS_IP_SW_MAJOR_VERSION_C             3
#define FTFC_MEM_INFLS_IP_SW_MINOR_VERSION_C             0
#define FTFC_MEM_INFLS_IP_SW_PATCH_VERSION_C             0

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/

/* Check if current file and Ftfc_Mem_InFls_Ip.h header file are of the same vendor */
#if (FTFC_MEM_INFLS_IP_VENDOR_ID_C != FTFC_MEM_INFLS_IP_VENDOR_ID)
    #error "Ftfc_Mem_InFls_Ip.c and Ftfc_Mem_InFls_Ip.h have different vendor ids"
#endif
/* Check if current file and Ftfc_Mem_InFls_Ip.h header file are of the same Autosar version */
#if ((FTFC_MEM_INFLS_IP_AR_RELEASE_MAJOR_VERSION_C    != FTFC_MEM_INFLS_IP_AR_RELEASE_MAJOR_VERSION) || \
     (FTFC_MEM_INFLS_IP_AR_RELEASE_MINOR_VERSION_C    != FTFC_MEM_INFLS_IP_AR_RELEASE_MINOR_VERSION) || \
     (FTFC_MEM_INFLS_IP_AR_RELEASE_REVISION_VERSION_C != FTFC_MEM_INFLS_IP_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of Ftfc_Mem_InFls_Ip.c and Ftfc_Mem_InFls_Ip.h are different"
#endif
/* Check if current file and Ftfc_Mem_InFls_Ip.h header file are of the same Software version */
#if ((FTFC_MEM_INFLS_IP_SW_MAJOR_VERSION_C != FTFC_MEM_INFLS_IP_SW_MAJOR_VERSION) || \
     (FTFC_MEM_INFLS_IP_SW_MINOR_VERSION_C != FTFC_MEM_INFLS_IP_SW_MINOR_VERSION) || \
     (FTFC_MEM_INFLS_IP_SW_PATCH_VERSION_C != FTFC_MEM_INFLS_IP_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of Ftfc_Mem_InFls_Ip.c and Ftfc_Mem_InFls_Ip.h are different"
#endif

/* Check if current file and Ftfc_Mem_InFls_Ip_Ac.h header file are of the same vendor */
#if (FTFC_MEM_INFLS_IP_VENDOR_ID_C != FTFC_MEM_INFLS_IP_AC_VENDOR_ID)
    #error "Ftfc_Mem_InFls_Ip.c and Ftfc_Mem_InFls_Ip_Ac.h have different vendor ids"
#endif
/* Check if current file and Ftfc_Mem_InFls_Ip_Ac.h header file are of the same Autosar version */
#if ((FTFC_MEM_INFLS_IP_AR_RELEASE_MAJOR_VERSION_C    != FTFC_MEM_INFLS_IP_AC_AR_RELEASE_MAJOR_VERSION) || \
     (FTFC_MEM_INFLS_IP_AR_RELEASE_MINOR_VERSION_C    != FTFC_MEM_INFLS_IP_AC_AR_RELEASE_MINOR_VERSION) || \
     (FTFC_MEM_INFLS_IP_AR_RELEASE_REVISION_VERSION_C != FTFC_MEM_INFLS_IP_AC_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of Ftfc_Mem_InFls_Ip.c and Ftfc_Mem_InFls_Ip_Ac.h are different"
#endif
/* Check if current file and Ftfc_Mem_InFls_Ip_Ac.h header file are of the same Software version */
#if ((FTFC_MEM_INFLS_IP_SW_MAJOR_VERSION_C != FTFC_MEM_INFLS_IP_AC_SW_MAJOR_VERSION) || \
     (FTFC_MEM_INFLS_IP_SW_MINOR_VERSION_C != FTFC_MEM_INFLS_IP_AC_SW_MINOR_VERSION) || \
     (FTFC_MEM_INFLS_IP_SW_PATCH_VERSION_C != FTFC_MEM_INFLS_IP_AC_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of Ftfc_Mem_InFls_Ip.c and Ftfc_Mem_InFls_Ip_Ac.h are different"
#endif

#ifndef DISABLE_MCAL_INTERMODULE_ASR_CHECK
    #if (STD_ON == FTFC_MEM_INFLS_IP_SYNCHRONIZE_CACHE)
        /* Check if current file and Cache_Ip.h header file are of the same Autosar version */
        #if ((FTFC_MEM_INFLS_IP_AR_RELEASE_MAJOR_VERSION_C != CACHE_IP_AR_RELEASE_MAJOR_VERSION) || \
             (FTFC_MEM_INFLS_IP_AR_RELEASE_MINOR_VERSION_C != CACHE_IP_AR_RELEASE_MINOR_VERSION) \
            )
            #error "Autosar Version Numbers of Ftfc_Mem_InFls_Ip.c and Cache_Ip.h are different"
        #endif
    #endif /* STD_ON == FTFC_MEM_INFLS_IP_SYNCHRONIZE_CACHE */

    #if (STD_ON == FTFC_TIMEOUT_SUPERVISION_ENABLED)
        /* Check if current file and OsIf.h header file are of the same Autosar version */
        #if ((FTFC_MEM_INFLS_IP_AR_RELEASE_MAJOR_VERSION_C != OSIF_AR_RELEASE_MAJOR_VERSION) || \
             (FTFC_MEM_INFLS_IP_AR_RELEASE_MINOR_VERSION_C != OSIF_AR_RELEASE_MINOR_VERSION) \
            )
            #error "AutoSar Version Numbers of Ftfc_Mem_InFls_Ip.c and OsIf.h are different"
        #endif
    #endif /* STD_ON == FTFC_TIMEOUT_SUPERVISION_ENABLED */

    #if (STD_ON == FTFC_MEM_INFLS_IP_SUPPORT_USER_MODE)
        /* Check if current file and RegLockMacros.h header file are of the same Autosar version */
        #if ((FTFC_MEM_INFLS_IP_AR_RELEASE_MAJOR_VERSION_C != REGLOCKMACROS_AR_RELEASE_MAJOR_VERSION) || \
             (FTFC_MEM_INFLS_IP_AR_RELEASE_MINOR_VERSION_C != REGLOCKMACROS_AR_RELEASE_MINOR_VERSION) \
            )
            #error "Autosar Version Numbers of Ftfc_Mem_InFls_Ip.c and RegLockMacros.h are different"
        #endif
    #endif /* STD_ON == FTFC_MEM_INFLS_IP_SUPPORT_USER_MODE */

    /* Check if current file and Mcal.h header file are of the same Autosar version */
    #if ((FTFC_MEM_INFLS_IP_AR_RELEASE_MAJOR_VERSION_C != MCAL_AR_RELEASE_MAJOR_VERSION) || \
         (FTFC_MEM_INFLS_IP_AR_RELEASE_MINOR_VERSION_C != MCAL_AR_RELEASE_MINOR_VERSION) \
        )
        #error "Autosar Version Numbers of Ftfc_Mem_InFls_Ip.c and Mcal.h are different"
    #endif
#endif /* DISABLE_MCAL_INTERMODULE_ASR_CHECK */


/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
#if (FTFC_MEM_INFLS_IP_INVALID_PREBUF_FROM_RAM == STD_ON)
#define MEM_43_INFLS_START_SEC_RAMCODE
#else
#define MEM_43_INFLS_START_SEC_CODE
#endif
#include "Mem_43_INFLS_MemMap.h"

/* The Ftfc_Mem_InFls_Ip_InvalidPrefetchBuff_Ram function should be placed and executed from RAM,
   because it accesses the flash prefetch buffers, which should not be modified while reading code instructions.
   In practice, this function still works normally from FLASH because it has already loaded to Cache and executed from there.
   However, on platforms that do not support cache or cache is disabled, this functions must be placed on RAM.
   For more information, please see the chapter Tips for Mem Internal integration in the Integration Manual.
 */
void Ftfc_Mem_InFls_Ip_InvalidPrefetchBuff_Ram(void);

#if (FTFC_MEM_INFLS_IP_INVALID_PREBUF_FROM_RAM == STD_ON)
#define MEM_43_INFLS_STOP_SEC_RAMCODE
#else
#define MEM_43_INFLS_STOP_SEC_CODE
#endif
#include "Mem_43_INFLS_MemMap.h"


#define MEM_43_INFLS_START_SEC_CODE
#include "Mem_43_INFLS_MemMap.h"

static void Ftfc_Mem_InFls_Ip_FlashAccessCalloutStart(void);
static void Ftfc_Mem_InFls_Ip_FlashAccessCalloutFinish(void);

static void Ftfc_Mem_InFls_Ip_InvalidPrefetchBuff(void);
static void Ftfc_Mem_InFls_Ip_CalculateDFlashBitSize(void);

static boolean Ftfc_Mem_InFls_Ip_CheckValidRange(uint32 startAddress, uint32 length);
static uint32 Ftfc_ComputeReadSize(uint32 srcAddress, uint32 desAddress, uint32 byteRemain);
static uint32 Ftfc_Mem_InFls_Ip_ConvertSysToFTFEAddr(uint32 u32SystemAddress);


static Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_SectorErasePreCheck(uint32 u32SectorStartAddress);
static Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_WritePreCheck(uint32 u32DestAddress, const uint8 *pSourceAddressPtr, uint32 u32Length);
static Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_ReadPreCheck(uint32 u32SrcAddress, const uint8 *pDestAddressPtr, uint32 u32Length);
static Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_ReadStatus(boolean compareStatus);
static Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_ComparePreCheck(uint32 u32SrcAddress, uint32 u32Length);

static Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_WaitForOperationFinish(void);
static Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_CheckFProtReg(uint32 u32SectStartAddr);
static Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_ControllerBusy(void);

static inline uint8  Ftfc_Mem_InFls_Ip_ReadData8(uint32 address);
static inline uint16 Ftfc_Mem_InFls_Ip_ReadData16(uint32 address);
static inline uint32 Ftfc_Mem_InFls_Ip_ReadData32(uint32 address);
static void Ftfc_Mem_InFls_Ip_ReadData(uint32 readSize,
                                       uint32 readAddress,
                                       uint32 desAddress,
                                       uint8 *desAddressPtr
                                      );

static boolean Ftfc_Mem_InFls_Ip_EraseVerifyData(uint32 readSize, uint32 srcAddress);
static boolean Ftfc_Mem_InFls_Ip_ProgramVerifyData(uint32 readSize, uint32 srcAddress, uint32 desAddress);
static void Ftfc_Mem_InFls_Ip_ProgramVerify(uint32 address, const uint8 * data, uint32 size);
static void Ftfc_Mem_InFls_Ip_EraseVerify(uint32 address, uint32 size);

static void Ftfc_Mem_InFls_Ip_ClearErrorFlags(void);
static void Ftfc_Mem_InFls_Ip_LoadFCCOBParams(const uint32 u32PhysicAddr,
                                              const uint8 *pDataAddr,
                                              const uint8  u8FCCOBCmdId
                                             );


#if (STD_ON == FTFC_MEM_INFLS_IP_SYNCHRONIZE_CACHE)
static void Ftfc_Mem_InFls_Ip_SynchronizeCache(uint32 address, uint32 length);
#endif

#define MEM_43_INFLS_STOP_SEC_CODE
#include "Mem_43_INFLS_MemMap.h"


/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/
#define MEM_43_INFLS_START_SEC_VAR_CLEARED_32
#include "Mem_43_INFLS_MemMap.h"

/* The amount of DFlash size which is covered by a bit in the Data Flash Protection Register (FDPROT),
 * which is the total amount of DFlash divided by 8.
 */
static uint32 Ftfc_Mem_InFls_Ip_u32DFlashBitSizeProt;

#if (STD_ON == FTFC_TIMEOUT_SUPERVISION_ENABLED)
uint32 Ftfc_Mem_InFls_Ip_u32ElapsedTicks;
uint32 Ftfc_Mem_InFls_Ip_u32TimeoutTicks;
uint32 Ftfc_Mem_InFls_Ip_u32CurrentTicks;
#endif

#if ((STD_ON == FTFC_MEM_INFLS_IP_PROGRAM_VERIFICATION) || (STD_ON == FTFC_MEM_INFLS_IP_SYNCHRONIZE_CACHE))
static uint32 Ftfc_Mem_InFls_Ip_u32ProgrammedAddress;
static uint32 Ftfc_Mem_InFls_Ip_u32ProgrammedLength;
#endif
#if (STD_ON == FTFC_MEM_INFLS_IP_PROGRAM_VERIFICATION)
static const uint8 *Ftfc_Mem_InFls_Ip_pProgrammedData;
#endif

#if ((STD_ON == FTFC_MEM_INFLS_IP_ERASE_VERIFICATION) || (STD_ON == FTFC_MEM_INFLS_IP_SYNCHRONIZE_CACHE))
static uint32 Ftfc_Mem_InFls_Ip_u32ErasedSectorAddress;
#endif

#if ((FTFC_MEM_INFLS_IP_ECC_CHECK == STD_ON) || (FTFC_MEM_INFLS_IP_ECC_CHECK_BY_AUTOSAR_OS == STD_ON))
#if (FTFC_MEM_INFLS_IP_ECC_SYNDROME_AND_DATA_ADDRESS_CHECK == STD_ON)
/* Save the most recent memory access address for ECC handling */
static volatile uint32 Ftfc_Mem_InFls_Ip_u32ReadAddressPtr;
#endif
#endif

#define MEM_43_INFLS_STOP_SEC_VAR_CLEARED_32
#include "Mem_43_INFLS_MemMap.h"


#define MEM_43_INFLS_START_SEC_VAR_INIT_UNSPECIFIED
#include "Mem_43_INFLS_MemMap.h"

#if ((FTFC_MEM_INFLS_IP_ECC_CHECK == STD_ON) || (FTFC_MEM_INFLS_IP_ECC_CHECK_BY_AUTOSAR_OS == STD_ON))
    /* Save the current read operation status for ECC handling */
    static volatile Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_eReadStatus = STATUS_FTFC_MEM_INFLS_IP_ERROR;
#else
    static Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_eReadStatus = STATUS_FTFC_MEM_INFLS_IP_ERROR;
#endif

static Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_eWriteStatus = STATUS_FTFC_MEM_INFLS_IP_ERROR;
static Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_eEraseStatus = STATUS_FTFC_MEM_INFLS_IP_ERROR;

#define MEM_43_INFLS_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "Mem_43_INFLS_MemMap.h"


#define MEM_43_INFLS_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Mem_43_INFLS_MemMap.h"
/**
 * @brief Pointer to current flash module configuration set
 */
static const Ftfc_Mem_InFls_Ip_ConfigType * Ftfc_Mem_InFls_Ip_pConfigPtr;

#define MEM_43_INFLS_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Mem_43_INFLS_MemMap.h"


#define MEM_43_INFLS_START_SEC_VAR_INIT_BOOLEAN
#include "Mem_43_INFLS_MemMap.h"

static boolean Ftfc_Mem_InFls_Ip_bAsync = TRUE;

#define MEM_43_INFLS_STOP_SEC_VAR_INIT_BOOLEAN
#include "Mem_43_INFLS_MemMap.h"


#define MEM_43_INFLS_START_SEC_CONST_UNSPECIFIED
#include "Mem_43_INFLS_MemMap.h"

/* Base address for Flash Memory Module */
FTFx_HARDWARE_TYPE * const FTFx_BaseAddress = FTFx_HARDWARE_UNIT;

#define MEM_43_INFLS_STOP_SEC_CONST_UNSPECIFIED
#include "Mem_43_INFLS_MemMap.h"

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/
#define MEM_43_INFLS_START_SEC_CODE
#include "Mem_43_INFLS_MemMap.h"

/**
 * Function Name : Ftfc_Mem_InFls_Ip_CalculateDFlashBitSize
 * Description   : Calculates the region size protected by each DPROT bit.
 *                  The total amount of DFlash size is determined based on the DEPART value, which cannot change in runtime
 */
static void Ftfc_Mem_InFls_Ip_CalculateDFlashBitSize(void)
{
    uint32 u32DFlashSize;
    uint32 u32RegSimFcfg1;
    uint32 u32FlexNvmPartSize;

    /* Determine the DFlash size. */
    u32RegSimFcfg1 = IP_SIM->FCFG1;
    /* Get the amount of flash allocated to FlexNVM/EEPROM partition, which has to be subtracted from the current Dflash size. */
    u32FlexNvmPartSize = (uint32)((u32RegSimFcfg1 & SIM_FCFG1_DEPART_MASK) >> SIM_FCFG1_DEPART_SHIFT);

    /* Special partition codes for 2048K device. */
#if (0x80000UL == FTFC_MEM_INFLS_IP_D_FLASH_SIZE)
/* 512K devices. */
    if ((FLASH_FLEXNVM_DFLASH_EEPROM_512_0_V1 == u32FlexNvmPartSize) ||
         (FLASH_FLEXNVM_DFLASH_EEPROM_DEFAULT  == u32FlexNvmPartSize)
       )
    {
         /* FlexNvm configured for all Data Flash (no EEPROM partitioned) */
        u32DFlashSize = FTFC_MEM_INFLS_IP_D_FLASH_SIZE;   /*0x80000, 512K*/
    }
    else if (FLASH_FLEXNVM_DFLASH_EEPROM_448_64_V1 == u32FlexNvmPartSize)
    {
         /* FlexNvm configured for all Data Flash (no EEPROM partitioned) */
        u32DFlashSize = 0x70000UL;  /* 448K */
    }
    else
    {
        u32DFlashSize = 0U;     /* Error case, should not be taken. */
    }
#elif (0x10000UL == FTFC_MEM_INFLS_IP_D_FLASH_SIZE)
/* 64K devices. */
    if ((FLASH_FLEXNVM_DFLASH_EEPROM_64_0_V1 == u32FlexNvmPartSize) ||
         (FLASH_FLEXNVM_DFLASH_EEPROM_64_0_V2 == u32FlexNvmPartSize) ||
         (FLASH_FLEXNVM_DFLASH_EEPROM_DEFAULT == u32FlexNvmPartSize)
       )
    {
        /* FlexNvm configured for all Data Flash (no EEPROM partitioned) */
        u32DFlashSize = FTFC_MEM_INFLS_IP_D_FLASH_SIZE;
    }
    else if ((FLASH_FLEXNVM_DFLASH_EEPROM_32_32_V1 == u32FlexNvmPartSize) ||
              (FLASH_FLEXNVM_DFLASH_EEPROM_32_32_V2 == u32FlexNvmPartSize)
            )
    {
         /* FlexNvm configured for all Data Flash (no EEPROM partitioned) */
        u32DFlashSize = 0x8000UL;  /* 32K */
    }
    else if (FLASH_FLEXNVM_DFLASH_EEPROM_16_48_V1 == u32FlexNvmPartSize)
    {
         /* FlexNvm configured for all Data Flash (no EEPROM partitioned) */
        u32DFlashSize = 0x4000UL;  /* 16K */
    }
    else
    {
        /*FLASH_FLEXNVM_DFLASH_EEPROM_0_64_V1*/
        /*FLASH_FLEXNVM_DFLASH_EEPROM_0_64_V2*/
        u32DFlashSize = 0U;     /* No data flash configured. */
    }
#elif (0x8000UL == FTFC_MEM_INFLS_IP_D_FLASH_SIZE)
/* 32K devices. */
    if ((FLASH_FLEXNVM_DFLASH_EEPROM_32_0_V1 == u32FlexNvmPartSize) ||
         (FLASH_FLEXNVM_DFLASH_EEPROM_32_0_V2 == u32FlexNvmPartSize) ||
         (FLASH_FLEXNVM_DFLASH_EEPROM_DEFAULT == u32FlexNvmPartSize)
       )
    {
        /* FlexNvm configured for all Data Flash (no EEPROM partitioned) */
        u32DFlashSize = FTFC_MEM_INFLS_IP_D_FLASH_SIZE;
    }

    else if (FLASH_FLEXNVM_DFLASH_EEPROM_8_24_V1 == u32FlexNvmPartSize)
    {
         /* FlexNvm configured 8K Data Flash (24K EEPROM partitioned) */
        u32DFlashSize = 0x2000UL;  /* 8K */
    }
    else
    {
        /*FLASH_FLEXNVM_DFLASH_EEPROM_0_32_V1*/
        /*FLASH_FLEXNVM_DFLASH_EEPROM_0_32_V2*/
        u32DFlashSize = 0U;     /* No data flash configured. */
    }
#endif

    /* Each bit in the FDPROT register protects an 8th of the available DFlash. */
    Ftfc_Mem_InFls_Ip_u32DFlashBitSizeProt = u32DFlashSize >> 3UL;
}


/**
 * Function Name : Ftfc_Mem_InFls_Ip_CheckFProtReg
 * Description   : Checks if the input address is protected or not, before executing the write or erase operation.
 */
static Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_CheckFProtReg(uint32 u32SectStartAddr)
{
    uint32 u32PFlashBitSize;     /* The amount of Pflash size which is covered by a bit in a protection register, which is the total amount of Pflash divided by 32. */
    uint32 u32PFlashRegSize;     /* The amount of Pflash size which is covered by a protection register, which is the total amount of Pflash divided by 4. */
    uint8  u8SecRegProtBitPos;
    uint8  u8RegProtNo;
    Ftfc_Mem_InFls_Ip_StatusType eRetVal = STATUS_FTFC_MEM_INFLS_IP_SUCCESS;

    /* Determine if it is a Pflash or Dflash data sector. */
    if (u32SectStartAddr >= (FTFC_MEM_INFLS_IP_P_FLASH_BASE_ADDR + FTFC_MEM_INFLS_IP_P_FLASH_SIZE))                /* if NOT a P-FLASH addr... */
    {
        if ((u32SectStartAddr < FTFC_MEM_INFLS_IP_D_FLASH_BASE_ADDR) ||
             (u32SectStartAddr >= (FTFC_MEM_INFLS_IP_D_FLASH_BASE_ADDR + FTFC_MEM_INFLS_IP_D_FLASH_SIZE))           /* ...and also NOT a D-FLASH addr... */
           )
        {
            /* invalid flash address */
            eRetVal = STATUS_FTFC_MEM_INFLS_IP_ERROR_INPUT_PARAM;
        }
        else
        {
            /* DATA flash sector */
            if (0U != Ftfc_Mem_InFls_Ip_u32DFlashBitSizeProt)
            {
                /* A prot register has 8 bits, determine the bit which the current address resides in */
                u8SecRegProtBitPos = (uint8)((u32SectStartAddr - FTFC_MEM_INFLS_IP_D_FLASH_BASE_ADDR) / Ftfc_Mem_InFls_Ip_u32DFlashBitSizeProt);
                if (0U == (FTFx_BaseAddress->FDPROT & (1UL << u8SecRegProtBitPos)))
                {
                    /* Data Flash region is protected */
                    eRetVal = STATUS_FTFC_MEM_INFLS_IP_ERROR;
                }
            }
            else
            {
                /* No Dflash configured, although this sector is supposed to be a DFlash one. */
                eRetVal = STATUS_FTFC_MEM_INFLS_IP_ERROR;
            }
        }
    }
    else
    {
        /* PROGRAM flash sector */

        /* Determine the Pflash size which is protected by a register. */
#if (0x180000UL == FTFC_MEM_INFLS_IP_P_FLASH_SIZE) /* 2048K device */
        /* For 2048K device, the PFlash size, which is 1.5K has to be rounded to 2K. */
        u32PFlashRegSize = 0x200000UL >> 2UL;
#else
        u32PFlashRegSize = FTFC_MEM_INFLS_IP_P_FLASH_SIZE >> 2UL;
#endif

        /* Compute the protection region size depending on the P-FLASH size. */
#if (0x20000UL == FTFC_MEM_INFLS_IP_P_FLASH_SIZE)
        u32PFlashBitSize = 4096UL;      /* Each bit protects 4KB regions */
#endif
#if (0x40000UL == FTFC_MEM_INFLS_IP_P_FLASH_SIZE)
        u32PFlashBitSize = 8192UL;      /* Each bit protects 8KB regions */
#endif
#if (0x80000UL == FTFC_MEM_INFLS_IP_P_FLASH_SIZE)
        u32PFlashBitSize = 16384UL;     /* Each bit protects 16KB regions */
#endif
#if (0x100000UL == FTFC_MEM_INFLS_IP_P_FLASH_SIZE)
        u32PFlashBitSize = 32768UL;     /* Each bit protects 32KB regions */
#endif
#if (0x180000UL == FTFC_MEM_INFLS_IP_P_FLASH_SIZE)
        u32PFlashBitSize = 65536UL;     /* Each bit protects 64KB regions */
#endif

        /* There are four prot registers for PROGRAM flash sectors, the right one has to be determined. Find in which reg prot register this sector belongs. */
        u8RegProtNo = (uint8)(u32SectStartAddr / u32PFlashRegSize);  /* Determine in which of the four protection regions the current address resides. */

        /* A prot register has 8 bits, determine the bit which the current address resides in */
        u8SecRegProtBitPos = (uint8)((u32SectStartAddr % u32PFlashRegSize) / u32PFlashBitSize);

        if (u8RegProtNo < FTFx_FPROT_COUNT)
        {
            /* Check if the sector is protected */
            if (0U == (FTFx_BaseAddress->FPROT[u8RegProtNo] & (1UL << u8SecRegProtBitPos)))
            {
                /* Program Flash region is protected */
                eRetVal = STATUS_FTFC_MEM_INFLS_IP_ERROR;
            }
        }
        else
        {
            /* Do nothing */
        }
    }
    return eRetVal;
}


/**
 * Function Name : Ftfc_Mem_InFls_Ip_Init
 * Description      This function will initialize Ftfc module and clear all error flags.
 * @implements      Ftfc_Mem_InFls_Ip_Init_Activity
 */
Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_Init(const Ftfc_Mem_InFls_Ip_ConfigType * Ftfc_Mem_InFls_Ip_pInitConfig)
{
#if (FTFC_MEM_INFLS_IP_DEV_ERROR_DETECT == STD_ON)
    FTFC_MEM_INFLS_IP_DEV_ASSERT(Ftfc_Mem_InFls_Ip_pInitConfig != NULL_PTR);
#endif

    Ftfc_Mem_InFls_Ip_pConfigPtr = Ftfc_Mem_InFls_Ip_pInitConfig;

    /* Precalculate the DFlash region protect size each bit to increase performance in runtime */
    Ftfc_Mem_InFls_Ip_CalculateDFlashBitSize();

    return STATUS_FTFC_MEM_INFLS_IP_SUCCESS;
}

/**
 * Function Name : Ftfc_Mem_InFls_Ip_Abort
 * Description      Abort a program or erase operation
 * @implements      Ftfc_Mem_InFls_Ip_Abort_Activity
 */
Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_Abort(void)
{
    Ftfc_Mem_InFls_Ip_StatusType eRetVal;

    /* FTFX does not support abort a program or erase operation by hardware.
        So The driver will be replaced by software polling until all work is completed. */
    if (STATUS_FTFC_MEM_INFLS_IP_SUCCESS == Ftfc_Mem_InFls_Ip_WaitForOperationFinish())
    {
        /* memory idle, no operation is pending */
        eRetVal = STATUS_FTFC_MEM_INFLS_IP_SUCCESS;
    }
    else
    {
        eRetVal = STATUS_FTFC_MEM_INFLS_IP_ERROR;
    }
    return eRetVal;
}


static Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_WaitForOperationFinish(void)
{
    #ifdef MCAL_ENABLE_FAULT_INJECTION
    /* Note: (For testing purpose only)
       A workaround to avoid duplication of fault injection label in inline assembly statement below.
       The volatile keyword helps to increase the code size, thus prevents the compiler from inlining this function.
     */
    volatile Ftfc_Mem_InFls_Ip_StatusType eRetVal;
    #else
    Ftfc_Mem_InFls_Ip_StatusType eRetVal;
    #endif

    #if (FTFC_TIMEOUT_SUPERVISION_ENABLED == STD_ON)
    uint32 u32ElapsedAbortTicks = 0UL;
    uint32 u32TimeoutAbortTicks = OsIf_MicrosToTicks(FTFC_MEM_INFLS_IP_ABORT_SUSPEND_TIMEOUT, (OsIf_CounterType)FTFC_MEM_INFLS_IP_TIMEOUT_TYPE);
    uint32 u32CurrentAbortTicks = OsIf_GetCounter((OsIf_CounterType)FTFC_MEM_INFLS_IP_TIMEOUT_TYPE);

    /* wait until done */
    while ((0U == ((FTFx_BaseAddress->FSTAT) & FTFx_FSTAT_CCIF_MASK)) && (u32ElapsedAbortTicks < u32TimeoutAbortTicks))
    {
        u32ElapsedAbortTicks += OsIf_GetElapsed(&u32CurrentAbortTicks, (OsIf_CounterType)FTFC_MEM_INFLS_IP_TIMEOUT_TYPE);
    }
    #else
    /* wait until done */
    while (0U == ((FTFx_BaseAddress->FSTAT) & FTFx_FSTAT_CCIF_MASK))
    {

    }
    #endif /* FTFC_TIMEOUT_SUPERVISION_ENABLED == STD_ON */

    MCAL_FAULT_INJECTION_POINT(FLS_FIP_FR_ERROR_HARDWARE_BUSY_IN_ABORT);
    /* return true if DONE */
    if (0U != ((FTFx_BaseAddress->FSTAT) & FTFx_FSTAT_CCIF_MASK))
    {
        /* OK, memory idle */
        eRetVal = STATUS_FTFC_MEM_INFLS_IP_SUCCESS;
    }
    else
    {
        /* error, memory controller not idle */
        eRetVal = STATUS_FTFC_MEM_INFLS_IP_ERROR;
    }

    return eRetVal;
}


#if (STD_ON == FTFC_MEM_INFLS_IP_SYNCHRONIZE_CACHE)
static void Ftfc_Mem_InFls_Ip_SynchronizeCache(uint32 address, uint32 length)
{
    #if (STD_ON == FTFC_FLS_D_FLASH_CACHEABLE)
        /* Both code and data flash are cacheable, call the invalidate cache function directly */
        (void)Cache_Ip_InvalidateByAddr(CACHE_IP_LMEM, CACHE_IP_PC_BUS, address, length);
    #else
        /* The data flash is non-cacheable, skip it, only invalidate cache for program flash */
        if ((boolean)TRUE == (boolean)FTFC_MEM_INFLS_IP_ADDRESS_VALID_P_FLASH(address))
        {
            Cache_Ip_InvalidateByAddr(CACHE_IP_LMEM, CACHE_IP_PC_BUS, address, length);
        }
    #endif
}
#endif

/**
 * @brief Set up then run new command operation
 */
static void Ftfc_Mem_InFls_Ip_NewCmdOperation(const uint8 u8FCCOBCmdId,
                                              uint32 u32DestAddress,
                                              const uint8 *pSourceAddressPtr
                                          #if (STD_ON == FTFC_TIMEOUT_SUPERVISION_ENABLED)
                                              , uint32 TimeOutValue
                                          #endif
                                             )
{
    /* Setup command for operation */
    Ftfc_Mem_InFls_Ip_LoadFCCOBParams(u32DestAddress, pSourceAddressPtr, u8FCCOBCmdId);

    MCAL_FAULT_INJECTION_POINT(FLS_FIP_FR_ERROR_WRITE_PREPARED_DATA_TO_SENT);

    /* Clear all error flags, otherwise CCIF cannot be cleared to launch command */
    Ftfc_Mem_InFls_Ip_ClearErrorFlags();

    if (TRUE == Ftfc_Mem_InFls_Ip_bAsync)
    {
        /* Start flash access */
        Ftfc_Mem_InFls_Ip_FlashAccessCalloutStart();

        /* Start internal erase/program sequence */
        FTFx_BaseAddress->FSTAT = FTFx_FSTAT_CCIF_MASK;
    }

#if (STD_ON == FTFC_TIMEOUT_SUPERVISION_ENABLED)
    /* Prepare timeout counter */
    Ftfc_Mem_InFls_Ip_u32ElapsedTicks = 0UL;
    Ftfc_Mem_InFls_Ip_u32CurrentTicks = OsIf_GetCounter((OsIf_CounterType)FTFC_MEM_INFLS_IP_TIMEOUT_TYPE);
    Ftfc_Mem_InFls_Ip_u32TimeoutTicks = OsIf_MicrosToTicks(TimeOutValue, (OsIf_CounterType)FTFC_MEM_INFLS_IP_TIMEOUT_TYPE);
#endif
}


/**
 * @brief Compute the data size, considering the addresses alignment and the remaining bytes.
 *
 * @param[in]  srcAddress the internal flash address
 * @param[in]  desAddress the user's buffer address
 * @param[in]  byteRemain how many bytes are left to process
 *
 * @return the data size that flash memory will accept given the address/buffer alignments
 * @retval FTFC_MEM_INFLS_IP_SIZE_1BYTE     unaligned operations
 * @retval FTFC_MEM_INFLS_IP_SIZE_2BYTE     16-bit aligned operations
 * @retval FTFC_MEM_INFLS_IP_SIZE_4BYTE     32-bit aligned operations
 */
static uint32 Ftfc_ComputeReadSize(uint32 srcAddress,
                                   uint32 desAddress,
                                   uint32 byteRemain
                                  )
{
    uint32 readSize = FTFC_MEM_INFLS_IP_SIZE_1BYTE;
    /* Combine two addresses for faster alignment checking */
    uint32 combinedAddress = srcAddress | desAddress;

    /* Both the lowest two bits are zero: 4 bytes aligned */
    if (0UL == (combinedAddress & 0x03UL))
    {
        if (byteRemain >= FTFC_MEM_INFLS_IP_SIZE_4BYTE)
        {
            /* 4 bytes operation */
            readSize = FTFC_MEM_INFLS_IP_SIZE_4BYTE;
        }
        else if (byteRemain >= FTFC_MEM_INFLS_IP_SIZE_2BYTE)
        {
            /* 2 bytes operation */
            readSize = FTFC_MEM_INFLS_IP_SIZE_2BYTE;
        }
        else
        {
            /* 1 byte operation */
        }
    }
    /* Both the lowest one bit are zero: 2 bytes aligned */
    else if (0UL == (combinedAddress & 0x01UL))
    {
        if (byteRemain >= FTFC_MEM_INFLS_IP_SIZE_2BYTE)
        {
            /* 2 bytes operation */
            readSize = FTFC_MEM_INFLS_IP_SIZE_2BYTE;
        }
        else
        {
            /* 1 byte operation */
        }
    }
    else
    {
        /* 1 byte operation */
    }

    return readSize;
}


/**
 * Function Name : Ftfc_Mem_InFls_Ip_ReadData32
 * Description      Reads 4 bytes from the specified address
 */
static inline uint32 Ftfc_Mem_InFls_Ip_ReadData32(uint32 address)
{
    return (*((uint32 *)address));
}


/**
 * Function Name : Ftfc_Mem_InFls_Ip_ReadData16
 * Description      Reads 2 bytes from the specified address
 */
static inline uint16 Ftfc_Mem_InFls_Ip_ReadData16(uint32 address)
{
    return (*((uint16 *)address));
}


/**
 * Function Name : Ftfc_Mem_InFls_Ip_ReadData8
 * Description      Reads 1 byte from the specified address
 */
static inline uint8 Ftfc_Mem_InFls_Ip_ReadData8(uint32 address)
{
    return (*((uint8 *)address));
}


/**
 * Function Name : Ftfc_Mem_InFls_Ip_ReadData
 * Description      Accesses the flash to read data to user buffer
 */
static void Ftfc_Mem_InFls_Ip_ReadData(uint32 readSize,
                                 uint32 readAddress,
                                 uint32 desAddress,
                                 uint8 *desAddressPtr
                                )
{
    switch (readSize)
    {
        case FTFC_MEM_INFLS_IP_SIZE_4BYTE:
            *((uint32 *)desAddress) = Ftfc_Mem_InFls_Ip_ReadData32(readAddress);
            break;

        case FTFC_MEM_INFLS_IP_SIZE_2BYTE:
            *((uint16 *)desAddress) = Ftfc_Mem_InFls_Ip_ReadData16(readAddress);
            break;

        case FTFC_MEM_INFLS_IP_SIZE_1BYTE:
            *desAddressPtr = Ftfc_Mem_InFls_Ip_ReadData8(readAddress);  /* Using uint8 directly to avoid pointer casting */
            break;

        default:
            /* Do nothing */
            break;
    }
}


/**
 * Function Name : Ftfc_Mem_InFls_Ip_CheckValidRange
 * Description   : Checks both start and end address are within the Program or Data flash address space, using before Read/Compare/Write operations
 */
static boolean Ftfc_Mem_InFls_Ip_CheckValidRange(uint32 startAddress, uint32 length)
{
    boolean bRetVal = (boolean)FALSE;
    uint32 endAddress = startAddress + length - 1UL;

    /* The start and end address must be within the program flash or data flash address space */
     if ((((boolean)TRUE == (boolean)FTFC_MEM_INFLS_IP_ADDRESS_VALID_P_FLASH(startAddress)) &&
           ((boolean)TRUE == (boolean)FTFC_MEM_INFLS_IP_ADDRESS_VALID_P_FLASH(endAddress)))
        ||
          (((boolean)TRUE == (boolean)FTFC_MEM_INFLS_IP_ADDRESS_VALID_D_FLASH(startAddress)) &&
           ((boolean)TRUE == (boolean)FTFC_MEM_INFLS_IP_ADDRESS_VALID_D_FLASH(endAddress)))
        )
    {
        /* The memory range is valid */
        bRetVal = (boolean)TRUE;
    }

    return bRetVal;
}


/**
 * Function Name : Ftfc_Mem_InFls_Ip_ReadPreCheck
 * Description      Prepare before starting a new read operation
 */
static Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_ReadPreCheck(uint32 u32SrcAddress, const uint8 *pDestAddressPtr, uint32 u32Length)
{
    Ftfc_Mem_InFls_Ip_StatusType eReturnCode = STATUS_FTFC_MEM_INFLS_IP_ERROR_INPUT_PARAM;

    /* Check 1: The user buffer pointer cannot be a null pointer */
    if (NULL_PTR == pDestAddressPtr)
    {
        /* Wrong input parameters */
    }
    /* Check 2: The read length must be greater than zero */
    else if (0UL == u32Length)
    {
        /* Wrong input parameters */
    }
    /* Check 3: The start and end address must be within the program flash or data flash address space */
    else if ((boolean)FALSE == Ftfc_Mem_InFls_Ip_CheckValidRange(u32SrcAddress, u32Length))
    {
        /* Wrong input parameters */
    }
    else
    {
        /* Reaching here means that everything is ok */
        eReturnCode = Ftfc_Mem_InFls_Ip_ControllerBusy();
    }

    return eReturnCode;
}

/**
 * @brief Read data form addressRead then push data to pAddressOutput
 */
static Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_ReadOperation(uint32 addressRead, uint8 *pAddressOutput, uint32 length)
{
    Ftfc_Mem_InFls_Ip_StatusType readStatus;

    /* Variable temporary */
    uint32 addressReadTemp    = addressRead;
    uint32 addressOutputTemp  = (uint32)pAddressOutput; /* Get Address uint32 first of Array Data */
    uint8 *pAddressOutputTemp = pAddressOutput;         /* Get Address uint8  first of Array Data */
    uint32 bytesRemain        = length;

    uint32 readSize;

    /* Invalidate prefetch buffer before reading to make sure that the driver always reads the new data from flash */
    Ftfc_Mem_InFls_Ip_InvalidPrefetchBuff();

    /* Start read operation */
    do
    {
#if ((FTFC_MEM_INFLS_IP_ECC_CHECK == STD_ON) || (FTFC_MEM_INFLS_IP_ECC_CHECK_BY_AUTOSAR_OS == STD_ON))
#if (FTFC_MEM_INFLS_IP_ECC_SYNDROME_AND_DATA_ADDRESS_CHECK == STD_ON)
        /* Save the current memory access address for ECC handling */
        Ftfc_Mem_InFls_Ip_u32ReadAddressPtr = addressReadTemp;
#endif
        /* Clear ECC flag */
        FTFx_BaseAddress->FERSTAT = FTFx_FERSTAT_DFDIF_MASK;
#endif

        /* Compute the maximum read size */
        readSize = Ftfc_ComputeReadSize(addressReadTemp, addressOutputTemp, bytesRemain);

        /* Start flash access */
        Ftfc_Mem_InFls_Ip_FlashAccessCalloutStart();

        /* Access the flash to read data to user buffer */
        Ftfc_Mem_InFls_Ip_ReadData(readSize, addressReadTemp, addressOutputTemp, pAddressOutputTemp);

        /* Finish flash access */
        Ftfc_Mem_InFls_Ip_FlashAccessCalloutFinish();

        /* Check for hardware errors or ECC happend */
        readStatus = Ftfc_Mem_InFls_Ip_ReadStatus((boolean)TRUE);
        if (STATUS_FTFC_MEM_INFLS_IP_SUCCESS != readStatus)
        {
            break;
        }

        /* Move to the next data */
        addressReadTemp     += readSize;
        addressOutputTemp   += readSize;
        pAddressOutputTemp   = &pAddressOutputTemp[readSize];
        bytesRemain         -= readSize;
    }
    while (0UL < bytesRemain);

    /* Checking if working was successful: the requested bytes were copied and no errors happened */
    if (0UL == bytesRemain)
    {
        /* Mark as success */
        Ftfc_Mem_InFls_Ip_eReadStatus = STATUS_FTFC_MEM_INFLS_IP_SUCCESS;
    }

    return Ftfc_Mem_InFls_Ip_eReadStatus;
}


/**
 * Function Name : Ftfc_Mem_InFls_Ip_Read
 * Description      This function fills data to pDestAddressPtr with data from the specified address
 * @implements      Ftfc_Mem_InFls_Ip_Read_Activity
 */
Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_Read(Ftfc_Mem_InFls_Ip_AddressType u32SrcAddress,
                                                    uint8                        *pDestAddressPtr,
                                                    Ftfc_Mem_InFls_Ip_LengthType  u32Length
                                                   )
{
#if (FTFC_MEM_INFLS_IP_DEV_ERROR_DETECT == STD_ON)
    boolean CheckValidStatus;

    FTFC_MEM_INFLS_IP_DEV_ASSERT(u32Length > 0UL);
    FTFC_MEM_INFLS_IP_DEV_ASSERT(pDestAddressPtr != NULL_PTR);

    CheckValidStatus = Ftfc_Mem_InFls_Ip_CheckValidRange(u32SrcAddress, u32Length);
    FTFC_MEM_INFLS_IP_DEV_ASSERT(CheckValidStatus);
#endif

    /* Checking before reading */
    Ftfc_Mem_InFls_Ip_eReadStatus = Ftfc_Mem_InFls_Ip_ReadPreCheck(u32SrcAddress, pDestAddressPtr, u32Length);

    /* Everything is ready for a new read operation */
    if (STATUS_FTFC_MEM_INFLS_IP_SUCCESS == Ftfc_Mem_InFls_Ip_eReadStatus)
    {
        /* Change to busy state */
        Ftfc_Mem_InFls_Ip_eReadStatus = STATUS_FTFC_MEM_INFLS_IP_BUSY;

        /* Invalidate cache */
#if (STD_ON == FTFC_MEM_INFLS_IP_SYNCHRONIZE_CACHE)
        Ftfc_Mem_InFls_Ip_SynchronizeCache(u32SrcAddress, u32Length);
#endif
        /* Start Read Operation */
        Ftfc_Mem_InFls_Ip_eReadStatus = Ftfc_Mem_InFls_Ip_ReadOperation(u32SrcAddress, pDestAddressPtr, u32Length);
    }

    return Ftfc_Mem_InFls_Ip_eReadStatus;
}


/**
 * @brief Check status after memory accessing of Read/Compare operations
 *
 * @return the status of the most recent memory access
 * @retval STATUS_FTFC_MEM_INFLS_IP_ERROR     Hardware errors or ECC happend or there was a compare error
 * @retval STATUS_FTFC_MEM_INFLS_IP_SUCCESS   Everything was ok
 */
static Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_ReadStatus(boolean compareStatus)
{
    Ftfc_Mem_InFls_Ip_StatusType eRetVal = STATUS_FTFC_MEM_INFLS_IP_ERROR;
    uint8 errorFlags;

    /* Check everything in order of priority: hardware > ECC > compare */

    /* Step 1: Check for hardware errors */
    errorFlags = FTFx_BaseAddress->FSTAT & (FTFx_FSTAT_RDCOLERR_MASK | FTFx_FSTAT_ACCERR_MASK | FTFx_FSTAT_FPVIOL_MASK);
    if (0U != errorFlags)
    {
        /* Clear all error flags: write 1 to clear */
        FTFx_BaseAddress->FSTAT = errorFlags;
        /* Hardware errors: change read status to error state */
        Ftfc_Mem_InFls_Ip_eReadStatus = STATUS_FTFC_MEM_INFLS_IP_ERROR;
    }

#if ((FTFC_MEM_INFLS_IP_ECC_CHECK == STD_ON) || (FTFC_MEM_INFLS_IP_ECC_CHECK_BY_AUTOSAR_OS == STD_ON))
    /* Step 2: Check for ECC */
    else if (STATUS_FTFC_MEM_INFLS_IP_ECC_UNCORRECTED == Ftfc_Mem_InFls_Ip_eReadStatus)
    {
        /* ECC happened in the DsiHandler: keep the current status */
    }
#endif

    /* Step 3: Check for compare error */
    else if ((boolean)TRUE != compareStatus)
    {
        /* Compare error: update read status */
        Ftfc_Mem_InFls_Ip_eReadStatus = STATUS_FTFC_MEM_INFLS_IP_ERROR_PROGRAM_VERIFY;
    }
    else
    {
        /* Everything was ok */
        eRetVal = STATUS_FTFC_MEM_INFLS_IP_SUCCESS;
    }

    return eRetVal;
}


/**
 * Function Name : Ftfc_Mem_InFls_Ip_ComparePreCheck
 * Description      Prepare before starting a new cpmpare operation
 */
static Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_ComparePreCheck(uint32 u32SrcAddress, uint32 u32Length)
{
    Ftfc_Mem_InFls_Ip_StatusType eReturnCode = STATUS_FTFC_MEM_INFLS_IP_ERROR_INPUT_PARAM;

    /* Check 1: The read length must be greater than zero */
    if (0UL == u32Length)
    {
        /* Wrong input parameters */
    }
    /* Check 2: The start and end address must be within the program flash or data flash address space */
    else if ((boolean)FALSE == Ftfc_Mem_InFls_Ip_CheckValidRange(u32SrcAddress, u32Length))
    {
        /* Wrong input parameters */
    }
    else
    {
        /* Reaching here means that everything is ok */
        eReturnCode = Ftfc_Mem_InFls_Ip_ControllerBusy();
    }

    return eReturnCode;
}


/**
 * Function Name : Ftfc_Mem_InFls_Ip_Compare
 * Description   : Checks that there is the desired data at the specified address
 * @implements      Ftfc_Mem_InFls_Ip_Compare_Activity
 */
Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_Compare(Ftfc_Mem_InFls_Ip_AddressType u32SrcAddress,
                                                       const uint8                  *pCompareAddressPtr,
                                                       Ftfc_Mem_InFls_Ip_LengthType  u32Length
                                                      )
{
    uint32 srcAddress = (uint32)u32SrcAddress;

#if (FTFC_MEM_INFLS_IP_DEV_ERROR_DETECT == STD_ON)
    boolean CheckValidStatus;
    FTFC_MEM_INFLS_IP_DEV_ASSERT(u32Length > 0UL);
    CheckValidStatus = Ftfc_Mem_InFls_Ip_CheckValidRange(u32SrcAddress, u32Length);
    FTFC_MEM_INFLS_IP_DEV_ASSERT(CheckValidStatus);
#endif

    /* Checking before comparing */
    Ftfc_Mem_InFls_Ip_eReadStatus = Ftfc_Mem_InFls_Ip_ComparePreCheck(u32SrcAddress, u32Length);

    /* Everything is ready for a new compare operation */
    if (STATUS_FTFC_MEM_INFLS_IP_SUCCESS == Ftfc_Mem_InFls_Ip_eReadStatus)
    {
        /* Change to busy state */
        Ftfc_Mem_InFls_Ip_eReadStatus = STATUS_FTFC_MEM_INFLS_IP_BUSY;

        /* Invalidate cache before reading memory */
        #if (STD_ON == FTFC_MEM_INFLS_IP_SYNCHRONIZE_CACHE)
        Ftfc_Mem_InFls_Ip_SynchronizeCache(srcAddress, u32Length);
        #endif

        /* Choose the type of comparison */
        if (NULL_PTR != pCompareAddressPtr)
        {
            /* Verify the written data */
            Ftfc_Mem_InFls_Ip_ProgramVerify(srcAddress, pCompareAddressPtr, u32Length);
        }
        else
        {
            /* Blank check */
            Ftfc_Mem_InFls_Ip_EraseVerify(srcAddress, u32Length);
        }

    }

    return Ftfc_Mem_InFls_Ip_eReadStatus;
}


/**
 * @brief      The function to Verify Data after erase operation.
 *
 * @param[in]  readSize   the size of data to read
 * @param[in]  srcAddress the internal flash address (physical)
 *
 * @return boolean
 */
static boolean Ftfc_Mem_InFls_Ip_EraseVerifyData(uint32 readSize, uint32 srcAddress)
{
    boolean compareStatus = (boolean)TRUE;

    switch (readSize)
    {
        case FTFC_MEM_INFLS_IP_SIZE_4BYTE:
            if ((uint32)FTFC_MEM_INFLS_IP_ERASED_VALUE != Ftfc_Mem_InFls_Ip_ReadData32(srcAddress))
            {
                compareStatus = (boolean)FALSE;
            }
            break;

        case FTFC_MEM_INFLS_IP_SIZE_2BYTE:
            if ((uint16)FTFC_MEM_INFLS_IP_ERASED_VALUE != Ftfc_Mem_InFls_Ip_ReadData16(srcAddress))
            {
                compareStatus = (boolean)FALSE;
            }
            break;

        case FTFC_MEM_INFLS_IP_SIZE_1BYTE:
            if ((uint8)FTFC_MEM_INFLS_IP_ERASED_VALUE != Ftfc_Mem_InFls_Ip_ReadData8(srcAddress))
            {
                compareStatus = (boolean)FALSE;
            }
            break;

        default:
            /* Do nothing */
            break;
    }

    return compareStatus;
}


/**
 * @brief      The function to Verify Data after progfram operation.
 *
 * @param[in]  readSize   the size of data to read
 * @param[in]  srcAddress the internal flash address (physical)
 * @param[in]  desAddress the user's buffer address
 *
 * @return boolean
 */
static boolean Ftfc_Mem_InFls_Ip_ProgramVerifyData(uint32 readSize, uint32 srcAddress, uint32 desAddress)
{
    boolean compareStatus = (boolean)TRUE;

    switch (readSize)
    {
        case FTFC_MEM_INFLS_IP_SIZE_4BYTE:
            if (Ftfc_Mem_InFls_Ip_ReadData32(srcAddress) != Ftfc_Mem_InFls_Ip_ReadData32(desAddress))
            {
                compareStatus = (boolean)FALSE;
            }
            break;

        case FTFC_MEM_INFLS_IP_SIZE_2BYTE:
            if (Ftfc_Mem_InFls_Ip_ReadData16(srcAddress) != Ftfc_Mem_InFls_Ip_ReadData16(desAddress))
            {
                compareStatus = (boolean)FALSE;
            }
            break;

        case FTFC_MEM_INFLS_IP_SIZE_1BYTE:
            if (Ftfc_Mem_InFls_Ip_ReadData8(srcAddress) != Ftfc_Mem_InFls_Ip_ReadData8(desAddress))
            {
                compareStatus = (boolean)FALSE;
            }
            break;

        default:
            /* Do nothing */
            break;
    }

    return compareStatus;
}


/**
 * @brief Verifies data written in serial flash
 */
static void Ftfc_Mem_InFls_Ip_ProgramVerify(uint32 address, const uint8 * data, uint32 size)
{
    Ftfc_Mem_InFls_Ip_StatusType status;

    uint32 verifySrcAddress = address;
    uint32 verifyDesAddress = (uint32)data;
    uint32 verifySizeRemain = size;
    uint32 readSize;
    boolean compareStatus;

    /* Invalidate prefetch buffer before reading to make sure that the driver always reads the new data from flash */
    Ftfc_Mem_InFls_Ip_InvalidPrefetchBuff();

    /* Start compare operation */
    do
    {

#if ((FTFC_MEM_INFLS_IP_ECC_CHECK == STD_ON) || (FTFC_MEM_INFLS_IP_ECC_CHECK_BY_AUTOSAR_OS == STD_ON))
    #if (FTFC_MEM_INFLS_IP_ECC_SYNDROME_AND_DATA_ADDRESS_CHECK == STD_ON)
        /* Save the current memory access address for ECC handling */
        Ftfc_Mem_InFls_Ip_u32ReadAddressPtr = verifySrcAddress;
    #endif
        /* Clear ECC flag */
        FTFx_BaseAddress->FERSTAT = FTFx_FERSTAT_DFDIF_MASK;
#endif

        /* Compute the maximum read size */
        readSize = Ftfc_ComputeReadSize(verifySrcAddress, verifyDesAddress, verifySizeRemain);

        /* Start flash access */
        Ftfc_Mem_InFls_Ip_FlashAccessCalloutStart();

        /* Compare data of verifySrcAddress with verifyDesAddress  */
        compareStatus = Ftfc_Mem_InFls_Ip_ProgramVerifyData(readSize, verifySrcAddress, verifyDesAddress);

        /* Finish flash access */
        Ftfc_Mem_InFls_Ip_FlashAccessCalloutFinish();

        /* Check for hardware errors or ECC happend or compare error */
        status = Ftfc_Mem_InFls_Ip_ReadStatus(compareStatus);
        if (STATUS_FTFC_MEM_INFLS_IP_SUCCESS != status)
        {
            break;
        }

        /* Move to next data */
        verifySrcAddress += readSize;
        verifyDesAddress += readSize;
        verifySizeRemain -= readSize;
    }
    while (0UL < verifySizeRemain);

    /* Checking if working is successful: the requested bytes were copied and no errors happend */
    if (0UL == verifySizeRemain)
    {
        /* Mark as success */
        Ftfc_Mem_InFls_Ip_eReadStatus = STATUS_FTFC_MEM_INFLS_IP_SUCCESS;
    }
}

/**
 * @brief Verifies that an area in memory flash is in erased state
 */
static void Ftfc_Mem_InFls_Ip_EraseVerify(uint32 address, uint32 size)
{
    uint32 verifySrcAddress = address;
    uint32 verifySizeRemain = size;
    uint32 readSize;
    Ftfc_Mem_InFls_Ip_StatusType status;
    boolean compareStatus;

    /* Invalidate prefetch buffer before reading to make sure that the driver always reads the new data from flash */
    Ftfc_Mem_InFls_Ip_InvalidPrefetchBuff();

    /* Start compare operation */
    do
    {

        #if ((FTFC_MEM_INFLS_IP_ECC_CHECK == STD_ON) || (FTFC_MEM_INFLS_IP_ECC_CHECK_BY_AUTOSAR_OS == STD_ON))
        #if (FTFC_MEM_INFLS_IP_ECC_SYNDROME_AND_DATA_ADDRESS_CHECK == STD_ON)
        /* Save the current memory access address for ECC handling */
        Ftfc_Mem_InFls_Ip_u32ReadAddressPtr = verifySrcAddress;
        #endif
        /* Clear ECC flag */
        FTFx_BaseAddress->FERSTAT = FTFx_FERSTAT_DFDIF_MASK;
        #endif

        /* Compute the maximum read size */
        readSize = Ftfc_ComputeReadSize(verifySrcAddress, 0UL, verifySizeRemain);

        /* Start flash access */
        Ftfc_Mem_InFls_Ip_FlashAccessCalloutStart();

        /* Compare data */
        compareStatus = Ftfc_Mem_InFls_Ip_EraseVerifyData(readSize, verifySrcAddress);

        /* Finish flash access */
        Ftfc_Mem_InFls_Ip_FlashAccessCalloutFinish();

        /* Check for hardware errors or ECC happend or compare error */
        status = Ftfc_Mem_InFls_Ip_ReadStatus(compareStatus);
        if (STATUS_FTFC_MEM_INFLS_IP_SUCCESS != status)
        {
            break;
        }

        /* Move to next data */
        verifySrcAddress += readSize;
        verifySizeRemain -= readSize;
    }
    while (0UL < verifySizeRemain);

    /* Checking if working is successful: the requested bytes were copied and no errors happend */
    if (0UL == verifySizeRemain)
    {
        /* Mark as success */
        Ftfc_Mem_InFls_Ip_eReadStatus = STATUS_FTFC_MEM_INFLS_IP_SUCCESS;
    }
}


/**
 * Function Name : Ftfc_Mem_InFls_Ip_SectorErasePreCheck
 * Description   : Prepare before starting a new erase operation: input params, memory protection and hardware status
 */
static Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_SectorErasePreCheck(uint32 u32SectorStartAddress)
{
    Ftfc_Mem_InFls_Ip_StatusType eReturnCode = STATUS_FTFC_MEM_INFLS_IP_SUCCESS;

    /* Step 1: Check address is in the valid range and align with the size of a program or data sector */
    if (((boolean)FALSE == ((boolean)FTFC_MEM_INFLS_IP_ADDRESS_VALID(u32SectorStartAddress)))  ||
         ((boolean)FALSE == ((boolean)FTFC_MEM_INFLS_IP_SECTOR_ALIGNED(u32SectorStartAddress)))
       )
    {
        /* Wrong input parameters*/
        eReturnCode = STATUS_FTFC_MEM_INFLS_IP_ERROR_INPUT_PARAM;
    }

    /* Step 2: Check if the memory area is protected or not */
    if (STATUS_FTFC_MEM_INFLS_IP_SUCCESS == eReturnCode)
    {
        eReturnCode = Ftfc_Mem_InFls_Ip_CheckFProtReg(u32SectorStartAddress);
    }

    /* Step 3: Check if hardware is busy or not */
    if (STATUS_FTFC_MEM_INFLS_IP_SUCCESS == eReturnCode)
    {
        eReturnCode = Ftfc_Mem_InFls_Ip_ControllerBusy();
    }

    return eReturnCode;
}

/**
 * Function Name : Ftfc_Mem_InFls_Ip_SectorErase
 * Description   : Accepts and erases a selected program flash or data flash sector if possible
 * @implements     Ftfc_Mem_InFls_Ip_SectorErase_Activity
 */
Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_SectorErase(Ftfc_Mem_InFls_Ip_AddressType u32SectorStartAddress)
{
    Ftfc_Mem_InFls_Ip_StatusType eRetVal;
    boolean bAddressValid  = FTFC_MEM_INFLS_IP_ADDRESS_VALID(u32SectorStartAddress);
    boolean bSectorAligned = FTFC_MEM_INFLS_IP_SECTOR_ALIGNED(u32SectorStartAddress);

#if (FTFC_MEM_INFLS_IP_DEV_ERROR_DETECT == STD_ON)
    FTFC_MEM_INFLS_IP_DEV_ASSERT(bAddressValid);
    FTFC_MEM_INFLS_IP_DEV_ASSERT(bSectorAligned);
#endif

    /* Unused variables */
    (void)bAddressValid;
    (void)bSectorAligned;

    /* Check(if erase suspended is possible) if any ongoing erase suspended and abort it */
    eRetVal = Ftfc_Mem_InFls_Ip_AbortSuspended();

    if (STATUS_FTFC_MEM_INFLS_IP_SUCCESS == eRetVal)
    {
        /* Pre-check before starting erase operation */
        eRetVal = Ftfc_Mem_InFls_Ip_SectorErasePreCheck(u32SectorStartAddress);
    }

    /* Verify that there is no other job in progress */
    if (STATUS_FTFC_MEM_INFLS_IP_SUCCESS == eRetVal)
    {
#if ((FTFC_MEM_INFLS_IP_ERASE_VERIFICATION == STD_ON) || (STD_ON == FTFC_MEM_INFLS_IP_SYNCHRONIZE_CACHE))
        /* Prepare address for the blank check */
        Ftfc_Mem_InFls_Ip_u32ErasedSectorAddress = u32SectorStartAddress;
#endif

        /* Setup erase operation */
        Ftfc_Mem_InFls_Ip_NewCmdOperation(FLASH_CMD_ERASE_SECTOR,
                                          u32SectorStartAddress,
                                          NULL_PTR
#if (STD_ON == FTFC_TIMEOUT_SUPERVISION_ENABLED)
                                          , FTFC_MEM_INFLS_IP_ERASE_TIMEOUT
#endif
                                          );
    }

    /* Save the erase job's status */
    Ftfc_Mem_InFls_Ip_eEraseStatus = eRetVal;
    return eRetVal;
}

 /*
 * Function Name : Ftfc_Mem_InFls_Ip_EraseSuspend
 * Description   : Suspend a current operation of Flash erase sector command.
 */
Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_EraseSuspend(void)
{
    #ifdef MCAL_ENABLE_FAULT_INJECTION
    /* Note: (For testing purpose only)
    A workaround to avoid duplication of fault injection label in inline assembly statement below.
    The volatile keyword helps to increase the code size, thus prevents the compiler from inlining this function.
    */
    volatile Ftfc_Mem_InFls_Ip_StatusType eRetVal = STATUS_FTFC_MEM_INFLS_IP_SUCCESS;
    #else
    Ftfc_Mem_InFls_Ip_StatusType eRetVal = STATUS_FTFC_MEM_INFLS_IP_SUCCESS;
    #endif

    /* Start of exclusive area */
    SchM_Enter_Mem_43_INFLS_MEM_EXCLUSIVE_AREA_08();

    /* If the Erase Operation is ready to be suspended */
    if (0U == ((FTFx_BaseAddress->FSTAT) & FTFx_FSTAT_CCIF_MASK))
    {
        /* Set ERSSUSP field bit to request suspend */
        FTFx_BaseAddress->FCNFG |= FTFx_FCNFG_ERSSUSP_MASK;

#if (FTFC_TIMEOUT_SUPERVISION_ENABLED == STD_ON)
        uint32 u32ElapsedSuspendTicks = 0UL;
        uint32 u32TimeoutSuspendTicks = OsIf_MicrosToTicks(FTFC_MEM_INFLS_IP_ERASE_SUSPEND_TIMEOUT, (OsIf_CounterType)FTFC_MEM_INFLS_IP_TIMEOUT_TYPE);
        uint32 u32CurrentSuspendTicks = OsIf_GetCounter((OsIf_CounterType)FTFC_MEM_INFLS_IP_TIMEOUT_TYPE);
#endif

        /* Wait until suspended done */
        while (0U == ((FTFx_BaseAddress->FSTAT) & FTFx_FSTAT_CCIF_MASK))
        {
#if (FTFC_TIMEOUT_SUPERVISION_ENABLED == STD_ON)
            u32ElapsedSuspendTicks += OsIf_GetElapsed(&u32CurrentSuspendTicks, (OsIf_CounterType)FTFC_MEM_INFLS_IP_TIMEOUT_TYPE);
            if (u32ElapsedSuspendTicks >= u32TimeoutSuspendTicks)
            {
                eRetVal = STATUS_FTFC_MEM_INFLS_IP_ERROR_TIMEOUT;
                break;
            }
#endif
        }
    }
    else
    {
        eRetVal = STATUS_FTFC_MEM_INFLS_IP_ERROR;
    }

    /* End of exclusive area */
    SchM_Exit_Mem_43_INFLS_MEM_EXCLUSIVE_AREA_08();

    return eRetVal;
}


 /*
 * Function Name : Ftfc_Mem_InFls_Ip_EraseResume
 * Description   : Resume a previous suspended operation of Flash erase sector command.
 */
Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_EraseResume(void)
{
    #ifdef MCAL_ENABLE_FAULT_INJECTION
    /* Note: (For testing purpose only)
    A workaround to avoid duplication of fault injection label in inline assembly statement below.
    The volatile keyword helps to increase the code size, thus prevents the compiler from inlining this function.
    */
    volatile Ftfc_Mem_InFls_Ip_StatusType eRetVal = STATUS_FTFC_MEM_INFLS_IP_SUCCESS;
    #else
    Ftfc_Mem_InFls_Ip_StatusType eRetVal = STATUS_FTFC_MEM_INFLS_IP_SUCCESS;
    #endif

    /* Check ERSSUSP bit of the flash configuration register */
    if (((FTFx_BaseAddress->FCNFG) & FTFx_FCNFG_ERSSUSP_MASK) == FTFx_FCNFG_ERSSUSP_MASK)
    {
        /* Clear CCIF to resume command */
        FTFx_BaseAddress->FSTAT |= FTFx_FSTAT_CCIF_MASK;

#if (FTFC_TIMEOUT_SUPERVISION_ENABLED == STD_ON)
        uint32 u32ElapsedResumeTicks = 0UL;
        uint32 u32TimeoutResumeTicks = OsIf_MicrosToTicks(FTFC_MEM_INFLS_IP_ERASE_RESUME_TIMEOUT, (OsIf_CounterType)FTFC_MEM_INFLS_IP_TIMEOUT_TYPE);
        uint32 u32CurrentResumeTicks = OsIf_GetCounter((OsIf_CounterType)FTFC_MEM_INFLS_IP_TIMEOUT_TYPE);
#endif

        /* Wait until resume done */
        while (0U != ((FTFx_BaseAddress->FCNFG) & FTFx_FCNFG_ERSSUSP_MASK))
        {
#if (FTFC_TIMEOUT_SUPERVISION_ENABLED == STD_ON)
            u32ElapsedResumeTicks += OsIf_GetElapsed(&u32CurrentResumeTicks, (OsIf_CounterType)FTFC_MEM_INFLS_IP_TIMEOUT_TYPE);
            if (u32ElapsedResumeTicks >= u32TimeoutResumeTicks)
            {
                eRetVal = STATUS_FTFC_MEM_INFLS_IP_ERROR_TIMEOUT;
                break;
            }
#endif
        }
    }
    else
    {
        /* The Erase Operation is not ready to be suspended */
        eRetVal = STATUS_FTFC_MEM_INFLS_IP_ERROR;
    }

    return eRetVal;
}

/**
 * Function Name : Ftfc_Mem_InFls_Ip_SectorEraseStatus
 * Description   : Checks the status of the hardware erase started by the Ftfc_Mem_InFls_Ip_SectorErase function.
 * @implements      Ftfc_Mem_InFls_Ip_SectorEraseStatus_Activity
 */
Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_SectorEraseStatus(void)
{
    Ftfc_Mem_InFls_Ip_StatusType eReturnCode;
#if ((FTFC_MEM_INFLS_IP_ERASE_VERIFICATION == STD_ON) || (STD_ON == FTFC_MEM_INFLS_IP_SYNCHRONIZE_CACHE))
    uint32 u32Length = 0;
#endif

    /* Check if the erase job started successfully */
    if (STATUS_FTFC_MEM_INFLS_IP_SUCCESS == Ftfc_Mem_InFls_Ip_eEraseStatus)
    {
        /* The job started successfully, polling the controller status */
        eReturnCode = Ftfc_Mem_InFls_Ip_CtrlStatus();

#if ((FTFC_MEM_INFLS_IP_ERASE_VERIFICATION == STD_ON) || (STD_ON == FTFC_MEM_INFLS_IP_SYNCHRONIZE_CACHE))
        /* The job has done, perform the verification */
        if (STATUS_FTFC_MEM_INFLS_IP_SUCCESS == eReturnCode)
        {
            /* Check the sector size */
            if (Ftfc_Mem_InFls_Ip_u32ErasedSectorAddress >= FTFC_MEM_INFLS_IP_D_FLASH_BASE_ADDR)
            {
                u32Length = FTFC_MEM_INFLS_IP_D_FLASH_SECTOR_SIZE;
            }
            else
            {
                u32Length = FTFC_MEM_INFLS_IP_P_FLASH_SECTOR_SIZE;
            }

        #if (FTFC_MEM_INFLS_IP_ERASE_VERIFICATION == STD_ON)
            MCAL_FAULT_INJECTION_POINT(FIP_FTFC_FAILED_VERIFICATION_ERASE);

            /* Verify blank check after erasing the data */
            eReturnCode = Ftfc_Mem_InFls_Ip_Compare(Ftfc_Mem_InFls_Ip_u32ErasedSectorAddress, NULL_PTR, u32Length);
            if ((STATUS_FTFC_MEM_INFLS_IP_SUCCESS != eReturnCode) && (STATUS_FTFC_MEM_INFLS_IP_ECC_UNCORRECTED != eReturnCode))
            {
                eReturnCode = STATUS_FTFC_MEM_INFLS_IP_ERROR_BLANK_CHECK;
            }
        #else
            /* Invalidate cache: this will be done inside the function Ftfc_Mem_InFls_Ip_Compare if erase verification is enabled */
            Ftfc_Mem_InFls_Ip_SynchronizeCache(Ftfc_Mem_InFls_Ip_u32ErasedSectorAddress, u32Length);
        #endif
        }
#endif
    }
    else
    {
        /* The erase job did not start successfully, return error immediately */
        eReturnCode = STATUS_FTFC_MEM_INFLS_IP_ERROR;
    }

    return eReturnCode;
}

/**
 * Function Name : Ftfc_Mem_InFls_Ip_WritePreCheck
 * Description   : Prepare before starting a new write operation: input params, memory protection and hardware status
 */
static Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_WritePreCheck(uint32 u32DestAddress, const uint8 *pSourceAddressPtr, uint32 u32Length)
{
    Ftfc_Mem_InFls_Ip_StatusType eReturnCode = STATUS_FTFC_MEM_INFLS_IP_SUCCESS;

    /* Step 1: Check length and address should align with FTFC_MEM_INFLS_IP_WRITE_DOUBLE_WORD, the start and end address must be a valid range */
    if (((u32DestAddress % (uint8)FTFC_MEM_INFLS_IP_WRITE_DOUBLE_WORD) != 0U) ||
        (u32Length      != FTFC_MEM_INFLS_IP_WRITE_DOUBLE_WORD)               ||
        (NULL_PTR       == pSourceAddressPtr)                                 ||
        ((boolean)FALSE == Ftfc_Mem_InFls_Ip_CheckValidRange(u32DestAddress, u32Length))
       )
    {
        /* Wrong input parameters*/
        eReturnCode = STATUS_FTFC_MEM_INFLS_IP_ERROR_INPUT_PARAM;
    }

    /* Step 2: Check if the memory area is protected or not */
    if (STATUS_FTFC_MEM_INFLS_IP_SUCCESS == eReturnCode)
    {
        eReturnCode = Ftfc_Mem_InFls_Ip_CheckFProtReg(u32DestAddress);
    }

    /* Step 3: Check if hardware is busy or not */
    if (STATUS_FTFC_MEM_INFLS_IP_SUCCESS == eReturnCode)
    {
        eReturnCode = Ftfc_Mem_InFls_Ip_ControllerBusy();
    }

    return eReturnCode;
}

/**
 * Function Name : Ftfc_Mem_InFls_Ip_Write
 * Description   : Writes data into the memory array using the main interface. Initiates the hardware write and then exits.
 * @implements     Ftfc_Mem_InFls_Ip_Write_Activity
 */
Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_Write(uint32 u32DestAddress, const uint8 *pSourceAddressPtr, uint32 u32Length)
{
    Ftfc_Mem_InFls_Ip_StatusType eReturnCode;

#if (FTFC_MEM_INFLS_IP_DEV_ERROR_DETECT == STD_ON)
    boolean CheckValidStatus;
    CheckValidStatus = Ftfc_Mem_InFls_Ip_CheckValidRange(u32DestAddress, u32Length);
    FTFC_MEM_INFLS_IP_DEV_ASSERT(CheckValidStatus);
    FTFC_MEM_INFLS_IP_DEV_ASSERT(0U == (u32DestAddress % (uint8)FTFC_MEM_INFLS_IP_WRITE_DOUBLE_WORD));
    FTFC_MEM_INFLS_IP_DEV_ASSERT(FTFC_MEM_INFLS_IP_WRITE_DOUBLE_WORD == u32Length);
    FTFC_MEM_INFLS_IP_DEV_ASSERT(pSourceAddressPtr != NULL_PTR);
#endif

    /* Check(if erase suspended is possible) if any ongoing erase suspended and abort it */
    eReturnCode = Ftfc_Mem_InFls_Ip_AbortSuspended();

    if (STATUS_FTFC_MEM_INFLS_IP_SUCCESS == eReturnCode)
    {
        /* Pre-check before starting write operation */
        eReturnCode = Ftfc_Mem_InFls_Ip_WritePreCheck(u32DestAddress, pSourceAddressPtr, u32Length);
    }

    /* Verify blank check before writing the data */
#if (FTFC_MEM_INFLS_IP_ERASE_VERIFICATION == STD_ON)
    if (STATUS_FTFC_MEM_INFLS_IP_SUCCESS == eReturnCode)
    {
        eReturnCode = Ftfc_Mem_InFls_Ip_Compare(u32DestAddress, NULL_PTR, u32Length);
        if ((STATUS_FTFC_MEM_INFLS_IP_SUCCESS != eReturnCode) && (STATUS_FTFC_MEM_INFLS_IP_ECC_UNCORRECTED != eReturnCode))
        {
            eReturnCode = STATUS_FTFC_MEM_INFLS_IP_ERROR_BLANK_CHECK;
        }
    }
#endif

    /* Everything is ready for a new write operation */
    if (STATUS_FTFC_MEM_INFLS_IP_SUCCESS == eReturnCode)
    {
#if ((STD_ON == FTFC_MEM_INFLS_IP_PROGRAM_VERIFICATION) || (STD_ON == FTFC_MEM_INFLS_IP_SYNCHRONIZE_CACHE))
        Ftfc_Mem_InFls_Ip_u32ProgrammedAddress = u32DestAddress;
        Ftfc_Mem_InFls_Ip_u32ProgrammedLength  = u32Length;
#endif

#if (STD_ON == FTFC_MEM_INFLS_IP_PROGRAM_VERIFICATION)
        Ftfc_Mem_InFls_Ip_pProgrammedData = pSourceAddressPtr;
#endif

        /* Setup write operation */
        Ftfc_Mem_InFls_Ip_NewCmdOperation(FLASH_CMD_PROGRAM_PHRASE,
                                          u32DestAddress,
                                          pSourceAddressPtr
#if (STD_ON == FTFC_TIMEOUT_SUPERVISION_ENABLED)
                                          , FTFC_MEM_INFLS_IP_WRITE_TIMEOUT
#endif
                                         );
    }

    /* Save the write's job result */
    Ftfc_Mem_InFls_Ip_eWriteStatus = eReturnCode;
    return eReturnCode;
}

/**
 * Function Name : Ftfc_Mem_InFls_Ip_WriteStatus
 * Description   : Checks the status of the hardware program started by the FTFC_MEM_INFLS_IP_MainInterfaceWrite function.
 * @implements      Ftfc_Mem_InFls_Ip_WriteStatus_Activity
 */
Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_WriteStatus(void)
{
    Ftfc_Mem_InFls_Ip_StatusType eReturnCode;

    /* Check if the write job started successfully */
    if (STATUS_FTFC_MEM_INFLS_IP_SUCCESS == Ftfc_Mem_InFls_Ip_eWriteStatus)
    {
        /* The job started successfully, polling the controller status */
        eReturnCode = Ftfc_Mem_InFls_Ip_CtrlStatus();

        /* The job has done, perform the verification */
        if (STATUS_FTFC_MEM_INFLS_IP_SUCCESS == eReturnCode)
        {
#if (FTFC_MEM_INFLS_IP_PROGRAM_VERIFICATION == STD_ON)
            /* Fault Injection point: write data to programmed sector to make the Compare failed */
            MCAL_FAULT_INJECTION_POINT(FLS_FIP_FTFC_FAILED_VERIFICATION_WRITE);

            /* Verify written data */
            eReturnCode = Ftfc_Mem_InFls_Ip_Compare(Ftfc_Mem_InFls_Ip_u32ProgrammedAddress, Ftfc_Mem_InFls_Ip_pProgrammedData, Ftfc_Mem_InFls_Ip_u32ProgrammedLength);
            if ((STATUS_FTFC_MEM_INFLS_IP_SUCCESS != eReturnCode) && (STATUS_FTFC_MEM_INFLS_IP_ECC_UNCORRECTED != eReturnCode))
            {
                eReturnCode = STATUS_FTFC_MEM_INFLS_IP_ERROR_PROGRAM_VERIFY;
            }
#else
        #if (STD_ON == FTFC_MEM_INFLS_IP_SYNCHRONIZE_CACHE)
        /* Invalidate cache: this will be done inside the function Ftfc_Mem_InFls_Ip_Compare if program verification is enabled */
        Ftfc_Mem_InFls_Ip_SynchronizeCache(Ftfc_Mem_InFls_Ip_u32ProgrammedAddress, Ftfc_Mem_InFls_Ip_u32ProgrammedLength);
        #endif
#endif
        }

    }
    else
    {
        /* The write job did not start successfully, return error immediately */
        eReturnCode = STATUS_FTFC_MEM_INFLS_IP_ERROR;
    }

    return eReturnCode;
}

/**
 * Function Name : Ftfc_Mem_InFls_Ip_AbortSuspended
 * Description   : Managing Abort Erase suspend. If there is a pending erase suspended, abort it.
 */
Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_AbortSuspended(void)
{
    Ftfc_Mem_InFls_Ip_StatusType eRetVal = STATUS_FTFC_MEM_INFLS_IP_SUCCESS;
    uint8 tempValueFCNFG_Flags;

    /* Start of exclusive area */
    SchM_Enter_Mem_43_INFLS_MEM_EXCLUSIVE_AREA_07();

    /* Ff there is a suspended operation in progress */
    if (0U != ((FTFx_BaseAddress->FCNFG) & FTFx_FCNFG_ERSSUSP_MASK))
    {
        /* if the memory controller is ready to receive new commands */
        if (0U != ((FTFx_BaseAddress->FSTAT) & FTFx_FSTAT_CCIF_MASK))
        {
            /* abort the suspended erase */
            tempValueFCNFG_Flags = FTFx_BaseAddress->FCNFG;
            FTFx_BaseAddress->FCNFG = tempValueFCNFG_Flags & ((uint8)(~FTFx_FCNFG_ERSSUSP_MASK));
        }
        else    /* memory controller busy */
        {
            /* wait for memory controller to finish current operation */
            if (STATUS_FTFC_MEM_INFLS_IP_SUCCESS == Ftfc_Mem_InFls_Ip_WaitForOperationFinish())
            {
                /* abort the suspended erase */
                tempValueFCNFG_Flags = FTFx_BaseAddress->FCNFG;
                FTFx_BaseAddress->FCNFG = tempValueFCNFG_Flags & ((uint8)(~FTFx_FCNFG_ERSSUSP_MASK));
            }
            else /* operation not finished, aborted on timeout */
            {
                eRetVal = STATUS_FTFC_MEM_INFLS_IP_ERROR;
            }
        }
    }
    else
    {
        ;/* No suspended erase operation */
    }

    /* End of exclusive area */
    SchM_Exit_Mem_43_INFLS_MEM_EXCLUSIVE_AREA_07();

    return eRetVal;
}

/**
 * Function Name : Ftfc_Mem_InFls_Ip_CtrlStatus
 * Description   : Get the status of job with main interface.
 */
Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_CtrlStatus(void)
{
    Ftfc_Mem_InFls_Ip_StatusType eRetVal;
    uint8 errorFlags;

    MCAL_FAULT_INJECTION_POINT(FLS_FIP_FTFC_ERROR_READ_FSTAT_FLAG);

    if (0U != ((FTFx_BaseAddress->FCNFG) & FTFx_FCNFG_ERSSUSP_MASK))
    {
        /* Flash controller is suspend */
        eRetVal = STATUS_FTFC_MEM_INFLS_IP_BUSY;
    }
    else if (0U != (FTFx_BaseAddress->FSTAT & FTFx_FSTAT_CCIF_MASK))
    {
        /* OK, memory controller is idle */

        errorFlags = FTFx_BaseAddress->FSTAT & (FTFx_ERR_FLAGS_MASK);
        if (0U == errorFlags)
        {
            eRetVal = STATUS_FTFC_MEM_INFLS_IP_SUCCESS;
        }
        else
        {
            /* Hardware errors happened => Clear the error flags, return error */
            FTFx_BaseAddress->FSTAT = errorFlags;
            eRetVal = STATUS_FTFC_MEM_INFLS_IP_ERROR;
        }
    }
    else
    {
#if (STD_ON == FTFC_TIMEOUT_SUPERVISION_ENABLED)
        Ftfc_Mem_InFls_Ip_u32ElapsedTicks += OsIf_GetElapsed(&Ftfc_Mem_InFls_Ip_u32CurrentTicks, (OsIf_CounterType)FTFC_MEM_INFLS_IP_TIMEOUT_TYPE);
        if (Ftfc_Mem_InFls_Ip_u32ElapsedTicks >= Ftfc_Mem_InFls_Ip_u32TimeoutTicks)
        {
            /* Errors timeout because wait for the Done bit long time */
            eRetVal = STATUS_FTFC_MEM_INFLS_IP_ERROR_TIMEOUT;
        }
        else
#endif
        {
            eRetVal = STATUS_FTFC_MEM_INFLS_IP_BUSY;
        }
    }

    return eRetVal;
}


/**
 * Function Name : Ftfc_Mem_InFls_Ip_ControllerBusy
 * Description   : Check if the flash controller is busy or not
 */
static Ftfc_Mem_InFls_Ip_StatusType Ftfc_Mem_InFls_Ip_ControllerBusy(void)
{
    Ftfc_Mem_InFls_Ip_StatusType eRetVal;

    /* Check if the flash controller is idle, by polling the CCIF status bit. */
    if (0U != (FTFx_BaseAddress->FSTAT & FTFx_FSTAT_CCIF_MASK))
    {
        /* CCIF = 1, memory controller is idle */
        eRetVal = STATUS_FTFC_MEM_INFLS_IP_SUCCESS;
    }
    else
    {
        /* Controller is busy */
        eRetVal = STATUS_FTFC_MEM_INFLS_IP_BUSY;
    }

    return eRetVal;
}

/**
 * Function Name : Ftfc_Mem_InFls_Ip_ClearErrorFlags
 * Description   : Clear all error flags before launching a new command
 */
static void Ftfc_Mem_InFls_Ip_ClearErrorFlags(void)
{
    FTFx_BaseAddress->FSTAT = (uint8)(FTFx_FSTAT_FPVIOL_MASK | FTFx_FSTAT_ACCERR_MASK | FTFx_FSTAT_RDCOLERR_MASK);
}



static void Ftfc_Mem_InFls_Ip_LoadFCCOBParams(const uint32 u32PhysicAddr,
                                        const uint8 * pDataAddr,
                                        const uint8 u8FCCOBCmdId
                                       )
{
    uint32 u32FTFEAddr = 0UL;

    /* convert system memory map address to FTFE module address */
    u32FTFEAddr = Ftfc_Mem_InFls_Ip_ConvertSysToFTFEAddr(u32PhysicAddr);

    FTFx_BaseAddress->FCCOB[3] = u8FCCOBCmdId;                      /* Erase Flash Sector */
    FTFx_BaseAddress->FCCOB[2] = (uint8)(u32FTFEAddr >> 16UL);      /* Flash address [23:16] */
    FTFx_BaseAddress->FCCOB[1] = (uint8)(u32FTFEAddr >> 8UL);       /* Flash address [15:8] */
    FTFx_BaseAddress->FCCOB[0] = (uint8)(u32FTFEAddr >> 0UL);       /* Flash address [7:0] */

    if (NULL_PTR != pDataAddr)
    {
        /* load the first word */
        FTFx_BaseAddress->FCCOB[7] = pDataAddr[3];
        FTFx_BaseAddress->FCCOB[6] = pDataAddr[2];
        FTFx_BaseAddress->FCCOB[5] = pDataAddr[1];
        FTFx_BaseAddress->FCCOB[4] = pDataAddr[0];

        /* load the second word */
        FTFx_BaseAddress->FCCOB[11] = pDataAddr[7];
        FTFx_BaseAddress->FCCOB[10] = pDataAddr[6];
        FTFx_BaseAddress->FCCOB[9]  = pDataAddr[5];
        FTFx_BaseAddress->FCCOB[8]  = pDataAddr[4];
    }
}

/**
* @brief            Convert system memory map address to FTFE internal memory address
*
* @details          FTFE module requires flash address in 24bit format. Address passed to FTFE module
*                   via FCCOB command object is composed of sector offset(reported to D-FLASH or P-FLASH
*                   base address) plus the MSB bit set for DATA flash sectors and cleared for
*                   PROGRAM flash sectors.
*
* @param[in]        u32SystemAddress Physical flash address in system memory map
*
* @return           u32FTFEAddr    FTFE internal memory address
*/
static uint32 Ftfc_Mem_InFls_Ip_ConvertSysToFTFEAddr(uint32 u32SystemAddress)
{
    uint32 u32FTFEAddr = 0x0UL;

    /* check if address is a valid flash address and calculate FTFE internal memory addr */
    if (u32SystemAddress < FTFC_MEM_INFLS_IP_P_FLASH_SIZE)
    {
        /* In P_FLASH range, valid P_FLASH addr. FTFC_MEM_INFLS_IP_P_FLASH_BASE_ADDR = 0UL */
        u32FTFEAddr = u32SystemAddress;
    }
    else if ((u32SystemAddress >= FTFC_MEM_INFLS_IP_D_FLASH_BASE_ADDR) && (u32SystemAddress < (FTFC_MEM_INFLS_IP_D_FLASH_BASE_ADDR + FTFC_MEM_INFLS_IP_D_FLASH_SIZE)))
    {
        /* In D_FLASH range, valid D_FLASH addr */
        /* set the MSB of 24-bit address to distinguish between P_FLASH and D_FLASH */
        u32FTFEAddr = (u32SystemAddress - FTFC_MEM_INFLS_IP_D_FLASH_BASE_ADDR) + 0x800000UL;
    }
    else
    {
        /* invalid flash address */
    }

    return u32FTFEAddr;
}

/**
 * Function Name    Ftfc_Mem_InFls_Ip_GetCodeBlockNumber
 * Description      Get code block number from address.
 */
static Ftfc_Mem_InFls_Ip_FlashBlocksNumberType Ftfc_Mem_InFls_Ip_GetCodeBlockNumber(uint32 TargetAddress)
{
    Ftfc_Mem_InFls_Ip_FlashBlocksNumberType BlockNumber;

    if (TargetAddress <= (uint32)FTFC_MEM_INLS_IP_CODE_BLOCK_0_END_ADDR)
    {
        /* The address is in the code block 0 */
        BlockNumber = FTFC_MEM_INFLS_IP_CODE_BLOCK_0;
    }
#ifdef FTFC_MEM_INLS_IP_CODE_BLOCK_1_BASE_ADDR
    else if (TargetAddress <= (uint32)FTFC_MEM_INLS_IP_CODE_BLOCK_1_END_ADDR)
    {
        /* The address is in the code block 1 */
        BlockNumber = FTFC_MEM_INFLS_IP_CODE_BLOCK_1;
    }
#endif
#ifdef FTFC_MEM_INLS_IP_CODE_BLOCK_2_BASE_ADDR
    else if (TargetAddress <= (uint32)FTFC_MEM_INLS_IP_CODE_BLOCK_2_END_ADDR)
    {
        /* The address is in the code block 2 */
        BlockNumber = FTFC_MEM_INFLS_IP_CODE_BLOCK_2;
    }
#endif
    else
    {
        BlockNumber = FTFC_MEM_INFLS_IP_BLOCK_INVALID;
    }
    return BlockNumber;
}
 /*
 * Function Name : Ftfc_Mem_InFls_Ip_GetBlockNumberFromAddress
 * Description   : Get block number from address.
 */
Ftfc_Mem_InFls_Ip_FlashBlocksNumberType Ftfc_Mem_InFls_Ip_GetBlockNumberFromAddress(uint32 u32TargetAddress)
{
    Ftfc_Mem_InFls_Ip_FlashBlocksNumberType blockNumber;

    if ((u32TargetAddress >= (uint32)(FTFC_MEM_INFLS_IP_D_FLASH_BASE_ADDR)) && (u32TargetAddress < (uint32)(FTFC_MEM_INFLS_IP_D_FLASH_BASE_ADDR + FTFC_MEM_INFLS_IP_D_FLASH_SIZE)))
    {
        /* The address is in the data */
        blockNumber = FTFC_MEM_INFLS_IP_DATA_BLOCK;
    }
    else if (u32TargetAddress < (uint32)(FTFC_MEM_INFLS_IP_P_FLASH_BASE_ADDR + FTFC_MEM_INFLS_IP_P_FLASH_SIZE)) /* FTFC_MEM_INFLS_IP_P_FLASH_BASE_ADDR = 0UL on all derivatives */
    {
        /* The address is in the code block */
        blockNumber = Ftfc_Mem_InFls_Ip_GetCodeBlockNumber(u32TargetAddress);
    }
    else
    {
        /* Out of bounds address */
        blockNumber = FTFC_MEM_INFLS_IP_BLOCK_INVALID;
    }

    return blockNumber;
}

void Ftfc_Mem_InFls_Ip_ReportEccUnCorrectedError(void)
{
    /* save read status */
    Ftfc_Mem_InFls_Ip_eReadStatus = STATUS_FTFC_MEM_INFLS_IP_ECC_UNCORRECTED;
}

/**
 * Function Name : Ftfc_Mem_InFls_Ip_InvalidPrefetchBuff
 * Description   : Invalidate prefetch buffer before reading to make sure that the driver always reads the new data from flash
 */
static void Ftfc_Mem_InFls_Ip_InvalidPrefetchBuff(void)
{
    /* start critical section: implementation depends on integrator */
    Ftfc_Mem_InFls_Ip_FlashAccessCalloutStart();

    /* Call the function allocated on RAM */
#if (STD_ON == FTFC_MEM_INFLS_IP_SUPPORT_USER_MODE)
    OsIf_Trusted_Call(Ftfc_Mem_InFls_Ip_InvalidPrefetchBuff_Ram);
#else
    Ftfc_Mem_InFls_Ip_InvalidPrefetchBuff_Ram();
#endif

    /* end flash access section: implementation depends on integrator */
    Ftfc_Mem_InFls_Ip_FlashAccessCalloutFinish();
}


/**
 * Function Name : Ftfc_Mem_InFls_Ip_SetLoadAcStatus
 * Description   : Set Status for Ftfc_Mem_InFls_Ip_bAsync
 */
void Ftfc_Mem_InFls_Ip_SetAsyncMode(const boolean bAsync)
{
    Ftfc_Mem_InFls_Ip_bAsync = bAsync;
}


/**
 * Function Name : Ftfc_Mem_InFls_Ip_FlashAccessCalloutStart
 * Description   : Call the callout function if it was configured
 */
static void Ftfc_Mem_InFls_Ip_FlashAccessCalloutStart(void)
{
    if (NULL_PTR != Ftfc_Mem_InFls_Ip_pConfigPtr->StartFlashAccessNotifPtr)
    {
        Ftfc_Mem_InFls_Ip_pConfigPtr->StartFlashAccessNotifPtr();
    }
}


/**
 * Function Name : Ftfc_Mem_InFls_Ip_FlashAccessCalloutFinish
 * Description   : Call the callout function if it was configured
 */
static void Ftfc_Mem_InFls_Ip_FlashAccessCalloutFinish(void)
{
    if (NULL_PTR != Ftfc_Mem_InFls_Ip_pConfigPtr->FinishedFlashAccessNotifPtr)
    {
        Ftfc_Mem_InFls_Ip_pConfigPtr->FinishedFlashAccessNotifPtr();
    }
}


#define MEM_43_INFLS_STOP_SEC_CODE
#include "Mem_43_INFLS_MemMap.h"


#if (FTFC_MEM_INFLS_IP_INVALID_PREBUF_FROM_RAM == STD_ON)
#define MEM_43_INFLS_START_SEC_RAMCODE
#else
#define MEM_43_INFLS_START_SEC_CODE
#endif
#include "Mem_43_INFLS_MemMap.h"

/**
 * Function Name : Ftfc_Mem_InFls_Ip_InvalidPrefetchBuff_Ram
 * Description   : Invalidate prefetch buffer to make sure that the driver always reads the new data from flash
 */
void Ftfc_Mem_InFls_Ip_InvalidPrefetchBuff_Ram(void)
{
    volatile uint32 u32RegMscmOcmdr;
    /* Check the status of the prefetch buffers, and invalidate them only if they are enabled.*/

    u32RegMscmOcmdr = IP_MSCM->OCMDR[0];
    if (0U == (u32RegMscmOcmdr & MSCM_OCMDR_OCM1(2)))              /* If prefetch is enabled for program flash, OCMDR0[5] == 0 */
    {
        IP_MSCM->OCMDR[0] = u32RegMscmOcmdr | MSCM_OCMDR_OCM1(2);  /* Set OCMDR0[5]. Disable PF for program flash */

        IP_MSCM->OCMDR[0] = u32RegMscmOcmdr;                       /* Clear OCMDR0[5]. Enable PF for program flash */
    }

    u32RegMscmOcmdr = IP_MSCM->OCMDR[1];
    if (0U == (u32RegMscmOcmdr & MSCM_OCMDR_OCM1(2)))              /* If prefetch is enabled for data flash, OCMDR1[5] == 0 */
    {
        IP_MSCM->OCMDR[1] = u32RegMscmOcmdr | MSCM_OCMDR_OCM1(2);  /* Set OCMDR0[5]. Disable PF for data flash */

        IP_MSCM->OCMDR[1] = u32RegMscmOcmdr;                       /* Clear OCMDR0[5]. Enable PF for data flash */
    }
}

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
