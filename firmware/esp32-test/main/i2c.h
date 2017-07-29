/*
 * i2c.h
 *
 *  Created on: Apr 11, 2017
 *      Author: supersat
 */

#ifndef MAIN_I2C_H_
#define MAIN_I2C_H_

extern void i2c_init();
extern esp_err_t i2c_config_max9723(uint8_t conf);
extern uint8_t accel_read_reg(uint8_t reg, esp_err_t *i2c_err);
extern esp_err_t accel_write_reg(uint8_t reg, uint8_t val);
extern esp_err_t accel_enter_standby();
extern esp_err_t accel_make_active();
extern esp_err_t accel_set_rate(uint8_t rate);
extern esp_err_t i2c_config_accel();
extern esp_err_t i2c_send_led_cmd(uint8_t *ledBuf);
extern esp_err_t i2c_atecc_check_presence();

#endif /* MAIN_I2C_H_ */
