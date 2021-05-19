/**
 * @file si7021.c
 * @author Sam Freed
 * @date Februrary 25th, 2021
 * @brief Creates and populates the I2C_OPEN_STRUCT, then sets up I2C0.
 */

// Includes
#include "si7021.h"
#include "em_i2c.h"

// Defined files
//#define i2c_def_1

// Private Variables
uint32_t data;
uint32_t BYTES = 2;
bool W_R = true;

// Private functions

// Global Functions
/*************************************************//**
 * @brief
 *	Beginning to enable the SI7021 peripheral.
 *
 * @details
 *	This function sets up the open struct and passes it to the I2C open function.
 *
 * @note
 *	SCL_ROUTE and SDA_ROUTE are defined in brd_config.h.
 ***************************************************/
void si7021_i2c_open(){
	I2C_OPEN_STRUCT i2cStruct;

	i2cStruct.enable = true;
	i2cStruct.master = true;
	i2cStruct.out_pin_scl_en = true;
	i2cStruct.out_pin_sda_en = true;
	i2cStruct.out_pin_scl_route = SCL_ROUTE;
	i2cStruct.out_pin_sda_route = SDA_ROUTE;
	i2cStruct.freq = I2C_FREQ_FAST_MAX;
	i2cStruct.clhr = i2cClockHLRAsymetric;

	i2c_open(I2C0, &i2cStruct);
}

/*************************************************//**
 * @brief
 *	Starts the I2C state machine for the SI7021.
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
void si7021_read(uint32_t read_cb) {
	i2c_start(I2C0, SI7021_TARGET_ADDR, SI7021_CMD, W_R, &data, BYTES, read_cb);
}

/*************************************************//**
 * @brief
 *	Conversion from the I2C value to float.
 *
 * @details
 *	Uses the calculation from the SI7021 datasheet to convert the raw value to %RH.
 *
 * @note
 *	The view variable is used to display the value in the debugger and could be removed to save memory.
 *
 * @return
 * 	Returns a float of the %RH.
 ***************************************************/
float si7021_conversion(void) {
	uint32_t view = data;
	return ((125*view)/65536)-6;
}
