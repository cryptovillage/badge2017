//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <SI_EFM8UB1_Register_Enums.h>                  // SFR declarations
#include "InitDevice.h"
#include "board.h"
// $[Generated Includes]
#include "efm8_usb.h"
// [Generated Includes]$

uint8_t rxHIDBuf[64];
uint8_t txBuf[64];

void SiLabs_Startup()
{
	// Disable the watchdog
	SFRPAGE = 0x00;
	WDTCN = 0xDE; //First key
	WDTCN = 0xAD; //Second key
}

//-----------------------------------------------------------------------------
// main() Routine
// ----------------------------------------------------------------------------
int main(void) {
	// Call hardware initialization routine
	enter_DefaultMode_from_RESET();

	// Initialize the LEDs ASAP to minimize flicker
	leds_init();
	leds_beginTx();

	// Bring up the rest of the system
	// Init the I2C interface and prepare the headphone amp
	// for ESP32 control before allowing the ESP32 to boot
	i2c_init();
	i2c_initHeadphoneAmp();
	power_init();
	encoder_init();
	uart_init();

	while (1) {
// $[Generated Run-time code]
// [Generated Run-time code]$
		power_update();

		// FIXME(supersat): For debugging... but without it, the UART fails?!
		//if (!leds_areBusy())
		//	leds_beginTx();
		if ((ledFlags & LED_FLAG_UPDATE_LEDS) && !leds_areBusy())
			leds_beginTx();

 		if (internalEncoderStatus & ENCODER_NEEDS_POLL)
 			encoder_updateStatus();

		/*
		if (internalEncoderStatus & ENCODER_CCW) {
			if (backlightBrightness) {
				backlightBrightness--;
				SFRPAGE = 0;
				PCA0CPH0 = backlightBrightness;
			}
		}

		if (internalEncoderStatus & ENCODER_CW) {
			if (backlightBrightness != 0xff) {
				backlightBrightness++;
				SFRPAGE = 0;
				PCA0CPH0 = backlightBrightness;
			}
		}
		*/

		if ((hidEncoderStatus & ENCODER_DIRTY) && !USBD_EpIsBusy(EP3IN)) {
			hid_sendBadgeINReport();
			hidEncoderStatus &= ~ENCODER_DIRTY;
		}

		// Disable interrupts to make fifo operations atomic
		// TODO(supersat): Make fifo operations work with interrupts enabled?
		IE_EA = 0;
		if (!USBD_EpIsBusy(EP2IN)) {
			uint8_t rxFifoLength = fifo_getContigLength(&rxFifo);
			if (rxFifoLength) {
				USBD_Write(EP2IN, fifo_getContigData(&rxFifo, rxFifoLength), rxFifoLength, false);
			}
		}

		if (!USBD_EpIsBusy(EP2OUT) && fifo_getFree(&txFifo) > 64) {
			USBD_Read(EP2OUT, txBuf, 64, true);
		}
		IE_EA = 1;

		if (!USBD_EpIsBusy(EP3OUT)) {
			USBD_Read(EP3OUT, rxHIDBuf, 64, true);
		}

		i2c_update();

		// Enter idle mode to save power
		// Will resume on interrupt

		// The commented out lines will toggle the pull-up on ENC_A,
		// allowing us to monitor processor load/sleep time
		//P0 = 0x0c;
		idle();
		//P0 = 0x0e;

		// WDTCN = 0xA5;
	}
}
