/**
* @file       CDD_INA219.h
* @version    1.0.0
* @brief      Public API of CDD INA219 power monitor driver.
*
* @details    Provides register-level read/write access to INA219 over I2C
*             for bus voltage, shunt voltage, current and power measurement.
*             Hardware: NXP S32K144, LPI2C0 PTA2(SDA)/PTA3(SCL) @ 100 kHz,
*             slave address 0x40.
*
*             All Read functions return INA219_ReturnType so the caller can
*             distinguish I2C bus errors (timeout) from valid measurements.
*             On success, the result is written through the output pointer;
*             on failure the output is left unchanged.
*
* @note       Caller must ensure I2c_Init() has been called before any
*             CDD_INA219_xxx() function. Not interrupt-safe -- do not call
*             from ISR context.
*/

#ifndef _CDD_INA219_
#define _CDD_INA219_

#ifdef __cplusplus
extern "C"
{
#endif

#include "Std_Types.h"
#include "CDD_INA219_Cfg.h"

/*==================================================================================================
* FUNCTION PROTOTYPES
==================================================================================================*/

/**
 * @brief Initialize INA219 sensor and load calibration
 * @return E_OK if initialization successful, E_NOT_OK if failed
 */
Std_ReturnType CDD_INA219_Init(void);

/**
 * @brief Read current value in Amperes
 * @param CurrentPtr Pointer to store current value (A)
 * @return E_OK if read successful, E_NOT_OK if failed
 */
Std_ReturnType CDD_INA219_ReadCurrent_mA(float32 *CurrentPtr);

/**
 * @brief Read bus voltage value in Volts
 * @param VoltagePtr Pointer to store voltage value (V)
 * @return E_OK if read successful, E_NOT_OK if failed
 */
Std_ReturnType CDD_INA219_ReadBusVoltage_mV(float32 *VoltagePtr);

/**
 * @brief Read power value in Watts
 * @param PowerPtr Pointer to store power value (W)
 * @return E_OK if read successful, E_NOT_OK if failed
 */
Std_ReturnType CDD_INA219_ReadPower(float32 *PowerPtr);

/**
 * @brief Read shunt voltage value in Volts
 * @param ShuntVoltagePtr Pointer to store shunt voltage value (V)
 * @return E_OK if read successful, E_NOT_OK if failed
 */
Std_ReturnType CDD_INA219_ReadShuntVoltage(float32 *ShuntVoltagePtr);

/**
 * @brief Read all values (voltage, current, power, shunt voltage)
 * @param VoltagePtr Pointer to store voltage value (V)
 * @param CurrentPtr Pointer to store current value (A)
 * @param PowerPtr Pointer to store power value (W)
 * @param ShuntVoltagePtr Pointer to store shunt voltage value (V)
 * @return E_OK if read successful, E_NOT_OK if failed
 */
Std_ReturnType CDD_INA219_ReadAll(float32 *VoltagePtr, float32 *CurrentPtr, float32 *PowerPtr, float32 *ShuntVoltagePtr);

/**
 * @brief Reset INA219 to default configuration
 * @return E_OK if reset successful, E_NOT_OK if failed
 */
Std_ReturnType CDD_INA219_Reset(void);

/**
 * @brief Check if INA219 is connected and responding
 * @return TRUE if connected, FALSE if not
 */
boolean CDD_INA219_IsConnected(void);

#ifdef __cplusplus
}
#endif

#endif /* _CDD_INA219_ */
