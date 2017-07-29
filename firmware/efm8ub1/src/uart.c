#include "board.h"

serialFifo_t rxFifo;
serialFifo_t txFifo;

uint32_t uartBPS = 115200;
uint8_t uartStopBits = 0;
uint8_t uartParityType = 0;
uint8_t uartDataBits = 8;
uint8_t uartFlags;

void uart_init()
{
	// TODO(supersat): Figure out why these aren't initialized on reset properly
	uartBPS = 115200;
	uartStopBits = 0;
	uartParityType = 0;
	uartDataBits = 8;
	uartFlags = 0;

	fifo_reset(&rxFifo, sizeof(rxFifo.buf));
	fifo_reset(&txFifo, sizeof(txFifo.buf));
}

//-----------------------------------------------------------------------------
// UART0_ISR
//-----------------------------------------------------------------------------
//
// UART0 ISR Content goes here. Remember to clear flag bits:
// SCON0::RI (Receive Interrupt Flag)
// SCON0::TI (Transmit Interrupt Flag)
//
//-----------------------------------------------------------------------------
SI_INTERRUPT (UART0_ISR, UART0_IRQn)
{
	uint8_t byte;

	if (SCON0_RI == 1) {
		// SCON0_RI is automatically cleared
		byte = SBUF0;
		if (fifo_getFree(&rxFifo)) {
			fifo_putData(&rxFifo, &byte, 1);
		} else {
			ledFrames[0].red = 0xff;
			ledFlags |= ledFlags;
		}
	}

	if (SCON0_TI == 1) {
		SCON0_TI = 0; // Clear interrupt flag
		if (fifo_getLength(&txFifo)) {
			SBUF0 = *fifo_getContigData(&txFifo, 1);
		}
	}
}
