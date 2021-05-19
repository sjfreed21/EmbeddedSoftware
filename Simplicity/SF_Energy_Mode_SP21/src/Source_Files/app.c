/**
 * @file app.c
 * @author Sam Freed
 * @date January 28th, 2021
 * @brief Creates and populates the APP_LETIMER_PWM struct, then sets up LETIMER0.
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "app.h"


//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Static / Private Variables
//***********************************************************************************


//***********************************************************************************
// Private functions
//***********************************************************************************

static void app_letimer_pwm_open(float period, float act_period, uint32_t out0_route, uint32_t out1_route);

//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *	Sets up the PWM LETIMER.
 *
 * @details
 *	This is a higher-level function that calls the CMU, GPIO, scheduler, and sleep open functions,
 *	then sets up the APP_LETIMER_PWM and LETIMER0 structures, and finally enables the
 *	LETIMER0 system in PWM mode.
 *
 * @note
 *  This function serves as an all-in-one command to set up the LED and LETIMER0 peripherals
 *  and only needs to be called once in order to function. It will repeat the LETIMER0 timer 100 times;
 *  to end the timer early, call letimer_start() with the enable parameter set to false.
 *
 *
 ******************************************************************************/

void app_peripheral_setup(void){
	cmu_open();
	gpio_open();
	scheduler_open();
	sleep_open();
	app_letimer_pwm_open(PWM_PER, PWM_ACT_PER, PWM_ROUTE_0, PWM_ROUTE_1);
	letimer_start(LETIMER0, true);   // letimer_start will inform the LETIMER0 peripheral to begin counting.
}

/***************************************************************************//**
 * @brief
 *	Sets up the APP_LETIMER_PWM struct and calls the letimer_pwm_open() function.
 *
 * @details
 *	This function defines a new structure of type APP_LETIMER_PWM, which is used to package
 *	and pass the basic setup data into the letimer_pwm_open() function.
 *
 * @note
 *	The function currently defaults to setting enable and debugRun to false, ensuring that
 *	the timer only runs when it is properly set up and does not continue when there
 *	is a breakpoint during debugging. It also has both output ports set up - in this lab
 *	implementation, these outputs are directed using the define statements in app.h.
 *
 * @param[in] period
 *	Clock period in seconds as an unsigned integer.
 *
 * @param[in] act_period
 *	PWM uptime in seconds as an unsigned integer.
 *
 * @param[in] out0_route
 *	Output route 0 expressed as an address, which can be determined using the datasheets.
 *
 * @param[in] out1_route
 *	Output route 1 expressed as an address, which can be determined using the datasheets.
 *
 ******************************************************************************/
void app_letimer_pwm_open(float period, float act_period, uint32_t out0_route, uint32_t out1_route){
	// Initializing LETIMER0 for PWM operation by creating the
	// letimer_pwm_struct and initializing all of its elements

	APP_LETIMER_PWM_TypeDef letStruct;

	letStruct.enable = false;
	letStruct.debugRun = false;
	letStruct.period = period;
	letStruct.active_period = act_period;
	letStruct.out_pin_0_en = true;
	letStruct.out_pin_1_en = true;
	letStruct.out_pin_route0 = out0_route;
	letStruct.out_pin_route1 = out1_route;
	letStruct.comp0_irq_enable = false;
	letStruct.comp0_cb = LETIMER0_COMP0_CB;
	letStruct.comp1_irq_enable = false;
	letStruct.comp1_cb = LETIMER0_COMP1_CB;
	letStruct.uf_irq_enable = true;
	letStruct.uf_cb = LETIMER0_UF_CB;

	letimer_pwm_open(LETIMER0, &letStruct);

}

/********************************************//**
 * @brief
 *	The scheduler program to handle UF interrupts.
 *
 * @details
 *	In its current implementation, this function cycles the blocked energy modes.
 *
 * @note
 *	This function is set up to handle 5 energy modes and must be changed if used on a system with
 *	more/fewer energy modes.
 *******************************************/
void scheduled_letimer0_uf_cb(void) {
	EFM_ASSERT(get_scheduled_events() & LETIMER0_UF_CB);
	remove_scheduled_event(LETIMER0_UF_CB);

	uint32_t current = current_block_energy_mode();
	sleep_unblock_mode(current);
	if (current < 4) sleep_block_mode(current + 1);
	else sleep_block_mode(EM0);
}

/********************************************//**
 * @brief
 *	The scheduler program to handle Comp0 interrupts.
 *
 * @details
 * 	This function is not currently used.
 *
 * @note
 * 	If this function is called, the system will be stuck in an EFM_ASSERT(false) loop.
 *******************************************/
void scheduled_letimer0_comp0_cb(void) {
	EFM_ASSERT(get_scheduled_events() & LETIMER0_COMP0_CB);
	remove_scheduled_event(LETIMER0_COMP0_CB);
	EFM_ASSERT(false);
}

/********************************************//**
 * @brief
 *	The scheduler program to handle Comp1 interrupts.
 *
 * @details
 *	This function is not currently used.
 *
 * @note
 * 	If this function is called, the system will be stuck in an EFM_ASSERT(false) loop.
 *******************************************/
void scheduled_letimer0_comp1_cb(void) {
	EFM_ASSERT(get_scheduled_events() & LETIMER0_COMP1_CB);
	remove_scheduled_event(LETIMER0_COMP1_CB);
	EFM_ASSERT(false);
}

