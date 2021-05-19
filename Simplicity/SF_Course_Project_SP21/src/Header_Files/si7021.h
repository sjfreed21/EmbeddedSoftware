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
#define SI7021_HUMID_CMD	0xF5
#define SI7021_TEMP_CMD 	0xF3
#define SI7021_UREAD_CMD	0xE7
#define SI7021_UWRITE_CMD	0xE6
#define RESOLUTION_CONFIG 	0x01
#define RESOLUTION_8_12		0x3B

#define SI7021_TEMP_RH		0xE0

// Global variables

// Function prototypes
void si7021_i2c_open();
void si7021_hread(uint32_t read_cb);
void si7021_tread(uint32_t read_cb);
float si7021_conversion(void);
float si7021_temp_conversion(void);
bool si7021_test(uint32_t read_cb);


#endif /* SRC_HEADER_FILES_SI7021_H_ */
