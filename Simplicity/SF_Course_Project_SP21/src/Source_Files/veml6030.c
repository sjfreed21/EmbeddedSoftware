/**
 * @file veml6030.c
 * @author Sam Freed
 * @date April 22nd, 2021
 * @brief Code for running and reading from the VEML6030 ambient light sensor.
 */


// Includes
#include "veml6030.h"
#include "HW_delay.h"
#include "em_i2c.h"

// Defined files

// Private Variables
uint32_t data;
uint32_t VE_BYTES = 2;
bool READ = true;
bool WRITE = false;

// Private functions

// Global Functions
/*************************************************//**
 * @brief
 *	Beginning to enable the VEML peripheral.
 *
 * @details
 *	This function sets up the open struct and passes it to the I2C open function.
 *
 * @note
 *	SCL_ROUTE and SDA_ROUTE are defined in brd_config.h.
 ***************************************************/
void veml6030_i2c_open(){
	I2C_OPEN_STRUCT i2cStruct;

	i2cStruct.enable = true;
	i2cStruct.master = true;
	i2cStruct.out_pin_scl_en = true;
	i2cStruct.out_pin_sda_en = true;
	i2cStruct.out_pin_scl_route = VE_SCL_ROUTE;
	i2cStruct.out_pin_sda_route = VE_SDA_ROUTE;
	i2cStruct.freq = I2C_FREQ_FAST_MAX;
	i2cStruct.clhr = i2cClockHLRAsymetric;

	i2c_open(I2C0, &i2cStruct);
}

/*************************************************//*
 * @brief
 *	Starts the I2C state machine for the VEML.
 *
 * @details
 *	This function calls i2c_start, which sets up the I2C state machine with passed in values.
 *
 * @note
 * 	All of the capitalized values are predefined, either by the board defines or by brd_config.h.
 *
 * @param[in] read_cb
 * 	The callback number for the scheduler.
 *
 ***************************************************/
//bool veml6030_startup(uint32_t read_cb) {
//	data = START_UP_COMMAND;
//	i2c_start(VEML6030_I2C, VEML6030_ADDR, START_UP_COMMAND, WRITE, &data, VE_BYTES, read_cb);
//	while(i2c_check_busy(VEML6030_I2C));
//	timer_delay(15);
//	EFM_ASSERT(data == START_UP_COMMAND);
//	return true;
//}

/*************************************************//**
 * @brief
 *	Starts the I2C state machine for the VEML read.
 *
 * @details
 *	This function calls i2c_start, which sets up the I2C state machine with passed in values.
 *
 * @note
 * 	All of the capitalized values are predefined, either by the board defines or by brd_config.h.
 *
 * @param[in] read_cb
 * 	The callback number for the scheduler.
 *
 ***************************************************/
void veml6030_read(uint32_t read_cb){
	i2c_start(VEML6030_I2C, VEML6030_ADDR, READ_COMMAND, READ, &data, VE_BYTES, read_cb);
}

/*************************************************//**
 * @brief
 *	Starts the I2C state machine for the VEML write.
 *
 * @details
 *	This function calls i2c_start, which sets up the I2C state machine with passed in values.
 *
 * @note
 * 	All of the capitalized values are predefined, either by the board defines or by brd_config.h.
 *
 * @param[in] read_cb
 * 	The callback number for the scheduler.
 *
 ***************************************************/
void veml6030_write(uint32_t write_cb) {
	i2c_start(VEML6030_I2C, VEML6030_ADDR, START_UP_COMMAND, WRITE, &data, VE_BYTES, write_cb);
}

/*************************************************//**
 * @brief
 *	Conversion from the I2C lux value to uint32_t.
 *
 * @details
 *	Uses the calculation from the VEML datasheet to convert the raw value to lux.
 *
 * @note
 *	The result variable is used to display the value in the debugger and could be removed to save memory.
 *
 * @return
 * 	Returns a uint32_t of the Lux value.
 ***************************************************/
uint32_t veml6030_conversion(void) {
	uint32_t result = data;
	return 0.0576*result;
}
