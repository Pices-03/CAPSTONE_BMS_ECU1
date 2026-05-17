/**
 * @file       BmsFault.h
 * @version    2.0.0
 * @brief      Fault detection, latching and CAN dispatch (BMS_Fault 0x101).
 *
 * @details    v2 redesign -- STATE-BASED instead of event-based:
 *               - Each fault source (OV, OC, OT, INA_COMM, STM_COMM) has its
 *                 own latch. The latch is set TRUE when the condition is
 *                 violated and FALSE when it normalises.
 *               - BmsFault_Process() runs every 50 ms, picks the highest
 *                 priority active fault, and transmits CAN 0x101 ONLY when
 *                 the chosen state changes (rising AND falling edges).
 *               - Falling edge (last active fault clears) sends a 0x101 with
 *                 source = BMS_FAULT_SRC_NONE so the HMI can clear its banner
 *                 automatically. Previously the HMI banner stayed red forever
 *                 because firmware never told it the fault was gone.
 *
 *             CAN 0x101 payload layout (4 bytes):
 *               B0 : fault source (BMS_FAULT_SRC_xxx, 0 = no active fault)
 *               B1 : severity     (BMS_FAULT_SEV_xxx)
 *               B2 : fault value MSB
 *               B3 : fault value LSB
 *
 *             Latches and the TX-edge buffer are written from both task
 *             context (Check / SetCommError / Reset) and ISR context (RX
 *             callback may call Reset), so every access is wrapped in
 *             OsIf_SuspendAllInterrupts() / OsIf_ResumeAllInterrupts().
 *
 * @note       Replaces v1 single-pending-flag model which leaked stale
 *             fault data into 0x101 byte 2-3 (backlog #22 root cause).
 */

#ifndef _BMSFAULT_H_
#define _BMSFAULT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Std_Types.h"

/*******************************************************************************
* Definitions
*******************************************************************************/

/**
 * @brief   BMS_Fault DLC (bytes).
 */
#define BMS_FAULT_DLC                (4U)

/**
 * @brief   Fault source codes (CAN 0x101 byte 0).
 * @details NEW (v2): 0x04 / 0x05 are dedicated comm-error codes. Previously
 *          all I2C errors were folded into 0x03 (OT) which made the HMI
 *          show a temperature warning every time an INA219 / STM32 read
 *          failed. Now 0x03 is pure overtemperature only.
 */
#define BMS_FAULT_SRC_NONE           (0x00U)  /**< No active fault.                 */
#define BMS_FAULT_SRC_OV             (0x01U)  /**< Overvoltage (pack > threshold).  */
#define BMS_FAULT_SRC_OC             (0x02U)  /**< Overcurrent.                     */
#define BMS_FAULT_SRC_OT             (0x03U)  /**< Overtemperature -- ONLY real T.  */
#define BMS_FAULT_SRC_INA_COMM       (0x04U)  /**< INA219 I2C read error.           */
#define BMS_FAULT_SRC_STM_COMM       (0x05U)  /**< STM32 temperature I2C error.     */

/**
 * @brief   Fault severity codes (CAN 0x101 byte 1).
 */
#define BMS_FAULT_SEV_WARNING        (0x00U)  /**< Warning -- inform but do not act.*/
#define BMS_FAULT_SEV_PROTECTION     (0x01U)  /**< Protection -- isolate / cut off. */

/**
 * @brief   Protection thresholds.
 *
 * @details Demo setup = 1S Li-ion (single cell) running off a 5 V resistor
 *          rig until a real cell is wired in, so the OV threshold is
 *          relaxed to 5 500 mV. This both:
 *            - tolerates the 4.7-5.2 V PSU sag during demo, AND
 *            - is still safely above 4.2 V Li-ion full-charge if a real
 *              1S cell replaces the rig later.
 *
 *          When migrating to a strict-protection deployment, set
 *          BMS_FAULT_OV_THRESH_MV to 4200U (4.2 V cell, 1S) or to
 *          12600U for a 3S pack.
 *
 *          OC : 3000 mA  (well above demo currents ~150 mA, leaves room
 *                          for real cell discharge bursts).
 *          OT : 55 deg C (warning level for Li-ion cells).
 */
#define BMS_FAULT_OV_THRESH_MV       (5500U)
#define BMS_FAULT_OC_THRESH_MA       (3000)
#define BMS_FAULT_OT_THRESH_C        (55.0f)

/**
 * @brief   Overtemperature threshold expressed in raw temp encoding (integer).
 * @details temp_raw = (temp_C + 40) / 0.5
 *          55 degC -> raw = (55 + 40) / 0.5 = 190
 *          Cho phép so sánh integer thuần trên safety path, không cần float.
 */
#define BMS_FAULT_OT_THRESH_RAW      (190U)

/*******************************************************************************
* API
*******************************************************************************/

/**
 * @brief   Initialise the fault module (clear all latches, blank SDU).
 *
 * @param   void
 * @return  void
 *
 * @post    All per-source latches set FALSE.
 *          No 0x101 transmitted from Init -- BmsFault_Process will emit a
 *          "no fault" frame on the first call to advertise the clean state.
 */
void BmsFault_Init(void);

/**
 * @brief   Run the fault state machine: pick highest active latch and TX
 *          CAN 0x101 if the (source, value) pair changed.
 *
 * @details Priority order (highest first): OV, OC, OT, INA_COMM, STM_COMM.
 *          When NO latch is active, the chosen source is BMS_FAULT_SRC_NONE
 *          and a clear frame is emitted exactly once (on the falling edge).
 *
 *          Called by BmsScheduler_Run() at the start of every 50 ms slot
 *          (highest priority dispatch).
 *
 * @pre     Can controller is in CAN_CS_STARTED state.
 *
 * @param   void
 * @return  void
 *
 * @post    If chosen state changed since last TX: 0x101 enqueued and the
 *          last-TX buffer is updated.
 *          Otherwise no CAN traffic generated.
 */
void BmsFault_Process(void);

/**
 * @brief   Evaluate electrical limits and update OV / OC latches accordingly.
 *
 * @details Called from BmsApp_Task50ms() right after INA219 has been read.
 *          Each latch is set or cleared based on the current reading -- so a
 *          transient overvoltage will auto-clear on the next tick when the
 *          measurement returns below threshold.
 *
 * @param[in] volt_mV   Bus voltage in millivolts.
 * @param[in] curr_raw  Current encoded as 0.1 mA LSB (signed).
 *
 * @return  void
 */
void BmsFault_CheckElec(uint16 volt_mV, sint16 curr_raw);

/**
 * @brief   Evaluate thermal limit and update OT latch accordingly.
 *
 * @details Called from BmsApp_Task200ms() after a successful STM32 read.
 *          Latch is FALSE whenever temp_C <= BMS_FAULT_OT_THRESH_C, so as
 *          soon as the sensor cools below 55 degC the HMI banner clears.
 *
 * @param[in] temp_C    Temperature in degrees C.
 * @param[in] temp_raw  Encoded raw value (0.5 deg C LSB, offset -40).
 *
 * @return  void
 */
void BmsFault_CheckTemp(float32 temp_C, uint8 temp_raw);

/**
 * @brief   Set or clear a sensor-comm fault latch.
 *
 * @details Replaces v1 `BmsFault_RaiseCommError`. The `active` argument lets
 *          the caller express both directions in one call:
 *
 *              BmsFault_SetCommError(SRC_INA_COMM, TRUE,  errCode);   // raise
 *              BmsFault_SetCommError(SRC_INA_COMM, FALSE, 0U);        // clear
 *
 *          Only BMS_FAULT_SRC_INA_COMM and BMS_FAULT_SRC_STM_COMM are
 *          accepted; any other source is silently ignored to prevent
 *          comm-error frames from masquerading as OT.
 *
 * @param[in] source    Fault source code.
 * @param[in] active    TRUE  -> latch the fault.
 *                      FALSE -> clear the latch (must be paired with each
 *                               successful read so the banner clears).
 * @param[in] errCode   Driver-specific error code (placed in SDU byte 2-3).
 *                      Ignored when active == FALSE.
 *
 * @return  void
 */
void BmsFault_SetCommError(uint8 source, boolean active, uint8 errCode);

/**
 * @brief   Force-clear every latch (called by HMI command 0x710 / 0x01).
 *
 * @details Equivalent to BmsFault_Init() in effect but does not modify the
 *          TX edge buffer (so the falling edge is still detected and a
 *          clear frame is emitted next Process tick).
 *
 * @param   void
 * @return  void
 */
void BmsFault_Reset(void);

#ifdef __cplusplus
}
#endif

#endif /* _BMSFAULT_H_ */
