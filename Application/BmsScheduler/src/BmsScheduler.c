/**
 * @file       BmsScheduler.c
 * @version    1.0.0
 * @brief      Implementation of the BMS cooperative tick-driven scheduler.
 *
 * @details    Owns LPIT_0 CH_0 via the AUTOSAR Gpt MCAL. The notification
 *             callback only sets a volatile flag; the super-loop polls the
 *             flag and dispatches per-rate tasks. The scheduler also drives
 *             the CAN driver's MainFunction polling (TX/RX) and the priority
 *             dispatch of pending faults via BmsFault_Process().
 */

/*******************************************************************************
* Includes
*******************************************************************************/
#include "Std_Types.h"
#include "BmsScheduler.h"
#include "BmsFault.h"
#include "BmsApp.h"
#include "Gpt.h"
#include "Gpt_Cfg.h"
#include "Can_43_FLEXCAN.h"
#include "SchM_Can_43_FLEXCAN.h"     /* declares Can_43_FLEXCAN_MainFunction_Write/Read */
#include "BMS_Nvm.h"
/*******************************************************************************
* Definitions
*******************************************************************************/

/**
 * @brief   Modulo divisor for the 100 ms slot (every 2 ticks).
 */
#define BMS_SCHED_DIV_100MS          (2U)

/**
 * @brief   Modulo divisor for the 200 ms slot (every 4 ticks).
 */
#define BMS_SCHED_DIV_200MS          (4U)

/**
 * @brief   Modulo divisor for the 500 ms slot (every 10 ticks).
 */
#define BMS_SCHED_DIV_500MS          (10U)

/**
 * @brief   Gpt channel reference produced by the RTD configurator.
 */
#define BMS_SCHED_GPT_CHANNEL        (GptConf_GptChannelConfiguration_GptChannelConfiguration_0)

/*******************************************************************************
* Variables
*******************************************************************************/

/**
 * @brief   Tick flag set from BmsScheduler_Tick() (ISR), cleared from Run().
 * @details volatile because it crosses the ISR / task boundary. Single-byte
 *          access on Cortex-M is naturally atomic, so no critical section is
 *          needed for read-clear-set on a single boolean.
 */
static volatile boolean s_tick50msFlag = FALSE;

/**
 * @brief   Monotonic tick counter (mod BMS_SCHED_DIV_500MS).
 * @details Incremented from task context, never read from ISR.
 *          Marked volatile so debugger always sees the live value.
 */
static volatile uint32 s_tickCount = 0U;

/**
 * @brief   ISR-side tick counter -- DEBUG ONLY.
 * @details Incremented from BmsScheduler_Tick() (LPIT_0 CH_0 ISR) every 50 ms.
 *          Never decremented or wrapped (will roll over after ~6.8 years).
 *          Use in S32DS Expression view to confirm GPT/LPIT is actually
 *          firing -- if this stays 0 after pause, the timer never started.
 *          Compare against s_runCallCnt (Run-side) to detect Run loop hung.
 */
static volatile uint32 s_schedIsrTickCnt = 0U;

/**
 * @brief   Super-loop call counter -- DEBUG ONLY.
 * @details Incremented at top of BmsScheduler_Run() on every iteration of
 *          the main while(1). Should be MUCH larger than s_schedIsrTickCnt
 *          if everything is healthy (Run is called many times per tick).
 *          If 0 -> main super-loop never reached BmsScheduler_Run().
 */
static volatile uint32 s_runCallCnt = 0U;

/*******************************************************************************
* Code
*******************************************************************************/

/**
 * @brief   See BmsScheduler.h.
 */
void BmsScheduler_Init(void)
{
    s_tick50msFlag = FALSE;
    s_tickCount    = 0U;

#if (GPT_PRECOMPILE_SUPPORT == STD_ON)
    Gpt_Init(NULL_PTR);
#else
    Gpt_Init(&Gpt_Config);
#endif

    /*
     * Gpt_EnableNotification() sets MIER[TIE0] = 1 so the LPIT module
     * forwards the IRQ pulse to the NVIC. This call is only declared
     * when GPT_ENABLE_DISABLE_NOTIFICATION_API == STD_ON (see
     * generate/include/Gpt_Cfg.h, which mirrors the Gpt component's
     * "GptEnableDisableNotificationApi" setting in the .mex file).
     * The #if keeps this file portable if that config is ever flipped.
     */
#if (GPT_ENABLE_DISABLE_NOTIFICATION_API == STD_ON)
    Gpt_EnableNotification(BMS_SCHED_GPT_CHANNEL);
#endif

    Gpt_StartTimer(BMS_SCHED_GPT_CHANNEL, BMS_SCHED_GPT_TICKS);
	Gpt_StartTimer(GptConf_GptChannelConfiguration_GptChannelConfiguration_1, 0xFFFFFFFFU);

}

/**
 * @brief   See BmsScheduler.h.
 */
void BmsScheduler_Run(void)
{
	BMS_Nvm_MainFunction();
    /* Debug counter -- proves the super-loop reaches Run(). */
    s_runCallCnt++;

    /* Always poll CAN MainFunctions even between ticks so TX confirms /
     * RX indications fire promptly. */
    Can_43_FLEXCAN_MainFunction_Write();
    Can_43_FLEXCAN_MainFunction_Read();

    if (s_tick50msFlag == TRUE)
    {
        s_tick50msFlag = FALSE;
        s_tickCount++;

        /* Highest priority -- always first */
        BmsFault_Process();

        /* 50 ms slot */
        BmsApp_Task50ms();

        /* 100 ms slot */
        if ((s_tickCount % BMS_SCHED_DIV_100MS) == 0U)
        {
            BmsApp_Task100ms();
        }

        /* 200 ms slot */
        if ((s_tickCount % BMS_SCHED_DIV_200MS) == 0U)
        {
            BmsApp_Task200ms();
        }

        /* 500 ms slot -- also rolls the tick counter */
        if ((s_tickCount % BMS_SCHED_DIV_500MS) == 0U)
        {
            BmsApp_Task500ms();
            s_tickCount = 0U;
        }
    }
}

/**
 * @brief   See BmsScheduler.h.
 */
void BmsScheduler_Tick(void)
{
    s_schedIsrTickCnt++;
    s_tick50msFlag = TRUE;
}
