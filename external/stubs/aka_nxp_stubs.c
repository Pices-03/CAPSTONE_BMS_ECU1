/*
 * aka_nxp_stubs.c
 * -----------------
 * Empty implementations of NXP RTD driver functions, used by AKAUTAUTO
 * to satisfy the linker when running unit tests on PC (MinGW gcc) without
 * compiling the actual driver code (which contains ARM-specific register
 * access and inline assembly).
 *
 * NOT used by the real S32DS embedded build.
 */

#include "Std_Types.h"
#include "Can_43_FLEXCAN.h"
#include "Gpt.h"
#include "Mem_43_INFLS.h"
#include "CDD_I2c.h"
#include "Mcu.h"
#include "Port.h"
#include "Platform.h"

/* ---------- Config storage (empty) ---------- */
const Can_43_FLEXCAN_ConfigType Can_43_FLEXCAN_Config;
const Gpt_ConfigType            Gpt_Config;
const I2c_ConfigType            I2c_Config;
const Mcu_ConfigType            Mcu_Config;

/* ---------- CAN ---------- */
void Can_43_FLEXCAN_Init(const Can_43_FLEXCAN_ConfigType *Config)
{
    (void)Config;
}

Std_ReturnType Can_43_FLEXCAN_SetControllerMode(uint8 Controller,
                                                 Can_ControllerStateType Transition)
{
    (void)Controller;
    (void)Transition;
    return (Std_ReturnType)0; /* E_OK */
}

/* AKAUT test hook: set this from a test case to force a specific return.
 * Default 0 = E_OK so existing tests continue to pass. */
Std_ReturnType g_canWriteRet = (Std_ReturnType)0;
uint32         g_canWriteCallCnt = 0U;

Std_ReturnType Can_43_FLEXCAN_Write(Can_HwHandleType Hth, const Can_PduType *PduInfo)
{
    (void)Hth;
    (void)PduInfo;
    g_canWriteCallCnt++;
    return g_canWriteRet;
}

void Can_43_FLEXCAN_MainFunction_Write(void) {}
void Can_43_FLEXCAN_MainFunction_Read(void) {}

/* ---------- GPT ---------- */
void Gpt_Init(const Gpt_ConfigType *configPtr)
{
    (void)configPtr;
}

void Gpt_EnableNotification(Gpt_ChannelType channel)
{
    (void)channel;
}

void Gpt_StartTimer(Gpt_ChannelType channel, Gpt_ValueType value)
{
    (void)channel;
    (void)value;
}

/* AKAUT hook: set g_gptTimeElapsed to control return value */
uint32 g_gptTimeElapsed = 0U;

Gpt_ValueType Gpt_GetTimeElapsed(Gpt_ChannelType channel)
{
    (void)channel;
    return (Gpt_ValueType)g_gptTimeElapsed;
}

/* ---------- MEM (Flash) ---------- */
void Mem_43_INFLS_Init(const Mem_43_INFLS_ConfigType *ConfigPtr)
{
    (void)ConfigPtr;
}

/* AKAUT hooks for Mem_43_INFLS_Read:
 *   g_memReadRet         : return value
 *   g_memReadInjectFloat : if non-zero, write g_memReadFloat to DestinationDataPtr
 *   g_memReadFloat       : float value to inject into rxBuffer
 */
uint8   g_memReadRet         = 0U;
uint8   g_memReadInjectFloat = 0U;
float32 g_memReadFloat       = 50.0f;

Std_ReturnType Mem_43_INFLS_Read(Mem_43_INFLS_InstanceIdType InstanceId,
                                  Mem_43_INFLS_AddressType    SourceAddress,
                                  Mem_43_INFLS_DataType      *DestinationDataPtr,
                                  Mem_43_INFLS_LengthType     Length)
{
    (void)InstanceId; (void)SourceAddress;
    if ((DestinationDataPtr != NULL_PTR) && (g_memReadInjectFloat != 0U) && (Length >= 4U))
    {
        *((float32*)DestinationDataPtr) = g_memReadFloat;
    }
    return (Std_ReturnType)g_memReadRet;
}

/* AKAUT Mem hooks - declared as uint8 so test drivers don't need Mem types */
uint8 g_memWriteRet  = 0U;   /* E_OK */
uint8 g_memEraseRet  = 0U;
uint8 g_memJobResult = 0U;   /* MEM_43_INFLS_JOB_OK */

Std_ReturnType Mem_43_INFLS_Write(Mem_43_INFLS_InstanceIdType  InstanceId,
                                   Mem_43_INFLS_AddressType     TargetAddress,
                                   const Mem_43_INFLS_DataType *SourceDataPtr,
                                   Mem_43_INFLS_LengthType      Length)
{
    (void)InstanceId; (void)TargetAddress; (void)SourceDataPtr; (void)Length;
    return (Std_ReturnType)g_memWriteRet;
}

Std_ReturnType Mem_43_INFLS_Erase(Mem_43_INFLS_InstanceIdType InstanceId,
                                   Mem_43_INFLS_AddressType    TargetAddress,
                                   Mem_43_INFLS_LengthType     Length)
{
    (void)InstanceId; (void)TargetAddress; (void)Length;
    return (Std_ReturnType)g_memEraseRet;
}

void Mem_43_INFLS_MainFunction(void) {}

Mem_43_INFLS_JobResultType Mem_43_INFLS_GetJobResult(Mem_43_INFLS_InstanceIdType InstanceId)
{
    (void)InstanceId;
    return (Mem_43_INFLS_JobResultType)g_memJobResult;
}

/* ---------- I2C ---------- */
/* AKAUT test hooks for I2c:
 *   g_i2cAsyncRet    : return value of I2c_AsyncTransmit (default E_OK)
 *   g_i2cStatusRet   : return value of I2c_GetStatus (default 0 = IDLE)
 *                       set to 2 (I2C_CH_RECEIVE) or 1 (I2C_CH_SEND) to force timeout
 *   g_i2cRxInject[]  : bytes to copy into the request DataBuffer when DataDirection
 *                       == I2C_RECEIVE_DATA. Set g_i2cRxInjectLen = 0 to disable.
 */
#define AKA_I2C_INJECT_MAX  16U
/* Note: hooks declared as uint8 (not Std_ReturnType / I2c_StatusType) so test
 * drivers don't need to include CDD_I2c_Types.h or Std_Types.h to use them. */
uint8  g_i2cAsyncRet      = 0U;
uint8  g_i2cStatusRet     = 0U;
uint8  g_i2cRxInject[AKA_I2C_INJECT_MAX] = {0};
uint8  g_i2cRxInjectLen   = 0U;

void I2c_Init(const I2c_ConfigType *Config)
{
    (void)Config;
}

Std_ReturnType I2c_AsyncTransmit(uint8 Channel, const I2c_RequestType *Request)
{
    uint8 i = 0U;
    uint8 n = 0U;
    (void)Channel;

    if ((Request != NULL_PTR) &&
        (Request->DataDirection == I2C_RECEIVE_DATA) &&
        (Request->DataBuffer != NULL_PTR) &&
        (g_i2cRxInjectLen > 0U))
    {
        n = (Request->BufferSize < g_i2cRxInjectLen) ?
                Request->BufferSize : g_i2cRxInjectLen;
        for (i = 0U; i < n; i++)
        {
            Request->DataBuffer[i] = g_i2cRxInject[i];
        }
    }
    return (Std_ReturnType)g_i2cAsyncRet;
}

I2c_StatusType I2c_GetStatus(uint8 Channel)
{
    (void)Channel;
    return (I2c_StatusType)g_i2cStatusRet;
}

/* ---------- MCU ---------- */
void Mcu_Init(const Mcu_ConfigType *ConfigPtr)
{
    (void)ConfigPtr;
}

Std_ReturnType Mcu_InitClock(Mcu_ClockType ClockSetting)
{
    (void)ClockSetting;
    return (Std_ReturnType)0;
}

void Mcu_SetMode(Mcu_ModeType McuMode)
{
    (void)McuMode;
}

/* ---------- PORT ---------- */
void Port_Init(const Port_ConfigType *ConfigPtr)
{
    (void)ConfigPtr;
}

/* ---------- PLATFORM ---------- */
void Platform_Init(const Platform_ConfigType *pConfig)
{
    (void)pConfig;
}
