/**
 * @file leuart.c
 * @author Sam Freed
 * @date April 4th, 2021
 * @brief Contains all the functions of the LEUART peripheral
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************

//** Standard Library includes
#include <string.h>

//** Silicon Labs include files
#include "em_gpio.h"
#include "em_cmu.h"

//** Developer/user include files
#include "leuart.h"
#include "scheduler.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// private variables
//***********************************************************************************
uint32_t	rx_done_evt;
uint32_t	tx_done_evt;
bool		leuart0_tx_busy;

typedef enum {
	EN_TX,
	TX_DATA,
	END_TX,
} DEFINED_STATES;

LEUART_STATE_MACHINE leuart_state;

/***************************************************************************//**
 * @brief LEUART driver
 * @details
 *  This module contains all the functions to support the driver's state
 *  machine to transmit a string of data across the LEUART bus.  There are
 *  additional functions to support the Test Driven Development test that
 *  is used to validate the basic set up of the LEUART peripheral.  The
 *  TDD test for this class assumes that the LEUART is connected to the HM-18
 *  BLE module.  These TDD support functions could be used for any TDD test
 *  to validate the correct setup of the LEUART.
 *
 ******************************************************************************/

//***********************************************************************************
// Private functions
//***********************************************************************************

static void leuart_txbl(LEUART_STATE_MACHINE *leuart_state);
static void leuart_txc(LEUART_STATE_MACHINE *leuart_state);

//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *	Initiates the LEUART system.
 *
 * @details
 * 	This function sets up the LEUART TypeDef default structure and its interrupt
 * 	routine structures.
 *
 * @note
 *  This function fully enables the LEUART system & peripheral.
 *
 * @param[in] leuart
 *  The LEUART system currently being used.
 *
 * @param[in] leuart_settings
 *  The open struct that will have its data values read, initiated outside of this function.
 ******************************************************************************/

void leuart_open(LEUART_TypeDef *leuart, LEUART_OPEN_STRUCT *leuart_settings){
	LEUART_Init_TypeDef leuart_init;

	if(leuart == LEUART0) {
		CMU_ClockEnable(cmuClock_LEUART0, true);
	} else {
		EFM_ASSERT(false);
	}

	if(!(leuart->STARTFRAME & 0x01)){
		leuart->STARTFRAME = 0x01;
		while(leuart->SYNCBUSY);
		EFM_ASSERT(leuart->STARTFRAME & 0x01);
		leuart->STARTFRAME = 0x00;
		while(leuart->SYNCBUSY);
	}

	leuart_init.baudrate = leuart_settings->baudrate;
	leuart_init.databits = leuart_settings->databits;
	leuart_init.parity = leuart_settings->parity;
	leuart_init.refFreq = false;
	leuart_init.stopbits = leuart_settings->stopbits;

	tx_done_evt = leuart_settings->tx_done_evt;
	rx_done_evt = leuart_settings->rx_done_evt;

	LEUART_Init(leuart, &leuart_init);
	while(leuart->SYNCBUSY);

	leuart->ROUTELOC0 = leuart_settings->rx_loc | leuart_settings->tx_loc;
	leuart->ROUTEPEN = (leuart_settings->rx_pin_en*leuart_settings->rx_en) | (leuart_settings->tx_pin_en*leuart_settings->tx_en);

	leuart->CMD = LEUART_CMD_CLEARTX | LEUART_CMD_CLEARRX;

	LEUART_Enable(leuart, leuart_settings->enable);
	while(leuart->SYNCBUSY);

	if(leuart_settings->tx_en){
		leuart->CMD = LEUART_CMD_TXEN;
		while(!(leuart->STATUS & LEUART_STATUS_TXENS));
		EFM_ASSERT(leuart->STATUS & LEUART_STATUS_TXENS);
	}
	if(leuart_settings->rx_en){
		leuart->CMD = LEUART_CMD_RXEN;
		while(!(leuart->STATUS & LEUART_STATUS_RXENS));
		EFM_ASSERT(leuart->STATUS & LEUART_STATUS_RXENS);
	}

	if(leuart == LEUART0) {
		NVIC_EnableIRQ(LEUART0_IRQn);
	}
	else {
		EFM_ASSERT(false);
	}
}

/***************************************************************************//**
 * @brief
 *  The IRQ handler for LEUART0.
 *
 * @details
 *  This function is automatically called and then calls the other IF functions based on
 *  the current IF register status.
 *
 * @note
 *  This can be adjusted easily for other state machines.
 ******************************************************************************/

void LEUART0_IRQHandler(void){
	int int_flag = LEUART0->IF & LEUART0->IEN;
	LEUART0->IFC = int_flag;

	if(int_flag & LEUART_IF_TXBL) {
		leuart_txbl(&leuart_state);
	}
	if(int_flag & LEUART_IF_TXC) {
		leuart_txc(&leuart_state);
	}
}

/***************************************************************************//**
 * @brief
 *  TXBL interrupt handler.
 *
 * @details
 *  This function uses the LEUART state machine developed in leuart.h and follows the
 *  previously generated state machine structure.
 *
 * @note
 *	If EFM_ASSERT(false) ever occurs, this means that somehow an invalid state has been reached.
 *
 * @param[in] leuart_state
 *  The LEUART state machine struct currently being used.
 ******************************************************************************/

static void leuart_txbl(LEUART_STATE_MACHINE *leuart_state){
	switch(leuart_state->state) {
		case EN_TX:
			LEUART0->IFC = LEUART_IF_TXBL;
			LEUART0->IEN |= LEUART_IF_TXBL;
			leuart_state->state = TX_DATA;
			break;
		case TX_DATA:
			leuart_app_transmit_byte(leuart_state->leuart, leuart_state->string[leuart_state->count]);
			leuart_state->count++;
			if (leuart_state->count == leuart_state->length) {
				leuart_state->leuart->IEN &= ~LEUART_IF_TXBL;
				leuart_state->leuart->IFC = LEUART_IF_TXC;
				leuart_state->leuart->IEN |= LEUART_IF_TXC;
				leuart_state->state = END_TX;
			}
			break;
		case END_TX:
			EFM_ASSERT(false);
			break;
		default:
			EFM_ASSERT(false);
			break;
	}
}

/***************************************************************************//**
 * @brief
 *  TXC interrupt handler.
 *
 * @details
 *  This function uses the LEUART state machine developed in leuart.h and follows the
 *  previously generated state machine structure.
 *
 * @note
 *	If EFM_ASSERT(false) ever occurs, this means that somehow an invalid state has been reached.
 *
 * @param[in] leuart_state
 *  The LEUART state machine struct currently being used.
 ******************************************************************************/

static void leuart_txc(LEUART_STATE_MACHINE *leuart_state){
	switch(leuart_state->state) {
		case EN_TX:
			EFM_ASSERT(false);
			break;
		case TX_DATA:
			EFM_ASSERT(false);
			break;
		case END_TX:
			leuart_state->leuart->IEN &= ~LEUART_IEN_TXC;
			add_scheduled_event(leuart_state->callback);
			sleep_unblock_mode(LEUART_TX_EM);
			leuart_state->state = EN_TX;
			leuart_state->busy = false;
			break;
		default:
			EFM_ASSERT(false);
			break;
	}
}

/***************************************************************************//**
 * @brief
 *  The function to set up the state machine.
 *
 * @details
 *  This function uses the input values to set up the state machine structure.
 *
 * @note
 *  The state machine states are based on the private DEFINED_STATES enum.
 *
 * @param[in] leuart
 *  The LEUART system currently being used.
 *
 * @param[in] string
 *  The string to be sent to the HM-10.
 *
 * @param[in] string_len
 *  The length of the string to be sent in case the string is not null-terminated.
 ******************************************************************************/

void leuart_start(LEUART_TypeDef *leuart, char *string, uint32_t string_len){
	while(leuart->SYNCBUSY);

	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();

	sleep_block_mode(LEUART_TX_EM);

	leuart_state.callback = tx_done_evt;
	leuart_state.count = 0;
	leuart_state.length = string_len;
	leuart_state.leuart = leuart;
	leuart_state.state = EN_TX;
	strcpy(leuart_state.string, string);
	leuart_state.busy = true;
	leuart->IEN |= LEUART_IEN_TXBL;
	CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 *	Returns the busy status of the LEUART.
 ******************************************************************************/

bool leuart_tx_busy(LEUART_TypeDef *leuart){
	return leuart_state.busy;
}

/***************************************************************************//**
 * @brief
 *   LEUART STATUS function returns the STATUS of the peripheral for the
 *   TDD test
 *
 * @details
 * 	 This function enables the LEUART STATUS register to be provided to
 * 	 a function outside this .c module.
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 * @return
 * 	 Returns the STATUS register value as an uint32_t value
 *
 ******************************************************************************/

uint32_t leuart_status(LEUART_TypeDef *leuart){
	uint32_t	status_reg;
	status_reg = leuart->STATUS;
	return status_reg;
}

/***************************************************************************//**
 * @brief
 *   LEUART CMD Write sends a command to the CMD register
 *
 * @details
 * 	 This function is used by the TDD test function to program the LEUART
 * 	 for the TDD tests.
 *
 * @note
 *   Before exiting this function to update  the CMD register, it must
 *   perform a SYNCBUSY while loop to ensure that the CMD has by synchronized
 *   to the lower frequency LEUART domain.
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 * @param[in] cmd_update
 * 	 The value to write into the CMD register
 *
 ******************************************************************************/

void leuart_cmd_write(LEUART_TypeDef *leuart, uint32_t cmd_update){

	leuart->CMD = cmd_update;
	while(leuart->SYNCBUSY);
}

/***************************************************************************//**
 * @brief
 *   LEUART IF Reset resets all interrupt flag bits that can be cleared
 *   through the Interrupt Flag Clear register
 *
 * @details
 * 	 This function is used by the TDD test program to clear interrupts before
 * 	 the TDD tests and to reset the LEUART interrupts before the TDD
 * 	 exits
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 ******************************************************************************/

void leuart_if_reset(LEUART_TypeDef *leuart){
	leuart->IFC = 0xffffffff;
}

/***************************************************************************//**
 * @brief
 *   LEUART App Transmit Byte transmits a byte for the LEUART TDD test
 *
 * @details
 * 	 The BLE module will respond to AT commands if the BLE module is not
 * 	 connected to the phone app.  To validate the minimal functionality
 * 	 of the LEUART peripheral, write and reads to the LEUART will be
 * 	 performed by polling and not interrupts.
 *
 * @note
 *   In polling a transmit byte, a while statement checking for the TXBL
 *   bit in the Interrupt Flag register is required before writing the
 *   TXDATA register.
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 * @param[in] data_out
 *   Byte to be transmitted by the LEUART peripheral
 *
 ******************************************************************************/

void leuart_app_transmit_byte(LEUART_TypeDef *leuart, uint8_t data_out){
	while (!(leuart->IF & LEUART_IF_TXBL));
	leuart->TXDATA = data_out;
}


/***************************************************************************//**
 * @brief
 *   LEUART App Receive Byte polls a receive byte for the LEUART TDD test
 *
 * @details
 * 	 The BLE module will respond to AT commands if the BLE module is not
 * 	 connected to the phone app.  To validate the minimal functionality
 * 	 of the LEUART peripheral, write and reads to the LEUART will be
 * 	 performed by polling and not interrupts.
 *
 * @note
 *   In polling a receive byte, a while statement checking for the RXDATAV
 *   bit in the Interrupt Flag register is required before reading the
 *   RXDATA register.
 *
 * @param[in] leuart
 *   Defines the LEUART peripheral to access.
 *
 * @return
 * 	 Returns the byte read from the LEUART peripheral
 *
 ******************************************************************************/

uint8_t leuart_app_receive_byte(LEUART_TypeDef *leuart){
	uint8_t leuart_data;
	while (!(leuart->IF & LEUART_IF_RXDATAV));
	leuart_data = leuart->RXDATA;
	return leuart_data;
}
