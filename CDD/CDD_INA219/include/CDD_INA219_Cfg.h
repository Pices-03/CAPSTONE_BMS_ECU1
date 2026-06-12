///**
// * @file       CDD_INA219_Cfg.h
// * @version    1.1.0
// * @brief      Configuration constants for INA219 power monitor driver.
// *
// * @details    Hardware-specific configuration: I2C slave address, channel,
// *             register map, calibration values for 1S Li-ion demo.
// *             Hardware: NXP S32K144, LPI2C0 PTA2(SDA)/PTA3(SCL) @ 100 kHz.
// *             Pack: 1S Li-ion (demo) / 5 V resistor rig, shunt 0.1 ohm,
// *             Imax 3.2 A.
// *
// *             Calibration is derived from INA219_SHUNT_R_OHM and INA219_IMAX_A
// *             at compile time -- changing the shunt value automatically
// *             rebuilds CALIB_VAL and CURRENT_LSB_MA. Power LSB is also derived
// *             per datasheet formula (Power_LSB = 20 * Current_LSB).
// *
// * @note       Modify only when hardware changes (different shunt, different
// *             pack topology, or different I2C channel).
// */
//
//#ifndef _CDD_INA219_CFG_H_
//#define _CDD_INA219_CFG_H_
//
//#ifdef __cplusplus
//extern "C" {
//#endif
//
///*******************************************************************************
//* Definitions
//*******************************************************************************/
//
///**
// * @brief   I2C slave address of INA219 (A0=GND, A1=GND).
// * @details 7-bit address. Other strap combinations: A0=VCC -> 0x41, etc.
// */
//#define INA219_I2C_ADDR              (0x40U)
//
///**
// * @brief   I2C master channel index (LPI2C0).
// * @details Same channel as STM32 temperature sensor; bus arbitration handled
// *          sequentially in the application scheduler.
// */
//#define INA219_I2C_CHANNEL           (0U)
//
///**
// * @brief   INA219 register addresses (datasheet Table 6).
// */
//#define INA219_REG_CONFIG            (0x00U)
//#define INA219_REG_SHUNT             (0x01U)
//#define INA219_REG_BUS               (0x02U)
//#define INA219_REG_POWER             (0x03U)
//#define INA219_REG_CURRENT           (0x04U)
//#define INA219_REG_CALIBRATION       (0x05U)
//
///**
// * @brief   Configuration register value (PG bumped to +/-320 mV).
// * @details Bit 13:12 BRNG = 01   -> Bus 32 V
// *          Bit 11:10 PG   = 11   -> Gain/8 +/-320 mV shunt (max 3.2 A @ R=0.1)
// *          Bit  9:6  BADC = 1111 -> 128-sample averaging  (~68 ms)
// *          Bit  5:2  SADC = 1111 -> 128-sample averaging
// *          Bit  1:0  MODE = 11   -> Shunt + Bus continuous
// *
// *          Previous value 0x219F used PG=00 (+/-40 mV) which saturated above
// *          0.4 A -- inconsistent with the 3.2 A range stated by calibration.
// */
//#define INA219_CONFIG_VAL            (0x319FU)
//
///**
// * @brief   Software reset command (Config register bit 15).
// */
//#define INA219_RESET_VAL             (0x8000U)
//
///**
// * @brief   Power-on reset value of CONFIG register (datasheet Table 6).
// * @details Used by CDD_INA219_IsConnected() to sanity-check the bus link.
// */
//#define INA219_CONFIG_POR_VAL        (0x399FU)
//
///**
// * @brief   Shunt resistor value (ohms).
// */
//#define INA219_SHUNT_R_OHM           (0.1f)
//
///**
// * @brief   Current LSB in amps per bit.
// * @details Clean choice = 100 uA/bit. Chip Cal register = 4096 (exact integer).
// *          Resulting Imax = 100 uA * 32768 = 3.2768 A (slightly above the
// *          3.2 A target -- the PG=11 (+/-320 mV) shunt limit caps physical
// *          Imax at exactly 3.2 A anyway). Power_LSB = 20 * Current_LSB =
// *          2 mW exactly.
// */
//#define INA219_CURRENT_LSB_A         (1.0e-4f)
//#define INA219_CURRENT_LSB_MA        (0.1f)
//
///**
// * @brief   Maximum expected pack current (amps, derived view).
// */
//#define INA219_IMAX_A                (INA219_CURRENT_LSB_A * 32768.0f)
//
///**
// * @brief   Calibration register value (clean integer 4096).
// * @details Cal = 0.04096 / (Current_LSB * R_shunt) = 0.04096 / (100e-6 * 0.1).
// */
//#define INA219_CALIB_VAL             (4096U)
//
///**
// * @brief   Bus voltage LSB (datasheet 8.6.3): 4 mV per bit.
// */
//#define INA219_BUS_LSB_V             (0.004f)
//
///**
// * @brief   Shunt voltage LSB (datasheet 8.6.3.2): 10 uV per bit.
// */
//#define INA219_SHUNT_LSB_MV          (0.01f)
//
///**
// * @brief   Power LSB = 20 * Current_LSB = 2 mW per bit.
// */
//#define INA219_POWER_LSB_MW          (2.0f)
//
///**
// * @brief   I2C polling timeout (loop iterations).
// * @details ~1.3 ms at 80 MHz core. Increase if bus runs slower or under
// *          heavy contention.
// */
//#define INA219_I2C_TIMEOUT           (0xFFFFU)
//
//#ifdef __cplusplus
//}
//#endif
//
//#endif /* _CDD_INA219_CFG_H_ */

/*==================================================================================================
* Project : BMS AUTOSAR Demo
* Platform : CORTEXM
* Peripheral : S32K144
* Component : CDD_INA219
* Module : CDD_INA219_Cfg.h
* Description : Configuration header file for INA219 Current/Power Monitor Driver (Polling Mode)
* Tool : S32DS 3.5 / RTD 2.0.0
*==================================================================================================*/

#ifndef _CDD_INA219_CFG_
#define _CDD_INA219_CFG_

/*==================================================================================================
* HARDWARE CONSTANTS
==================================================================================================*/

/**
 * @brief INA219 Default I2C Slave Address (A0, A1 connected to GND)
 */
#define INA219_I2C_ADDRESS          0x40U

/**
 * @brief I2C Channel ID defined in the Configuration Tool
 * Mapping to I2cChannel_0 in LPI2C_0
 */
#define INA219_I2C_CHANNEL          0U

/*==================================================================================================
* REGISTER ADDRESS MAP
==================================================================================================*/
#define INA219_REG_CONFIG           0x00U  /* Configuration Register */
#define INA219_REG_SHUNT_VOLTAGE    0x01U  /* Shunt Voltage Register */
#define INA219_REG_BUS_VOLTAGE      0x02U  /* Bus Voltage Register (Battery Voltage) */
#define INA219_REG_POWER            0x03U  /* Power Register */
#define INA219_REG_CURRENT          0x04U  /* Current Register */
#define INA219_REG_CALIBRATION      0x05U  /* Calibration Register */

/*==================================================================================================
* CONFIGURATION PARAMETERS
==================================================================================================*/

/**
 * @brief Default Configuration Value for CONFIG Register (0x00)
 * Settings: 32V Range, +/-320mV Shunt Range, 12-bit ADC, Continuous Conversion
 */
#define INA219_CONFIG_DEFAULT       0x399FU

/**
 * @brief Shunt resistor value in Ohms (Î©).
 * This is a crucial parameter for current calculation.
 * For a variable resistor simulation, a typical value is 0.1Î©.
 * Change this to match your actual hardware.
 */
#define INA219_SHUNT_RESISTOR_OHMS       0.1f

/**
 * @brief Expected maximum expected current in Amperes (A).
 * Used to calculate the Calibration Register value.
 * With a 0.1Î© shunt and PGA=Â±320mV, the max current is 3.2A.
 * Adjust based on your variable resistor range and power supply.
 */
#define INA219_MAX_EXPECTED_CURRENT_A    3.2f

/**
 * @brief Expected maximum shunt voltage in Volts (V).
 * This is derived from SHUNT_RESISTOR and MAX_EXPECTED_CURRENT.
 * It is used to calculate the Calibration Register value.
 */
#define INA219_MAX_SHUNT_VOLTAGE_V       (INA219_SHUNT_RESISTOR_OHMS * INA219_MAX_EXPECTED_CURRENT_A)

/*============================================================================*/
/*                           Calculation Macros                               */
/*============================================================================*/
/**
 * @brief Calculates the Calibration Register value based on the configuration.
 * The formula is derived from the datasheet, Section 8.5.1 (Page 17).
 * Current_LSB (A/LSB) = Maximum Expected Current (A) / 2^15 (32768)
 * Cal = 0.04096 / (Current_LSB * Shunt_Resistor)
 */
#define INA219_CURRENT_LSB               (INA219_MAX_EXPECTED_CURRENT_A / 32768.0f)
#define INA219_CALIBRATION_VALUE         (uint16)(0.04096f / (INA219_CURRENT_LSB * INA219_SHUNT_RESISTOR_OHMS))

/**
 * @brief Actual current LSB after calibration (for verification)
 */
#define INA219_ACTUAL_CURRENT_LSB        (0.04096f / (INA219_CALIBRATION_VALUE * INA219_SHUNT_RESISTOR_OHMS))

/**
 * @brief Bus Voltage LSB in millivolts per LSB (from datasheet)
 */
#define INA219_VOLTAGE_LSB_MV           4.0f

#endif /* _CDD_INA219_CFG_ */

