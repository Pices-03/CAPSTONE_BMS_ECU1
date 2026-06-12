/**
 * @file       BmsScheduler.h
 * @version    1.0.0
 * @brief      Cooperative timer-driven scheduler for the BMS application.
 *
 * @details    Owns LPIT_0 channel 0 (via the AUTOSAR Gpt MCAL) as the system
 *             tick source. The Gpt notification callback BmsScheduler_Tick()
 *             is invoked from interrupt context every 50 ms and only sets a
 *             volatile flag; BmsScheduler_Run() polls that flag from the
 *             super-loop in main() and dispatches application tasks at the
 *             rates documented in section 5 of project_context.md:
 *                  every  50 ms : BmsFault_Process(), BmsApp_Task50ms()
 *                  every 100 ms : BmsApp_Task100ms()
 *                  every 200 ms : BmsApp_Task200ms()
 *                  every 500 ms : BmsApp_Task500ms()
 *
 * @note       Replaces the busy-wait DELAY_COUNT_50MS that was used in the
 *             original main.c (backlog item #10).
 */

#ifndef _BMSSCHEDULER_
#define _BMSSCHEDULER_

#ifdef __cplusplus
extern "C" {
#endif

#include "Std_Types.h"

/*******************************************************************************
* Definitions
*******************************************************************************/

/**
 * @brief   Tick period in milliseconds.
 */
#define BMS_SCHED_TICK_PERIOD_MS     (50U)

/**
 * @brief   Gpt timer tick frequency (FIRC 48 MHz).
 */
#define BMS_SCHED_GPT_TICK_HZ        (48000000U)

/**
 * @brief   Gpt timer ticks per scheduler period: 48 MHz * 50 ms = 2 400 000.
 */
#define BMS_SCHED_GPT_TICKS          (2400000U)

/*******************************************************************************
* API
*******************************************************************************/

/**
 * @brief   Initialize the BMS scheduler and start the system tick.
 *
 * @details Calls Gpt_Init(), enables the LPIT_0 CH_0 notification, and
 *          starts the timer with BMS_SCHED_GPT_TICKS so that
 *          BmsScheduler_Tick() fires every 50 ms.
 *
 * @pre     Mcu/Port/I2c/Can have been initialised. CDD modules have been
 *          initialised. Gpt_Config from generate/ is linked in.
 *
 * @param   void
 * @return  void
 *
 * @post    LPIT_0 CH_0 is running. The scheduler tick flag is FALSE.
 *          The scheduler tick counter is 0.
 */
void BmsScheduler_Init(void);

/**
 * @brief   Run one iteration of the cooperative scheduler.
 *
 * @details Returns immediately if the tick flag is FALSE. Otherwise clears
 *          the flag, increments the tick counter, and dispatches the matching
 *          application tasks. Designed to be called in a tight super-loop;
 *          MCU is free to spin between ticks (or be put to WFI in future).
 *
 * @pre     BmsScheduler_Init() has been called.
 *
 * @param   void
 * @return  void
 *
 * @post    On a tick: tick flag cleared, counter incremented, all tasks for
 *          this slot have completed.
 *          Off a tick: no state change.
 */
void BmsScheduler_Run(void);

/**
 * @brief   Gpt notification callback (referenced from generate/Gpt_PBcfg.c).
 *
 * @details Called from LPIT_0 CH_0 ISR every BMS_SCHED_TICK_PERIOD_MS ms.
 *          Sets the internal tick flag to TRUE. Keeps interrupt time near zero.
 *
 * @param   void
 * @return  void
 *
 * @post    Internal s_tick50msFlag = TRUE.
 *
 * @note    Called from interrupt context. Must not block. Must not call
 *          functions that take long-lived locks.
 */
void BmsScheduler_Tick(void);

#ifdef __cplusplus
}
#endif

#endif /* _BMSSCHEDULER_ */

