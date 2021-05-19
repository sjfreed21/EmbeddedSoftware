/*
 * veml6030.h
 *
 *  Created on: Apr 22, 2021
 *      Author: sjfre
 */

#ifndef SRC_HEADER_FILES_VEML6030_H_
#define SRC_HEADER_FILES_VEML6030_H_

// Include Statements
#include "em_cmu.h"
#include "em_assert.h"
#include "cmu.h"
#include "gpio.h"
#include "i2c.h"
#include "brd_config.h"

// Defined files
#define VEML6030_ADDR		0x48
#define	VEML6030_I2C 		I2C0
#define	START_UP_COMMAND	0x0
#define READ_COMMAND		0x4

// Global variables

// Function prototypes
void veml6030_i2c_open();
void veml6030_write(uint32_t write_cb);
void veml6030_read(uint32_t read_cb);
uint32_t veml6030_conversion(void);


#endif /* SRC_HEADER_FILES_VEML6030_H_ */
