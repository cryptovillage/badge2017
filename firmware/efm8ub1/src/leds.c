#include <SI_EFM8UB1_Register_Enums.h>                  // SFR declarations
#include "InitDevice.h"
#include "board.h"

uint8_t ledBuf[32];
uint8_t ledBufPtr;
uint8_t ledFlags;
LEDFrame *ledFrames = (LEDFrame *)ledBuf;

SI_INTERRUPT (SPI0_ISR, SPI0_IRQn)
{
	SFRPAGE = 0x20;
	while (SPI0CN0 & SPI0CN0_TXNF) {
		if (ledBufPtr < 32) {
			SPI0DAT = ledBuf[ledBufPtr++];
		} else {
			SPI0DAT = 0xff;
			ledBufPtr++;
			if (ledBufPtr == 36) {
				SPI0FCN0 &= ~(1 << 7); // Disable TFRQE
				return;
			}
		}
	}
}

void leds_beginTx()
{
	ledFlags &= ~LED_FLAG_UPDATE_LEDS;
	ledBufPtr = 0;

	IE_EA = 0;
	SFRPAGE = 0x20;
	SPI0FCN0 |= (1 << 7); // Set TFRQE

	// Start frame
	SPI0DAT = 0x00;
	SPI0DAT = 0x00;
	SPI0DAT = 0x00;
	SPI0DAT = 0x00;

	IE_EA = 1;
}

void leds_init()
{
	uint8_t i;

	for (i = 0; i < 8; i++) {
		ledFrames[i].ones = 7;
		ledFrames[i].brightness = 16;
		ledFrames[i].blue = 0;
		ledFrames[i].green = 0;
		ledFrames[i].red = 0;
	}

	/*
	ledFrames[0].red = 0xff;
	ledFrames[1].red = 0xff;
	ledFrames[1].green = 0x80;
	ledFrames[2].red = 0xff;
	ledFrames[2].green = 0xff;
	ledFrames[3].green = 0xff;
	ledFrames[4].green = 0xff;
	ledFrames[4].blue = 0x80;
	ledFrames[5].blue = 0xff;
	ledFrames[6].blue = 0xff;
	ledFrames[6].red = 0xff;
	ledFrames[7].blue = 0xff;
	ledFrames[7].green = 0xff;
	ledFrames[7].red = 0xff;
	*/

	ledBufPtr = 36;
}

void leds_set_charging_pattern(uint8_t brightness)
{
	ledFrames[2].red = brightness;
	ledFrames[2].green = 0;
	ledFrames[3].red = brightness;
	ledFrames[3].green = 0;
	ledFlags |= LED_FLAG_UPDATE_LEDS;
}

void leds_set_charged_pattern(uint8_t brightness)
{
	ledFrames[2].red = 0;
	ledFrames[2].green = brightness;
	ledFrames[3].red = 0;
	ledFrames[3].green = brightness;
	ledFlags |= LED_FLAG_UPDATE_LEDS;
}

uint8_t leds_areBusy()
{
	return ledBufPtr < 36;
}
