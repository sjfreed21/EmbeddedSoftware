/**
 * @file si7021.c
 * @author Sam Freed
 * @date February 25th, 2021
 * @brief Creates and populates the I2C_OPEN_STRUCT, then sets up I2C1.
 */

// Includes
#include "si7021.h"
#include "app.h"
#include "HW_delay.h"
#include "em_i2c.h"

// Defined files
//#define i2c_def_1

// Private Variables
uint32_t data;
uint32_t BYTE = 1;
uint32_t BYTES = 2;

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
	timer_delay(80);
	I2C_OPEN_STRUCT i2cStruct;

	i2cStruct.enable = true;
	i2cStruct.master = true;
	i2cStruct.out_pin_scl_en = true;
	i2cStruct.out_pin_sda_en = true;
	i2cStruct.out_pin_scl_route = SI_SCL_ROUTE;
	i2cStruct.out_pin_sda_route = SI_SDA_ROUTE;
	i2cStruct.freq = I2C_FREQ_FAST_MAX;
	i2cStruct.clhr = i2cClockHLRAsymetric;

	i2c_open(I2C1, &i2cStruct);
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
void si7021_hread(uint32_t read_cb) {
	i2c_start(I2C1, SI7021_TARGET_ADDR, SI7021_HUMID_CMD, I2C_READ, &data, BYTES, read_cb);
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
void si7021_tread(uint32_t read_cb) {
	i2c_start(I2C1, SI7021_TARGET_ADDR, SI7021_TEMP_RH, I2C_READ, &data, BYTES, read_cb);
}

/*************************************************//**
 * @brief
 *	Conversion from the I2C humidity value to float.
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

/*************************************************//**
 * @brief
 *	Conversion from the I2C temp value to float.
 *
 * @details
 *	Uses the calculation from the SI7021 datasheet to convert the raw value to degrees C.
 *
 * @note
 *	The view variable is used to display the value in the debugger and could be removed to save memory.
 *
 * @return
 * 	Returns a float of the temp in degrees C.
 ***************************************************/
float si7021_temp_conversion(void) {
	uint32_t view = data;
	float cel = ((175.72*view)/65536) - 46.85;
	return cel*1.8+32;
}

/*************************************************//**
* @brief
* 	Test function for the SI7021 I2C peripheral.
*
* @details
*	This function uses Test-Driven Development methods to ensure the proper operation
*	of I2C reads and writes to the SI7021 temperature and humidity peripheral. If any of the
*	EFM_ASSERT statements are triggered, an unexpected error has occurred (or things are working
*	properly but the ASSERT condition is incorrect).
*
* @note
* 	All defines are either defined in this file, in the corresponding header file, or are system defined.
***************************************************/
bool si7021_test(uint32_t read_cb) {

	// Test Read
	uint32_t prev = data;
	i2c_start(I2C1, SI7021_TARGET_ADDR, SI7021_UREAD_CMD, I2C_READ, &data, BYTE, read_cb);
	while(i2c_check_busy(I2C1));
	EFM_ASSERT(data == 0x3a || data == prev);

	// Test Write
	data = RESOLUTION_CONFIG;
	i2c_start(I2C1, SI7021_TARGET_ADDR, SI7021_UWRITE_CMD, I2C_WRITE, &data, BYTE, read_cb);
	timer_delay(15);
	while(i2c_check_busy(I2C1));
	EFM_ASSERT(data == RESOLUTION_CONFIG);

	// Test Read
	i2c_start(I2C1, SI7021_TARGET_ADDR, SI7021_UREAD_CMD, I2C_READ, &data, BYTE, read_cb);
	while(i2c_check_busy(I2C1));
	EFM_ASSERT(data == RESOLUTION_8_12);

	// Test Read Humidity
	i2c_start(I2C1, SI7021_TARGET_ADDR, SI7021_HUMID_CMD, I2C_READ, &data, BYTES, read_cb);
	while(i2c_check_busy(I2C1));
	float hum = si7021_conversion();
	EFM_ASSERT((hum > 10.0) && (hum < 50.0));

	// Test Read Temp
	i2c_start(I2C1, SI7021_TARGET_ADDR, SI7021_TEMP_CMD, I2C_READ, &data, BYTES, SI7021_TREAD_CB);
	while(i2c_check_busy(I2C1));
	float temp = si7021_temp_conversion();
	EFM_ASSERT((temp > 50.0) && (temp < 90.0));

	return true;
}
