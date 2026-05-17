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

#ifndef _CDD_INA219_H_
#define _CDD_INA219_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Std_Types.h"
#include "CDD_INA219_Types.h"
#include "CDD_INA219_Cfg.h"

/*******************************************************************************
* API
*******************************************************************************/

/**
 * @brief   Initialize INA219 power monitor.
 *
 * @details Performs the following register writes via I2C:
 *            1. Config register 0x00 <- 0x8000 (software reset)
 *            2. Calibration register 0x05 <- INA219_CALIB_VAL (4096)
 *            3. Config register 0x00 <- 0x219F (32V, Gain/1, 128-samp, continuous)
 *          After this function returns, INA219 continuously converts and
 *          updates internal registers every ~68 ms.
 *
 * @pre     I2c_Init() has been called. LPI2C0 bus is idle.
 *
 * @param   void
 * @return  void
 *
 * @post    INA219 is in continuous shunt+bus conversion mode.
 *          Internal calibration register = INA219_CALIB_VAL.
 *
 * @note    No error reported if I2C write fails during init -- failure will
 *          surface on first CDD_INA219_ReadXxx() call. Re-calling reinitializes.
 */
void CDD_INA219_Init(void);

/**
 * @brief   Read shunt-derived current from INA219 register 0x04.
 *
 * @details Two-step I2C transaction: write register pointer, read 2 bytes.
 *          Conversion: signed 16-bit raw * INA219_CURRENT_LSB_MA mA/bit.
 *
 * @pre     CDD_INA219_Init() has been called.
 *
 * @param[out] pOutCurrent_mA  Pointer to float32 to receive result.
 *                              On INA219_OK: in range +/- 3200.0 mA.
 *                              Otherwise unchanged. Must not be NULL_PTR.
 *
 * @return  INA219_OK           Transaction complete, data valid.
 * @return  INA219_ERR_TIMEOUT  I2C timeout.
 * @return  INA219_ERR_PARAM    pOutCurrent_mA == NULL_PTR.
 *
 * @post    On success: output written. On failure: output unchanged.
 */
INA219_ReturnType CDD_INA219_ReadCurrent_mA(float32 *pOutCurrent_mA);

/**
 * @brief   Read bus voltage from INA219 register 0x02.
 *
 * @details Two-step I2C transaction. Raw value is right-shifted by 3 bits
 *          (CNVR/OVF flags discarded), then scaled by INA219_BUS_LSB_V (0.004 V).
 *
 * @pre     CDD_INA219_Init() has been called.
 *
 * @param[out] pOutVoltage_V  Pointer to float32 to receive result.
 *                             On INA219_OK: in range [0.0, 26.0] V.
 *                             Otherwise unchanged. Must not be NULL_PTR.
 *
 * @return  INA219_OK / INA219_ERR_TIMEOUT / INA219_ERR_PARAM.
 */
INA219_ReturnType CDD_INA219_ReadBusVoltage_V(float32 *pOutVoltage_V);

/**
 * @brief   Read shunt voltage from INA219 register 0x01 (signed).
 *
 * @details LSB = 10 uV = 0.01 mV. Range +/- 40 mV with PG=00.
 *
 * @pre     CDD_INA219_Init() has been called.
 *
 * @param[out] pOutShunt_mV  Pointer to float32 to receive result.
 *                            Must not be NULL_PTR.
 *
 * @return  INA219_OK / INA219_ERR_TIMEOUT / INA219_ERR_PARAM.
 */
INA219_ReturnType CDD_INA219_ReadShuntVoltage_mV(float32 *pOutShunt_mV);

/**
 * @brief   Read power from INA219 register 0x03.
 *
 * @details Power_LSB = 20 * Current_LSB. Returned in milliwatts.
 *
 * @pre     CDD_INA219_Init() has been called.
 *
 * @param[out] pOutPower_mW  Pointer to float32 to receive result.
 *                            Must not be NULL_PTR.
 *
 * @return  INA219_OK / INA219_ERR_TIMEOUT / INA219_ERR_PARAM.
 */
INA219_ReturnType CDD_INA219_ReadPower_mW(float32 *pOutPower_mW);

/**
 * @brief   Software-reset INA219 (Config register bit 15 = 1).
 *
 * @details Forces the chip back to its power-on-reset configuration
 *          (0x399F) and clears the calibration register. The caller should
 *          re-run CDD_INA219_Init() to restore the BMS configuration.
 *
 * @pre     I2c_Init() has been called.
 *
 * @param   void
 * @return  INA219_OK           Reset command was accepted.
 * @return  INA219_ERR_TIMEOUT  I2C bus timed out.
 *
 * @post    On success: INA219 internal registers are in POR state.
 *          Subsequent reads will return zeros until Init() is re-run.
 */
INA219_ReturnType CDD_INA219_Reset(void);

/**
 * @brief   Check that the INA219 is alive on the I2C bus.
 *
 * @details Reads the CONFIG register once and returns TRUE if the I2C
 *          transaction completed without timeout. Does NOT validate the
 *          register value -- a returned TRUE means the slave ACKed, not
 *          that the configuration is correct.
 *
 * @pre     I2c_Init() has been called.
 *
 * @param   void
 *
 * @return  TRUE   I2C read succeeded (slave is responding).
 * @return  FALSE  I2C timeout (slave missing, wiring fault, bus stuck).
 *
 * @post    No side effects on the chip; one register pointer + read
 *          transaction is issued on the bus.
 *
 * @note    Safe to call periodically from a task context for a liveness
 *          probe. Do not call from ISR context.
 */
boolean CDD_INA219_IsConnected(void);

/**
 * @brief   Atomic snapshot read of bus voltage, current, power, shunt voltage.
 *
 * @details Convenience wrapper that calls the four individual Read functions
 *          in sequence and returns the first non-OK status (subsequent reads
 *          are skipped). Reduces caller overhead in the 50 ms slot by
 *          packaging all telemetry into one call. NULL pointers are checked
 *          up-front; if any output pointer is NULL the function returns
 *          INA219_ERR_PARAM without touching the bus.
 *
 * @pre     CDD_INA219_Init() has been called.
 *
 * @param[out] pOutVolt_V    Bus voltage (V).            Must not be NULL_PTR.
 * @param[out] pOutCurr_mA   Current (mA, signed).       Must not be NULL_PTR.
 * @param[out] pOutPower_mW  Power (mW).                 Must not be NULL_PTR.
 * @param[out] pOutShunt_mV  Shunt voltage (mV, signed). Must not be NULL_PTR.
 *
 * @return  INA219_OK            All four reads succeeded.
 * @return  INA219_ERR_TIMEOUT   At least one read timed out (which one is
 *                                indicated by the order of evaluation:
 *                                voltage, current, power, shunt).
 * @return  INA219_ERR_PARAM     One or more output pointers were NULL.
 *
 * @post    On success: all four outputs hold fresh values from the SAME
 *                       conversion cycle (within ~1 ms read latency).
 *          On failure: outputs that were successfully read hold their new
 *                       values; the one that failed is unchanged.
 */
INA219_ReturnType CDD_INA219_ReadAll(float32 *pOutVolt_V,
                                     float32 *pOutCurr_mA,
                                     float32 *pOutPower_mW,
                                     float32 *pOutShunt_mV);

#ifdef __cplusplus
}
#endif

#endif /* _CDD_INA219_H_ */
