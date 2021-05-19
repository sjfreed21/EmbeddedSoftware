/*
 * i2c.h
 *
 *  Created on: Feb 23, 2021
 *      Author: sjfre
 */

#ifndef SRC_HEADER_FILES_I2C_H_
#define SRC_HEADER_FILES_I2C_H_

// Include Statements
#include "stdbool.h"
#include "em_i2c.h"
#include "em_assert.h"
#include "sleep_routines.h"
#include "scheduler.h"

// Defined files
#define	I2C_EM_BLOCK 	EM2
#define I2C_READ		1
#define I2C_WRITE		0

// Global variables
typedef struct {
	bool					enable;
	bool					master;
	uint32_t				refFreq;
	uint32_t				freq;
	I2C_ClockHLR_TypeDef	clhr;
	uint32_t				out_pin_sda_route;
	uint32_t				out_pin_scl_route;
	bool					out_pin_sda_en;
	bool					out_pin_scl_en;
} I2C_OPEN_STRUCT;

typedef struct {
	uint32_t 				state;
	I2C_TypeDef				*I2Cn;
	uint32_t				target_addr;
	uint32_t				target_reg;
	bool					w_r;
	uint32_t				*data_addr;
	uint32_t				bytes;
	bool					busy;
	uint32_t				cb;
} I2C_STATE_MACHINE;

// Function Prototypes
void i2c_open(I2C_TypeDef *i2c, I2C_OPEN_STRUCT *i2c_setup);
void I2C0_IRQHandler(void);
void I2C1_IRQHandler(void);
void i2c_start(I2C_TypeDef *i2c, uint32_t target_addr, uint32_t target_reg, bool w_r, uint32_t *data_addr, uint32_t bytes, uint32_t cb);
#endif /* SRC_HEADER_FILES_I2C_H_ */
