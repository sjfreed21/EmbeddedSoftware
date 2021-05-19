
#include "main.h"
#include "scheduler.h"
#include "sleep_routines.h"

int main(void)
{
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;

  /* Chip errata */
  CHIP_Init();

  /* Init DCDC regulator and HFXO with kit specific parameters */
  /* Init DCDC regulator and HFXO with kit specific parameters */
  /* Initialize DCDC. Always start in low-noise mode. */
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);
  em23Init.vScaleEM23Voltage = emuVScaleEM23_LowPower;
  EMU_EM23Init(&em23Init);
  CMU_HFXOInit(&hfxoInit);

  /* Switch HFCLK to HFRCO and disable HFRCO */
  CMU_HFRCOBandSet(MCU_HFXO_FREQ);					// Set main CPU oscillator frequency
  CMU_OscillatorEnable(cmuOsc_HFRCO, true, true);
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);
  CMU_OscillatorEnable(cmuOsc_HFXO, false, false);

  /* Call application program to open / initialize all required peripheral */
  app_peripheral_setup();

  /* Infinite blink loop */
  while (1) {
//	  EMU_EnterEM1();
//	  EMU_EnterEM2(true);

	  if(!get_scheduled_events()) {
		  CORE_DECLARE_IRQ_STATE;
		  CORE_ENTER_CRITICAL();
		  enter_sleep();
		  CORE_EXIT_CRITICAL();
	  }
	  if(get_scheduled_events() == LETIMER0_UF_CB) {
		  scheduled_letimer0_uf_cb();
	  }
	  if(get_scheduled_events() == LETIMER0_COMP0_CB) {
		  scheduled_letimer0_comp0_cb();
	  }
	  if(get_scheduled_events() == LETIMER0_COMP1_CB) {
		  scheduled_letimer0_comp1_cb();
	  }
	  if(get_scheduled_events() == SI7021_HREAD_CB) {
		  scheduled_si7021_hread_cb();
	  }
	  if(get_scheduled_events() == SI7021_TREAD_CB) {
		  scheduled_si7021_tread_cb();
	  }
	  if(get_scheduled_events() == BOOT_UP_CB) {
		  scheduled_boot_up_cb();
	  }
	  if(get_scheduled_events() == BLE_TX_DONE_CB) {
		  scheduled_ble_tx_done_cb();
	  }
	  if(get_scheduled_events() == VEML6030_READ_CB) {
		  scheduled_veml6030_read_cb();
	  }
  }
}
