/**
 * @file cmu.c
 * @author Sam Freed
 * @date January 28th, 2021
 * @brief Contains all the setup for the CMU base clocks when setting up LETIMER0.
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "cmu.h"

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
 *	Sets up the clocks required for the LETIMER PWM setup.
 *
 * @details
 *	This function disables the default oscillators in order to select our proper
 *	base clock and to minimize power usage. It also enables the higher frequency
 *	peripheral clock for the LEDs.
 *
 * @note
 *	Clocks should always be initialized first, so this function should run prior
 *	to the initializing and setup of the LETIMER and other peripherals.
 *
 ******************************************************************************/

void cmu_open(void){

		CMU_ClockEnable(cmuClock_HFPER, true);

		// By default, Low Frequency Resistor Capacitor Oscillator, LFRCO, is enabled,
		// Disable the LFRCO oscillator
		CMU_OscillatorEnable(cmuOsc_LFRCO, false, false);	 // What is the enumeration required for LFRCO?

		// Disable the Low Frequency Crystal Oscillator, LFXO
		CMU_OscillatorEnable(cmuOsc_LFXO, false, false);	// What is the enumeration required for LFXO?

		// No requirement to enable the ULFRCO oscillator.  It is always enabled in EM0-4H

		// Route LF clock to LETIMER0 clock tree
		CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);	// What clock tree does the LETIMER0 reside on?

		// Now, you must ensure that the global Low Frequency is enabled
		CMU_ClockEnable(cmuClock_CORELE, true);	//This enumeration is found in the Lab 2 assignment

}

