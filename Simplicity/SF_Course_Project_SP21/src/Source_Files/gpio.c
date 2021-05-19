/**
 * @file gpio.c
 * @author Sam Freed
 * @date January 28th, 2021
 * @brief Enables LED output using GPIO.
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "gpio.h"


//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Private variables
//***********************************************************************************


//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *	Sets up the GPIO for the LEDs.
 *
 * @details
 * 	This function enables the GPIO clock and then sets the LED drive strength
 * 	and pinmodes. These are all defined in the brd_config header files based on
 * 	the datasheet.
 *
 * @note
 * 	This function should only be called once to initialize the GPIO settings.
 *
 *
 ******************************************************************************/

void gpio_open(void){

	CMU_ClockEnable(cmuClock_GPIO, true);

	// Configure LED pins
	GPIO_DriveStrengthSet(LED0_PORT, LED0_DRIVE_STRENGTH);
	GPIO_PinModeSet(LED0_PORT, LED0_PIN, LED0_GPIOMODE, LED0_DEFAULT);

	GPIO_DriveStrengthSet(LED1_PORT, LED1_DRIVE_STRENGTH);
	GPIO_PinModeSet(LED1_PORT, LED1_PIN, LED1_GPIOMODE, LED1_DEFAULT);

	// Configure I2C Pins
	GPIO_DriveStrengthSet(SI7021_SENSOR_EN_PORT, SI7021_DRIVE_STRENGTH);
	GPIO_PinModeSet(SI7021_SENSOR_EN_PORT, SI7021_SENSOR_EN_PIN, SI7021_SENSOR_GPIOMODE, SI7021_SENSOR_DEFAULT);

	GPIO_PinModeSet(SI7021_SCL_PORT, SI7021_SCL_PIN, SI7021_I2C_GPIOMODE, SI7021_I2C_DEFAULT);
	GPIO_PinModeSet(SI7021_SDA_PORT, SI7021_SDA_PIN, SI7021_I2C_GPIOMODE, SI7021_I2C_DEFAULT);

	GPIO_PinModeSet(VEML6030_SCL_PORT, VEML6030_SCL_PIN, VEML6030_I2C_GPIOMODE, VEML6030_I2C_DEFAULT);
	GPIO_PinModeSet(VEML6030_SDA_PORT, VEML6030_SDA_PIN, VEML6030_I2C_GPIOMODE, VEML6030_I2C_DEFAULT);

	// Configure LEUART Pins
	GPIO_DriveStrengthSet(LEUART_TX_PORT, LEUART_TX_STRENGTH);
	GPIO_PinModeSet(LEUART_TX_PORT, LEUART_TX_PIN, LEUART_TX_GPIOMODE, LEUART_TR_DEFAULT);
	GPIO_PinModeSet(LEUART_RX_PORT, LEUART_RX_PIN, LEUART_RX_GPIOMODE, LEUART_TR_DEFAULT);
}
