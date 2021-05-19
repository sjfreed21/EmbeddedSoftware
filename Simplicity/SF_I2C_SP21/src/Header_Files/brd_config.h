#ifndef BRD_CONFIG_HG
#define BRD_CONFIG_HG

//***********************************************************************************
// Include files
//***********************************************************************************
/* System include statements */


/* Silicon Labs include statements */
#include "em_cmu.h"
#include "em_gpio.h"

/* The developer's include statements */


//***********************************************************************************
// defined files
//***********************************************************************************

// GPIO pin setup
#define STRONG_DRIVE

// LED 0 pin is
#define	LED0_PORT				gpioPortF
#define LED0_PIN				04u
#define LED0_DEFAULT			false 	// Default false (0) = off, true (1) = on
#define LED0_GPIOMODE			gpioModePushPull

// LED 1 pin is
#define LED1_PORT				gpioPortF
#define LED1_PIN				05u
#define LED1_DEFAULT			false	// Default false (0) = off, true (1) = on
#define LED1_GPIOMODE			gpioModePushPull

#ifdef STRONG_DRIVE
	#define LED0_DRIVE_STRENGTH		gpioDriveStrengthStrongAlternateStrong
	#define LED1_DRIVE_STRENGTH		gpioDriveStrengthStrongAlternateStrong
#else
	#define LED0_DRIVE_STRENGTH		gpioDriveStrengthWeakAlternateWeak
	#define LED1_DRIVE_STRENGTH		gpioDriveStrengthWeakAlternateWeak
#endif


// System Clock setup
#define MCU_HFXO_FREQ			cmuHFRCOFreq_19M0Hz


// LETIMER PWM Configuration
#define		PWM_ROUTE_0			LETIMER_ROUTELOC0_OUT0LOC_LOC28
#define		PWM_ROUTE_1			LETIMER_ROUTELOC0_OUT1LOC_LOC28

// Si7021 Config
#define		SI7021_SCL_PORT			gpioPortC
#define		SI7021_SCL_PIN			11u
#define		SI7021_SDA_PORT			gpioPortC
#define		SI7021_SDA_PIN			10u
#define		SI7021_SENSOR_EN_PORT	gpioPortB
#define		SI7021_SENSOR_EN_PIN	10u

#define		SI7021_DRIVE_STRENGTH	gpioDriveStrengthWeakAlternateWeak
#define		SI7021_SENSOR_GPIOMODE	gpioModePushPull
#define		SI7021_SENSOR_DEFAULT	true
#define		SI7021_I2C_GPIOMODE		gpioModeWiredAnd
#define		SI7021_I2C_DEFAULT		true


/* This didn't work...not sure what I need to do for this part. */
//#ifdef i2c_def_1
//	#define 	SCL_ROUTE				I2C_ROUTELOC0_SCLLOC_LOC19
//	#define		SDA_ROUTE				I2C_ROUTELOC0_SDALOC_LOC19
//#else
//	#define 	SCL_ROUTE				I2C_ROUTELOC0_SCLLOC_LOC15
//	#define		SDA_ROUTE				I2C_ROUTELOC0_SDALOC_LOC15
//#endif

#define 	SCL_ROUTE				I2C_ROUTELOC0_SCLLOC_LOC15
#define		SDA_ROUTE				I2C_ROUTELOC0_SDALOC_LOC15

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************

#endif
