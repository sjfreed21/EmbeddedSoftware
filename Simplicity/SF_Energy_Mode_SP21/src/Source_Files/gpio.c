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

}
