/*
 * si7021.h
 *
 *  Created on: Feb 25, 2021
 *      Author: sjfre
 */

#ifndef SRC_HEADER_FILES_SI7021_H_
#define SRC_HEADER_FILES_SI7021_H_

// Include Statements
#include "em_cmu.h"
#include "em_assert.h"
#include "cmu.h"
#include "gpio.h"
#include "i2c.h"
#include "brd_config.h"


// Defined files
#define SI7021_TARGET_ADDR	0x40
#define SI7021_CMD			0xF5

// Global variables

// Function prototypes
void si7021_i2c_open();
void si7021_read(uint32_t read_cb);
float si7021_conversion(void);


#endif /* SRC_HEADER_FILES_SI7021_H_ */
