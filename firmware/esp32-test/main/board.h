/*
 * board.h
 *
 *  Created on: Apr 4, 2017
 *      Author: supersat
 */

#ifndef MAIN_BOARD_H_
#define MAIN_BOARD_H_

#include "sdkconfig.h"

#define PIN_LCD_SCK			18
#define PIN_LCD_MOSI		23
#define PIN_LCD_CS			5
#define PIN_LCD_RESET		17
#define PIN_LCD_DC			16
#define PIN_I2C_SCL			22
#define PIN_I2C_SDA			21

#define I2C_CLK_SPEED		100000	// Change to 400000 once I figure out how to deal with the ATECC508A
#define MMA8452Q_I2C_ADDR	0x38
#define MAX9723_I2C_ADDR	0x9A
#define ATECC508A_I2C_ADDR	0xC0
#define EFM8_I2C_ADDR		0xC8
#define ESP32_I2C_ADDR		0xCC

#ifdef CONFIG_BOARD_VERSION_V1
// TODO(supersat): Double-check these
#define BOARD_TOUCH_PAD_0	0		// TOUCH_PAD_NUM0
#define BOARD_TOUCH_PAD_1	2		// TOUCH_PAD_NUM2
#endif

#ifdef CONFIG_BOARD_VERSION_V2
#define BOARD_TOUCH_PAD_0	1		// TOUCH_PAD_NUM1
#define BOARD_TOUCH_PAD_1	7		// TOUCH_PAD_NUM7
#endif

#ifdef CONFIG_BOARD_VERSION_V3
#define BOARD_TOUCH_PAD_0	1		// TOUCH_PAD_NUM1
#define BOARD_TOUCH_PAD_1	7		// TOUCH_PAD_NUM7
#endif

#endif /* MAIN_BOARD_H_ */
