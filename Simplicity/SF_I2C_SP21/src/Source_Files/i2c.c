/**
 * @file i2c.c
 * @author Sam Freed
 * @date February 23, 2021
 * @brief Contains all I2C driver functions.
 */

// Includes
#include "i2c.h"
#include "em_i2c.h"
#include "em_cmu.h"

// Defined files

// Private Variables
typedef enum {
	Start_CMD,
	Read_CMD,
	Wait_Read,
	End_Sense,
	Stop,
} DEFINED_STATES;

I2C_STATE_MACHINE i2c_state;

// Private functions
static void i2c_bus_reset(I2C_TypeDef *i2c);
static void i2c_ack_sm(I2C_STATE_MACHINE *i2c);
static void i2c_nack_sm(I2C_STATE_MACHINE *i2c);
static void i2c_mstop_sm(I2C_STATE_MACHINE *i2c);
static void i2c_rxdatav_sm(I2C_STATE_MACHINE *i2c);

/*************************************************//**
 * @brief
 *	Bus reset for I2C.
 *
 * @details
 *	This function sends an abort signal, disables interrupts, resets the I2C system,
 *	and then restores the interrupt enable register.
 *
 * @note
 *	This function is only based on pre-defined macros from board-specific libraries; if used
 *	on another board, ensure that these macros are matched.
 *
 *	@param[in] i2c
 *	The I2C system currently being used.
 ***************************************************/
void i2c_bus_reset(I2C_TypeDef * i2c) {
	i2c->CMD = I2C_CMD_ABORT;

	uint32_t ien_reg = i2c->IEN;
	i2c->IEN = false;

	i2c->IFC = i2c->IF;
	i2c->CMD = I2C_CMD_CLEARTX;
	i2c->CMD = I2C_CMD_START | I2C_CMD_STOP;
	while(!(i2c->IF & I2C_IF_MSTOP));
	i2c->IFC = i2c->IF;

	i2c->CMD = I2C_CMD_ABORT;
	i2c->IEN = ien_reg;
}

/*************************************************//**
 * @brief
 *	ACK interrupt handler.
 *
 * @details
 *	This function uses the I2C state machine developed in i2c.h and follows the state diagram
 *	previously generated.
 *
 * @note
 *	If EFM_ASSERT(false) ever occurs, this means that somehow an invalid state has been reached.
 *
 *	@param[in] i2c
 *	The I2C state machine struct currently being used.
 ***************************************************/
static void i2c_ack_sm(I2C_STATE_MACHINE *i2c){
	switch(i2c->state) {
	case Start_CMD:
		i2c->state = Read_CMD;
		i2c->I2Cn->TXDATA = i2c->target_reg;
		break;
	case Read_CMD:
		i2c->state = Wait_Read;
		i2c->I2Cn->CMD = I2C_CMD_START;
		i2c->I2Cn->TXDATA = (i2c->target_addr << 1) | I2C_READ;
		break;
	case Wait_Read:
		i2c->state = End_Sense;
		break;
	case End_Sense:
		EFM_ASSERT(false);
		break;
	case Stop:
		EFM_ASSERT(false);
		break;
	default:
		EFM_ASSERT(false);
		break;
	}
}

/*************************************************//**
 * @brief
 *	NACK interrupt handler.
 *
 * @details
 *	This function uses the I2C state machine developed in i2c.h and follows the state diagram
 *	previously generated.
 *
 * @note
 *	If EFM_ASSERT(false) ever occurs, this means that somehow an invalid state has been reached.
 *
 *	@param[in] i2c
 *	The I2C state machine struct currently being used.
 ***************************************************/
static void i2c_nack_sm(I2C_STATE_MACHINE *i2c){
	switch(i2c->state) {
	case Start_CMD:
		EFM_ASSERT(false);
		break;
	case Read_CMD:
		EFM_ASSERT(false);
		break;
	case Wait_Read:
		i2c->I2Cn->CMD = I2C_CMD_START;
		i2c->I2Cn->TXDATA = (i2c->target_addr << 1) | I2C_READ;
		break;
	case End_Sense:
		EFM_ASSERT(false);
		break;
	case(Stop):
		EFM_ASSERT(false);
		break;
	default:
		EFM_ASSERT(false);
		break;
	}
}

/*************************************************//**
 * @brief
 *	MSTOP interrupt handler.
 *
 * @details
 *	This function uses the I2C state machine developed in i2c.h and follows the state diagram
 *	previously generated.
 *
 * @note
 *	If EFM_ASSERT(false) ever occurs, this means that somehow an invalid state has been reached.
 *
 *	@param[in] i2c
 *	The I2C state machine struct currently being used.
 ***************************************************/
static void i2c_mstop_sm(I2C_STATE_MACHINE *i2c){
	switch(i2c->state) {
	case Start_CMD:
		EFM_ASSERT(false);
		break;
	case Read_CMD:
		EFM_ASSERT(false);
		break;
	case Wait_Read:
		EFM_ASSERT(false);
		break;
	case End_Sense:
		EFM_ASSERT(false);
		break;
	case Stop:
		sleep_unblock_mode(EM2);
		add_scheduled_event(i2c->cb);
		break;
	default:
		EFM_ASSERT(false);
		break;
	}
}

/*************************************************//**
 * @brief
 *	RXDATAV interrupt handler.
 *
 * @details
 *	This function uses the I2C state machine developed in i2c.h and follows the state diagram
 *	previously generated.
 *
 * @note
 *	If EFM_ASSERT(false) ever occurs, this means that somehow an invalid state has been reached.
 *	This function is also only set up to handle 2 bytes at the moment - in theory, if an I2C system
 *	that needs more than that is implemented, replacing the = in the bytes > 0 if clause with an |=
 *	would fix this; however, this change caused errors in the current implementation, so it was not implemented.
 *
 *	@param[in] i2c
 *	The I2C state machine struct currently being used.
 ***************************************************/
static void i2c_rxdatav_sm(I2C_STATE_MACHINE *i2c) {
	switch(i2c->state) {
	case(Start_CMD):
			EFM_ASSERT(false);
			break;
	case(Read_CMD):
			EFM_ASSERT(false);
			break;
	case(Wait_Read):
			EFM_ASSERT(false);
			break;
	case(End_Sense):
			i2c->bytes -= 1;
			if(i2c->bytes > 0) {
				*i2c->data_addr = (i2c->I2Cn->RXDATA) << (8*i2c->bytes);
				i2c->I2Cn->CMD = I2C_CMD_ACK;
			}
			else {
				*i2c->data_addr |= i2c->I2Cn->RXDATA;
				i2c->I2Cn->CMD = I2C_CMD_NACK;
				i2c->I2Cn->CMD = I2C_CMD_STOP;
				i2c->state = Stop;
			}
			break;
	case(Stop):
			EFM_ASSERT(false);
			break;
	default:
		EFM_ASSERT(false);
		break;
	}
}

//////////////// Global Functions
/*************************************************//**
 * @brief
 *	Initiates the I2C system.
 *
 * @details
 *	This function mirrors the letimer_pwm_open function, setting up the I2C TypeDef default structure
 *	and its interrupt routine structures.
 *
 * @note
 *	This function only sets up the I2C system but does not actually initialize the system for communication- this is
 *	done in i2c_start();.
 *
 * @param[in] i2c
 * 	The I2C system currently being used.
 *
 * @parar[in] i2c_setup
 * 	The open struct that will have its data values modified, initiated outside of this function.
 ***************************************************/
void i2c_open(I2C_TypeDef *i2c, I2C_OPEN_STRUCT *i2c_setup) {
	I2C_Init_TypeDef i2c_init_values;

	if(i2c == I2C0) {
		CMU_ClockEnable(cmuClock_I2C0, true);
	} else if (i2c == I2C1) {
		CMU_ClockEnable(cmuClock_I2C1, true);
	} else {
		EFM_ASSERT(false);
	}

	if ((i2c->IF & 0x01) == 0) {
		i2c->IFS = 0x01;
		EFM_ASSERT(i2c->IF & 0x01);
		i2c->IFC = 0x01;
	} else {
		i2c->IFC = 0x01;
		EFM_ASSERT(!(i2c->IF & 0x01));
	}

	i2c_init_values.clhr = i2c_setup->clhr;
	i2c_init_values.enable = i2c_setup->enable;
	i2c_init_values.freq = i2c_setup->freq;
	i2c_init_values.master = i2c_setup->master;
	i2c_init_values.refFreq = i2c_setup->refFreq;

	I2C_Init(i2c, &i2c_init_values);

	i2c->ROUTELOC0 = i2c_setup->out_pin_scl_route | i2c_setup->out_pin_sda_route;
	i2c->ROUTEPEN = (I2C_ROUTEPEN_SCLPEN*i2c_setup->out_pin_scl_en) | (I2C_ROUTEPEN_SDAPEN*i2c_setup->out_pin_sda_en);

	i2c_bus_reset(i2c);

	i2c->IFC = I2C_IF_ACK;
	i2c->IEN |= I2C_IF_ACK;

	i2c->IFC = I2C_IF_NACK;
	i2c->IEN |= I2C_IF_NACK;

	i2c->IFC = I2C_IF_MSTOP;
	i2c->IEN |= I2C_IF_MSTOP;

	i2c->IEN |= I2C_IF_RXDATAV;

	if(i2c == I2C0) {
		NVIC_EnableIRQ(I2C0_IRQn);
	} else if (i2c == I2C1) {
		NVIC_EnableIRQ(I2C1_IRQn);
	} else {
		EFM_ASSERT(false);
	}
}

/*************************************************//**
 * @brief
 *	The IRQ handler for I2C0.
 *
 * @details
 *	This function is automatically called and then calls the individual interrupt functions above
 *	based on the current IF register status.
 *
 * @note
 *	This can be adjusted easily for other state machines.
 ***************************************************/
void I2C0_IRQHandler(void) {
	int int_flag = I2C0->IF & I2C0->IEN;
	I2C0->IFC = int_flag;

	if(int_flag & I2C_IF_ACK) {
		i2c_ack_sm(&i2c_state);
	}
	if(int_flag & I2C_IF_NACK) {
		i2c_nack_sm(&i2c_state);
	}
	if(int_flag & I2C_IF_MSTOP) {
		i2c_mstop_sm(&i2c_state);
	}
	if(int_flag & I2C_IF_RXDATAV) {
		i2c_rxdatav_sm(&i2c_state);
	}
}

/*************************************************//**
 * @brief
 *	The IRQ handler for I2C0.
 *
 * @details
 *	This function is automatically called and then calls the individual interrupt functions above
 *	based on the current IF register status.
 *
 * @note
 *	This can be adjusted easily for other state machines.
 ***************************************************/
void I2C1_IRQHandler(void) {
	int int_flag = I2C1->IF & I2C1->IEN;
	I2C1->IFC = int_flag;

	if(int_flag & I2C_IF_ACK) {
		i2c_ack_sm(&i2c_state);
	}
	if(int_flag & I2C_IF_NACK) {
		i2c_nack_sm(&i2c_state);
	}
	if(int_flag & I2C_IF_MSTOP) {
		i2c_mstop_sm(&i2c_state);
	}
	if(int_flag & I2C_IF_RXDATAV) {
		i2c_rxdatav_sm(&i2c_state);
	}
}

/*************************************************//**
 * @brief
 *	The function to set up the state machine.
 *
 * @details
 *	This function pulls in values from the SI setup function and creates the state machine structure.
 *
 * @note
 *	The state machine states are based on the private DEFINED_STATES enum.
 *
 *	@param[in] i2c
 *	The I2C system currently being used.
 *
 *	@param[in] target_addr
 *	The I2C address of the desired peripheral.
 *
 *	@param[in] target_reg
 *	The command to be sent to the peripheral.
 *
 *	@param[in] w_r
 *	Bool for read/write.
 *
 *	@param[in] data_addr
 *	The address of where the read data should be stored.
 *
 *	@param[in] bytes
 *	The number of bytes expected to be read from the peripheral.
 *
 *	@param[in] cb
 *	The callback number for the scheduler.
 ***************************************************/
void i2c_start(I2C_TypeDef *i2c, uint32_t target_addr, uint32_t target_reg, bool w_r, uint32_t *data_addr, uint32_t bytes, uint32_t cb) {
	EFM_ASSERT((i2c->STATE & _I2C_STATE_STATE_MASK) == I2C_STATE_STATE_IDLE);
	sleep_block_mode(I2C_EM_BLOCK);

	i2c_state.state = Start_CMD;
	i2c_state.I2Cn = i2c;
	i2c_state.target_addr = target_addr;
	i2c_state.target_reg = target_reg;
	i2c_state.w_r = w_r;
	i2c_state.data_addr = data_addr;
	i2c_state.bytes = bytes;

	i2c_state.I2Cn->CMD = I2C_CMD_START;
	i2c_state.I2Cn->TXDATA = (i2c_state.target_addr << 1) | I2C_WRITE;
	i2c_state.busy = true;
	i2c_state.cb = cb;
}
