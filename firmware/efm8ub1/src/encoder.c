#include <SI_EFM8UB1_Register_Enums.h>
#include "board.h"
#include "efm8_usb.h"

int8_t encoderCount = 0;
uint8_t internalEncoderStatus = 0;
uint8_t i2cEncoderStatus = 0;
uint8_t hidEncoderStatus = 0;

static uint8_t prevAB;
static uint8_t prevA;
static uint8_t prevB;
static uint8_t prevSW = 0x7f;

void encoder_enterBootloader() {
	USBD_Stop();
	// Write R0 and issue a software reset
	SFRPAGE = 0;
	*((uint8_t SI_SEG_DATA *)0x00) = 0xA5;
	RSTSRC = RSTSRC_SWRSF__SET | RSTSRC_PORSF__SET;
}

void encoder_init()
{
	// If the encoder switch is pushed down on powerup, reset into bootloader
	if (!ENC_SW) {
		encoder_enterBootloader();
	}

	// TODO(supersat): Figure out why these are not initialized on reset automatically
	encoderCount = 0;
	internalEncoderStatus = 0;
	i2cEncoderStatus = 0;
	hidEncoderStatus = 0;

	prevA = ENC_A ? 0xff : 0x00;
	prevB = ENC_B ? 0xff : 0x00;
	prevAB = (prevB & 0xAA) | (prevA & 0x55);
	prevSW = 0x7f;
}

void encoder_updateStatus()
{
	encoder_updateInternalStatus();
	IE_EA = 0;
	encoder_updateExternalStatus(&i2cEncoderStatus);
	encoder_updateExternalStatus(&hidEncoderStatus);
	IE_EA = 1;
}

// Loosely based on code by Oleg Mazurov
// returns non-zero when the encoder status changes
void encoder_updateInternalStatus()
{
	SI_SEGMENT_VARIABLE(statusDecoder[], static const int8_t, SI_SEG_CODE) =
		{ 0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0 };
	uint8_t result;
	// Debounce the push button
	prevSW = (prevSW & 0x80) | ((prevSW << 1) & 0x7e) | ENC_SW;
	if (prevSW & 0x80) {
		if ((prevSW & 0x7f) == 0x7f) {
			prevSW &= ~0x80;
			result = ENCODER_UP;
		} else {
			result = ENCODER_PRESSED;
		}
	} else {
		if ((prevSW & 0x7f) == 0) {
			prevSW |= 0x80;
			result = ENCODER_DOWN | ENCODER_PRESSED;
		} else {
			result = 0;
		}
	}

	prevA = (prevA << 1) | ENC_A;
	prevB = (prevB << 1) | ENC_B;
	// Debounce the A and B switches, and only update the encoder status when stable
	if ((prevA == 0x00 || prevA == 0xff) && (prevB == 0x00 || prevB == 0xff)) {
		prevAB = (prevAB << 2) | (prevB & 2) | (prevA & 1);
		encoderCount += statusDecoder[prevAB & 0xf];
		if (encoderCount == -4) {
			encoderCount = 0;
			result |= ENCODER_CCW;
		} else if (encoderCount == 4) {
			encoderCount = 0;
			result |= ENCODER_CW;
		}
	}

	internalEncoderStatus = result;
}

void encoder_updateExternalStatus(uint8_t *status)
{
	uint8_t prevStatus = *status;

	if (internalEncoderStatus & ENCODER_CW) {
		if (*status & ENCODER_CCW) {
			*status &= ~ENCODER_CCW;
		} else {
			*status |= ENCODER_CW;
		}
	}

	if (internalEncoderStatus & ENCODER_CCW) {
		if (*status & ENCODER_CW) {
			*status &= ~ENCODER_CW;
		} else {
			*status |= ENCODER_CCW;
		}
	}

	if (internalEncoderStatus & ENCODER_DOWN) {
		if (*status & ENCODER_UP) {
			*status &= ~ENCODER_UP;
		} else {
			*status |= ENCODER_DOWN;
		}
	}

	if (internalEncoderStatus & ENCODER_UP) {
		if (*status & ENCODER_DOWN) {
			*status &= ~ENCODER_DOWN;
		} else {
			*status |= ENCODER_UP;
		}
	}

	if (!(*status & ENCODER_DIRTY)) {
		if (!(internalEncoderStatus & ENCODER_CW)) {
			*status &= ~ENCODER_CW;
		}

		if (!(internalEncoderStatus & ENCODER_CCW)) {
			*status &= ~ENCODER_CCW;
		}

		if (!(internalEncoderStatus & ENCODER_DOWN)) {
			*status &= ~ENCODER_DOWN;
		}

		if (!(internalEncoderStatus & ENCODER_UP)) {
			*status &= ~ENCODER_UP;
		}

		if (internalEncoderStatus & ENCODER_PRESSED) {
			*status |= ENCODER_PRESSED;
		} else {
			*status &= ~ENCODER_PRESSED;
		}
	}

	if (prevStatus != *status) {
		*status |= ENCODER_DIRTY;
	}
}
