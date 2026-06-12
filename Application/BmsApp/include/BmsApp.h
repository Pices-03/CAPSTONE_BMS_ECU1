/**
 * @file       BmsApp.h
 * @version    1.0.0
 * @brief      Top-level BMS application: read sensors, pack signals, TX CAN.
 *
 * @details    Provides four scheduler entry points (50/100/200/500 ms) that
 *             the BmsScheduler dispatches at the right rate, plus the AUTOSAR
 *             CanIf callbacks (TxConfirmation, RxIndication, BusOff,
 *             ControllerModeIndication) and the I2c callback hook.
 *
 *             The pack functions are public so that test code (V-model
 *             section 12) can verify byte-by-byte signal layout without
 *             running the scheduler.
 */

#ifndef _BMSAPP_
#define _BMSAPP_

#ifdef __cplusplus
extern "C" {
#endif

#include "Std_Types.h"
#include "Can_43_FLEXCAN.h"

/*******************************************************************************
* Definitions
*******************************************************************************/

/**
 * @brief   Bus state codes for CAN 0x300 byte 1.
 * @details IDLE was added in v3 so the HMI can render an "at rest" state
 *          while the current sits inside the dead zone. The HMI already
 *          maps icon 0x00 -> idle (paused).
 */
#define BMS_STATE_IDLE               (0x00U)
#define BMS_STATE_CHARGING           (0x01U)
#define BMS_STATE_DISCHARGING        (0x02U)

/**
 * @brief   INA219 status code in CAN 0x100 byte 6 / 0x210 byte 2 when read OK.
 */
#define BMS_INA219_STATUS_OK         (0x03U)

/**
 * @brief   Temperature status byte values for CAN 0x200 byte 3.
 */
#define BMS_TEMP_STATUS_OK           (0x01U)
#define BMS_TEMP_STATUS_FAIL         (0x00U)

/**
 * @brief   HMI command codes received on CAN 0x710 byte 0.
 */
#define BMS_HMI_CMD_RESET_FAULT      (0x01U)
#define BMS_HMI_CMD_CALIB_SOC        (0x02U)
#define BMS_HMI_CMD_SNAPSHOT         (0x03U)

/**
 * @brief   CAN 0x710 (HMI command) identifier.
 */
#define BMS_HMI_CMD_CAN_ID           (0x710U)

/*******************************************************************************
* API
*******************************************************************************/

/**
 * @brief   Initialise application state (reset cached SDUs, counters).
 *
 * @pre     CDD_INA219_Init() and CDD_STM32Temp_Init() have been called.
 *
 * @param   void
 * @return  Std_ReturnType  E_OK if initialization successful, E_NOT_OK otherwise.
 */
Std_ReturnType BmsApp_Init(void);

/**
 * @brief   50 ms task: read INA219, pack & TX 0x100, run electrical fault check.
 *
 * @return  Std_ReturnType  E_OK if task executed successfully, E_NOT_OK otherwise.
 */
Std_ReturnType BmsApp_Task50ms(void);

/**
 * @brief   100 ms task: compute SOC, pack & TX 0x300 (SOC) and 0x210 (Voltage).
 *
 * @return  Std_ReturnType  E_OK if task executed successfully, E_NOT_OK otherwise.
 */
Std_ReturnType BmsApp_Task100ms(void);

/**
 * @brief   200 ms task: read STM32 temperature, pack & TX 0x200, run thermal fault check.
 *
 * @return  Std_ReturnType  E_OK if task executed successfully, E_NOT_OK otherwise.
 */
Std_ReturnType BmsApp_Task200ms(void);

/**
 * @brief   500 ms task: compute prediction (TTE/TTF), pack & TX 0x400.
 *
 * @return  Std_ReturnType  E_OK if task executed successfully, E_NOT_OK otherwise.
 */
Std_ReturnType BmsApp_Task500ms(void);

#ifdef __cplusplus
}
#endif

#endif /* _BMSAPP_ */

