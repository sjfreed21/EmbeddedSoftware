/**
 * @file scheduler.c
 * @author Sam Freed
 * @date February 11, 2021
 * @brief Sets up the scheduler structure.
 */

#include "scheduler.h"
#include "em_assert.h"
#include "em_core.h"
#include "em_emu.h"

//*******************************************
// private variables
//*******************************************

static unsigned int event_scheduled;

/********************************************//**
 * @brief
 *	Enables the scheduler structure.
 *
 * @details
 *	This function sets the private variable event_scheduled to zero, ensuring that the event handler is clear
 *	on startup.
 *
 * @note
 * 	This function should be called before any other scheduler functions, especially with cmu_open() and gpio_open().
 *
 *******************************************/
void scheduler_open(void) {
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	event_scheduled = 0;
	CORE_EXIT_CRITICAL();
}

/********************************************//**
 * @brief
 *	Adds an event to the scheduler.
 *
 * @details
 *	This atomic function ORs the input with the current scheduler status in order to add it to the handler.
 *
 * @note
 *	This function should be called during the IRQ routine after an interrupt has occured.
 *
 * @param[in] event
 * 	This will contain the bit that signifies whichever event needs to be added to the scheduler; in this implementation,
 * 	these values are defined in app.h.
 *******************************************/
void add_scheduled_event(uint32_t event) {
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	event_scheduled |= event;
	CORE_EXIT_CRITICAL();
}

/********************************************//**
 * @brief
 *	Removes an event from the scheduler.
 *
 * @details
 * 	Using the negate AND method, this function removes an event from the scheduler.
 *
 * @note
 * 	This should be called inside the scheduler routine in order to clear the event so that it is not repeatedly called.
 *
 * @param[in] event
 * 	This will contain the bit that signifies whichever event needs to be removed from the scheduler; in this implementation,
 * 	these values are defined in app.h.
 *******************************************/
void remove_scheduled_event(uint32_t event) {
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	event_scheduled &= ~event;
	CORE_EXIT_CRITICAL();
}

/********************************************//**
 * @brief
 *	Returns the current scheduler status.
 *
 * @details
 *	This function is the only way to read the private variable event_scheduled outside of scheduler.c.
 *
 * @note
 *	In this implementation, this function is used to poll the scheduler.
 *
 * @return
 * 	The current scheduler status as a uint32_t.
 *******************************************/
uint32_t get_scheduled_events(void){
	return event_scheduled;
}
