#include <stdint.h>

typedef struct {
	uint8_t readPtr;
	uint8_t writePtr;
	uint8_t size;
} fifoHeader_t;


typedef struct {
	fifoHeader_t header;
	uint8_t buf[1];
} fifo_t;

typedef struct {
	fifoHeader_t header;
	uint8_t buf[128];
} serialFifo_t;

extern void fifo_reset(fifo_t *fifo, uint8_t size);
extern uint8_t fifo_getLength(fifo_t *fifo);
extern uint8_t fifo_getContigLength(fifo_t *fifo);
extern uint8_t fifo_getFree(fifo_t *fifo);
extern uint8_t *fifo_getContigData(fifo_t *fifo, uint8_t numBytes);
extern void fifo_putData(fifo_t *fifo, uint8_t *buf, uint8_t len);
