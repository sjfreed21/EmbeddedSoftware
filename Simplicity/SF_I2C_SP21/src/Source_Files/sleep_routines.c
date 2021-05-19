/***********************************************************************//**
* @file sleep_routines.c
***************************************************************************
* @section License
* <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
***************************************************************************
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
* DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
* obligation to support this Software. Silicon Labs is providing the
* Software "AS IS", with no express or implied warranties of any kind,
* including, but not limited to, any implied warranties of merchantability
* or fitness for any particular purpose or warranties against infringement
* of any proprietary rights of a third party.
*
* Silicon Labs will not be liable for any consequential, incidental, or
* special damages, or any other relief, or for any claim by any third party,
* arising from your use of this Software.
*
**************************************************************************/

#include "sleep_routines.h"


//*******************************************
// private variables
//*******************************************
static int lowest_energy_mode[MAX_ENERGY_MODES];

/********************************************//**
 * @brief
 *	Enables the sleep blocking system.
 *
 * @details
 *	This function initializes the lowest_energy_mode array in order to prevent the system from entering
 *	a sleep mode that would disable a necessary peripheral.
 *
 * @note
 * 	This function should be called before any other sleep-blocking function.
 *******************************************/
void sleep_open(void){
	for (int i = 0; i < MAX_ENERGY_MODES; i++) {
		lowest_energy_mode[i] = 0;
	}
}

/********************************************//**
 * @brief
 *	Sets the sleep mode at which the system should not go below.
 *
 * @details
 *	This function changes the value in the lowest_energy_mode array, which is later used to prevent a specific
 *	sleep mode.
 *
 * @note
 *	The EFM_ASSERT() line is placed in order to prevent the system from getting stuck in a loop and setting the same
 *	array index for too long.
 *
 * @param[in] EM
 * 	The EM parameter should hold the value of the Energy Mode that should be blocked.
 *******************************************/
void sleep_block_mode(uint32_t EM) {
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	lowest_energy_mode[EM]++;
	EFM_ASSERT(lowest_energy_mode[EM] < 5);
	CORE_EXIT_CRITICAL();
}

/********************************************//**
 * @brief
 *	Removes a blocked sleep mode.
 *
 * @details
 *	This function changes the value in the lowest_energy_mode array to remove a block previously placed.
 *
 * @note
 *	This function must be called on the completion of whatever called the previous sleep_block_mode().
 *
 * @param[in] EM
 * 	The EM parameter should hold the value of the Energy Mode that should be unblocked.
 *******************************************/
void sleep_unblock_mode(uint32_t EM){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	if(lowest_energy_mode[EM] > 0) lowest_energy_mode[EM]--;
	EFM_ASSERT(lowest_energy_mode[EM] >= 0);
	CORE_EXIT_CRITICAL();
}

/********************************************//**
 * @brief
 *	Sets the system to the lowest sleep mode that peripherals will allow.
 *
 * @details
 * 	This function depends on the lowest_energy_mode array and definitions in sleep_routines.h.
 *
 * @note
 * 	If this function is not functioning as expected, ensure that sleep_open() and sleep_unblock_mode are creating
 * 	the lowest_energy_mode array.
 *******************************************/
void enter_sleep(void){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	if (lowest_energy_mode[EM0] > 0) {
		CORE_EXIT_CRITICAL();
		return;
	}
	else if (lowest_energy_mode[EM1] > 0) {
		CORE_EXIT_CRITICAL();
		return;
	}
	else if (lowest_energy_mode[EM2] > 0) {
		EMU_EnterEM1();
		CORE_EXIT_CRITICAL();
		return;
	}
	else if (lowest_energy_mode[EM3] > 0) {
		EMU_EnterEM2(true);
		CORE_EXIT_CRITICAL();
		return;
	}
	else {
		EMU_EnterEM3(true);
		CORE_EXIT_CRITICAL();
		return;
	}
}

/********************************************//**
 * @brief
 *	Returns the highest energy mode that has a block on it.
 *
 * @details
 *	This function is the only way to read the private lowest_energy_mode array outside of sleep_routines.c.
 *
 * @note
 *	In this implementation, MAX_ENERGY_MODES is defined in sleep_routines.h and must be changed if the file
 *	is included to control a system with more energy modes.
 *
 * @return
 * 	The highest energy mode with a block on it as an integer.
 *******************************************/
uint32_t current_block_energy_mode(void){
	for (uint32_t i; i < MAX_ENERGY_MODES; i++){
		if(lowest_energy_mode[i] != 0) return i;
	}
	return (MAX_ENERGY_MODES - 1);
}
