#include <SI_EFM8UB1_Register_Enums.h>
#include "board.h"

data uint8_t _cur_10khz_ctr = 0;
data uint16_t _cur_ms = 0;

SI_INTERRUPT(TIMER2_ISR, TIMER2_IRQn)
{
	TMR2CN0_TF2H = 0;
	_cur_10khz_ctr++;
	if (_cur_10khz_ctr >= 10) {
		_cur_10khz_ctr = 0;
		_cur_ms++;
	}
	internalEncoderStatus |= ENCODER_NEEDS_POLL;
}

// Returns ts2 - ts1. The difference must be within 16384 ms to be valid.
int16_t msDiff(uint16_t ts1, uint16_t ts2)
{
	int16_t result = ts2 - ts1;

	if (result > 16384) {
		result -= 32768;
	} else if (result < -16384) {
		result += 32768;
	}

	return result;
}
