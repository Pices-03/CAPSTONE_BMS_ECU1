/**
 * @file       main.c
 * @version    1.0.0
 * @brief      Boot entry point for the BMS firmware.
 *
 * @details    Initialises MCU clocks, ports, peripherals, CDDs and the BMS
 *             application, then enters the cooperative scheduler super-loop.
 *             All real work lives under Application/ and CDD/ -- this file
 *             is intentionally thin.
 *
 *             Initialisation order follows AUTOSAR Classic conventions:
 *               1. Mcu  / clock
 *               2. Port / Platform
 *               3. I2c  (LPI2C0) -- needed by CDDs
 *               4. INA219 / STM32Temp CDD init
 *               5. FlexCAN init + start
 *               6. BmsApp init (clears SDUs)
 *               7. BmsScheduler init (starts LPIT_0 CH_0 -- last so the first
 *                  tick has a fully initialised system to dispatch into)
 */

/*******************************************************************************
* Includes
*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include "Mcu.h"
#include "Mcal.h"
#include "Platform.h"
#include "Port.h"
#include "CDD_I2c.h"
#include "Can_43_FLEXCAN.h"
#include "SchM_Can_43_FLEXCAN.h"

#include "CDD_INA219.h"
#include "CDD_STM32Temp.h"
#include "BmsApp.h"
#include "BmsFault.h"
#include "BmsScheduler.h"

/*******************************************************************************
* Code
*******************************************************************************/

/**
 * @brief   Firmware entry point.
 *
 * @return  Never returns; while(1) loop drives the scheduler.
 */
int main(void)
{
    /* ---- 1. MCU & clock ---------------------------------------------- */
#if (MCU_PRECOMPILE_SUPPORT == STD_ON)
    Mcu_Init(NULL_PTR);
#else
    Mcu_Init(&Mcu_Config);
#endif

    Mcu_InitClock(McuClockSettingConfig_0);

#if (MCU_NO_PLL == STD_OFF)
    while (MCU_PLL_LOCKED != Mcu_GetPllStatus())
    {
        /* Wait for PLL lock */
    }
    Mcu_DistributePllClock();
#endif

    Mcu_SetMode(McuModeSettingConf_0);

    /* ---- 2. Port & Platform ----------------------------------------- */
    Port_Init(NULL_PTR);
    Platform_Init(NULL_PTR);

    /* ---- 3. I2C (LPI2C0) -------------------------------------------- */
#if (I2C_PRECOMPILE_SUPPORT == STD_ON)
    I2c_Init(NULL_PTR);
#else
    I2c_Init(&I2c_Config);
#endif

    /* ---- 4. CDDs that depend on I2C --------------------------------- */
    CDD_INA219_Init();
    CDD_STM32Temp_Init();

    /* ---- 5. FlexCAN ------------------------------------------------- */
#if (CAN_43_FLEXCAN_PRECOMPILE_SUPPORT == STD_ON)
    Can_43_FLEXCAN_Init(NULL_PTR);
#else
    Can_43_FLEXCAN_Init(&Can_43_FLEXCAN_Config);
#endif

    Can_43_FLEXCAN_SetControllerMode(
        Can_43_FLEXCANConf_CanController_CanController_0,
        CAN_CS_STARTED);

    /* ---- 6. Application state --------------------------------------- */
    BmsFault_Init();
    BmsApp_Init();

    /* ---- 7. Scheduler (last -- starts the system tick) -------------- */
    BmsScheduler_Init();

    /* ---- Super-loop ------------------------------------------------- */
    while (1)
    {
        BmsScheduler_Run();
    }

    /* unreachable */
    /* coverity[unreachable_code] */
    return 0;
}

#ifdef __cplusplus
}
#endif
