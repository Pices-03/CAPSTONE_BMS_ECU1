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

