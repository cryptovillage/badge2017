/*
 * dac.h
 *
 *  Created on: Apr 10, 2017
 *      Author: supersat
 */

#ifndef MAIN_DAC_H_
#define MAIN_DAC_H_

#include <stdint.h>

extern void dac_init();
extern void dac_start();
extern void dac_stop();
extern void dac_sample_signed(int16_t left, int16_t right);
extern void dac_sample_unsigned(uint16_t left, uint16_t right);
extern void dac_samples(const char *buf, int len);

#endif /* MAIN_DAC_H_ */
