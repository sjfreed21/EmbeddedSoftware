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
#include <stdio.h>

//***********************************************************************************
// defined files
//***********************************************************************************
//#define BLE_TEST_ENABLED
//#define I2C_TEST_ENABLED

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
 *	This is a higher-level function that calls the CMU, GPIO, scheduler, sleep, and SI7021 open functions,
 *	then sets up the APP_LETIMER_PWM and LETIMER0 structures, and finally enables the
 *	LETIMER0 system in PWM mode.
 *
 * @note
 *  This function serves as an all-in-one command to set up the LED, LETIMER0, and SI7021 peripherals
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
	sleep_block_mode(SYSTEM_BLOCK_EM);
	add_scheduled_event(BOOT_UP_CB);
	app_letimer_pwm_open(PWM_PER, PWM_ACT_PER, PWM_ROUTE_0, PWM_ROUTE_1);
	si7021_i2c_open();
	veml6030_i2c_open();
	ble_open(BLE_TX_DONE_CB, BLE_RX_DONE_CB);
	veml6030_write(VEML6030_READ_CB);
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
	letStruct.out_pin_0_en = false;
	letStruct.out_pin_1_en = false;
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
 *	In its current implementation, this function calls the SI7021 read function.
 *
 * @note
 *	If set to the EM shifting mode as in Lab 3, this function is set up to handle 5 energy modes and must be
 *	changed if used on a system with more/fewer energy modes.
 *******************************************/
void scheduled_letimer0_uf_cb(void) {
	EFM_ASSERT(get_scheduled_events() & LETIMER0_UF_CB);
	remove_scheduled_event(LETIMER0_UF_CB);

//	uint32_t current = current_block_energy_mode();
//	sleep_unblock_mode(current);
//	if (current < 4) sleep_block_mode(current + 1);
//	else sleep_block_mode(EM0);

	si7021_hread(SI7021_HREAD_CB);
	veml6030_read(VEML6030_READ_CB);
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

/********************************************//**
 * @brief
 *	The scheduler program to handle I2C Humidity Read interrupts.
 *
 * @details
 *	This function currently toggles LED1 based on the value output by the SI7021, as well as
 *	outputs the humidity value to the HM-10 for transmission via BLE.
 *
 * @note
 *	If run on different system, LED1_PORT and _PIN must be updated in brd_config.h.
 *******************************************/
void scheduled_si7021_hread_cb(void) {
	EFM_ASSERT(get_scheduled_events() & SI7021_HREAD_CB);
	remove_scheduled_event(SI7021_HREAD_CB);

	float val = si7021_conversion();
	char send[25];
	sprintf(send, "Humidity = %2.1f %%\n", val);
	ble_write(send);
	if(val >= 30.0) GPIO_PinModeSet(LED1_PORT, LED1_PIN, LED1_GPIOMODE, true);
	else GPIO_PinOutClear(LED1_PORT, LED1_PIN);

	si7021_tread(SI7021_TREAD_CB);
}

/********************************************//**
 * @brief
 *	The scheduler program to handle I2C Temp Read interrupts.
 *
 * @details
 *	This function outputs the temperature value to the HM-10 for transmission via BLE.
 *******************************************/
void scheduled_si7021_tread_cb(void) {
	EFM_ASSERT(get_scheduled_events() & SI7021_TREAD_CB);
	remove_scheduled_event(SI7021_TREAD_CB);
	float val = si7021_temp_conversion();
	char send[25];
	sprintf(send, "Temp = %2.1f F\n", val);
	ble_write(send);
}


/********************************************//**
 * @brief
 *	The scheduler program to handle LEUART boot up interrupts.
 *
 * @details
 *	This function can run the ble_test function defined in ble.c and will always
 *	write the string given in ble_write() to the HM-10.
 *
 * @note
 * 	To enable the ble_test call, uncomment the #define statement on line 18 of this file
 * 	or otherwise add "#define BLE_TEST_ENABLED".
 *******************************************/
void scheduled_boot_up_cb(void) {
	EFM_ASSERT(get_scheduled_events() & BOOT_UP_CB);
	remove_scheduled_event(BOOT_UP_CB);
	letimer_start(LETIMER0, true);
#ifdef BLE_TEST_ENABLED
	EFM_ASSERT(ble_test("SJF HM-10"));
	timer_delay(2000);
#endif
#ifdef I2C_TEST_ENABLED
	EFM_ASSERT(si7021_test(SI7021_HREAD_CB));
#endif
	ble_write("\nHello World\n");
}

/********************************************//**
 * @brief
 *	The scheduler program to handle LEUART TX interrupts.
 *
 * @details
 *  At the moment, this function only removes the TX event.
 *
 * @note
 *  BLE_TX_DONE_CB is defined in app.h with all other scheduler defines.
 *******************************************/
void scheduled_ble_tx_done_cb(void) {
	EFM_ASSERT(get_scheduled_events() & BLE_TX_DONE_CB);
	remove_scheduled_event(BLE_TX_DONE_CB);
}

/********************************************//**
 * @brief
 *	The scheduler program to handle VEML Read interrupts.
 *
 * @details
 *	This function retrieves the data read from the VEML, converts it to
 *	the correct value, and sends it to the LEUART BT setup.
 *******************************************/
void scheduled_veml6030_read_cb(void) {
	EFM_ASSERT(get_scheduled_events() & VEML6030_READ_CB);
	remove_scheduled_event(VEML6030_READ_CB);
	unsigned int val = (unsigned int) veml6030_conversion();
	char send[30];
	sprintf(send, "Light Intensity = %3u lux\n\n", val);
	ble_write(send);
}
