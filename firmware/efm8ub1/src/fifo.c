#include "fifo.h"

void fifo_reset(fifo_t *fifo, uint8_t size)
{
	fifo->header.readPtr = 0;
	fifo->header.writePtr = 0;
	fifo->header.size = size;
}

uint8_t fifo_getLength(fifo_t *fifo)
{
	if (fifo->header.readPtr <= fifo->header.writePtr)
		return fifo->header.writePtr - fifo->header.readPtr;
	else
		return fifo->header.writePtr + fifo->header.size - fifo->header.readPtr;
}

uint8_t fifo_getContigLength(fifo_t *fifo)
{
	uint8_t len = fifo_getLength(fifo);
	if (len > fifo->header.size - fifo->header.readPtr) {
		len = fifo->header.size - fifo->header.readPtr;
	}
	return len;
}

uint8_t fifo_getFree(fifo_t *fifo)
{
	return fifo->header.size - fifo_getLength(fifo);
}

uint8_t *fifo_getContigData(fifo_t *fifo, uint8_t numBytes)
{
	uint8_t *retPtr = &fifo->buf[fifo->header.readPtr];
	fifo->header.readPtr += numBytes;
	if (fifo->header.readPtr >= fifo->header.size)
		fifo->header.readPtr -= fifo->header.size;
	return retPtr;
}

void fifo_putData(fifo_t *fifo, uint8_t *buf, uint8_t len)
{
	while (len--) {
		fifo->buf[fifo->header.writePtr++] = *buf++;
		if (fifo->header.writePtr == fifo->header.size)
			fifo->header.writePtr = 0;
	}
}
