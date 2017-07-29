#include <string.h>

#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "driver/i2c.h"

#include "board.h"
#include "events.h"

static const char* TAG = "I2C";

#define ACCEL_REG_WHO_AM_I		0x0D
#define ACCEL_REG_XYZ_DATA_CFG	0x0E
#define ACCEL_REG_CTRL_REG1		0x2A
#define ACCEL_REG_CTRL_REG2		0x2B

#define ACCEL_RATE_100HZ		3

static const i2c_config_t i2c_conf_master = {
	.mode = I2C_MODE_MASTER,
	.scl_io_num = PIN_I2C_SCL,
	.sda_io_num = PIN_I2C_SDA,
	.sda_pullup_en = GPIO_PULLUP_DISABLE,
	.scl_pullup_en = GPIO_PULLUP_DISABLE,
	.master.clk_speed = I2C_CLK_SPEED
};

esp_err_t i2c_set_backlight_brightness(uint8_t brightness)
{
	esp_err_t ret;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, EFM8_I2C_ADDR, 1);
	i2c_master_write_byte(cmd, 0xDB, 1);
	i2c_master_write_byte(cmd, brightness, 1);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	if (ret)
		ESP_LOGW(TAG, "i2c_set_backlight_brightness result: %08x", ret);

	return ret;
}

esp_err_t i2c_poll_efm8()
{
	esp_err_t ret;
	uint8_t respBuf[5];
	badge_event_t e;

	// TODO(supersat): Use repeated starts
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, EFM8_I2C_ADDR, 1);
	i2c_master_write_byte(cmd, 'U', 1);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	if (ret) {
		ESP_LOGW(TAG, "i2c_poll_efm8 phase 1 result: %08x", ret);
		return ret;
	}

	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, EFM8_I2C_ADDR | 1, 1);
	i2c_master_read(cmd, respBuf, 4, 0);
	i2c_master_read_byte(cmd, &respBuf[4], 1);
	i2c_master_stop(cmd);

	ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	if (ret) {
		ESP_LOGW(TAG, "i2c_poll_efm8 phase 2 result: %08x", ret);
		return ret;
	}

	// Check to see if the EFM8 has had time to respond to our request
	// TODO(supersat): Figure out how to stall the I2C request until the EFM8 is ready?
	if (respBuf[0] != 'V') {
		return ESP_OK;
	}

	if (respBuf[1] & ENCODER_CW) {
		memset(&e, 0, sizeof(e));
		e.type = BADGE_EVENT_KNOB_CW;
		xQueueSendToBack(eventQueue, &e, portMAX_DELAY);
	}

	if (respBuf[1] & ENCODER_CCW) {
		memset(&e, 0, sizeof(e));
		e.type = BADGE_EVENT_KNOB_CCW;
		xQueueSendToBack(eventQueue, &e, portMAX_DELAY);
	}

	if (respBuf[1] & ENCODER_DOWN) {
		memset(&e, 0, sizeof(e));
		e.type = BADGE_EVENT_KNOB_DOWN;
		xQueueSendToBack(eventQueue, &e, portMAX_DELAY);
	}

	if (respBuf[1] & ENCODER_UP) {
		memset(&e, 0, sizeof(e));
		e.type = BADGE_EVENT_KNOB_UP;
		xQueueSendToBack(eventQueue, &e, portMAX_DELAY);
	}

	return ESP_OK;
}

esp_err_t i2c_config_max9723(uint8_t conf)
{
	esp_err_t ret;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, MAX9723_I2C_ADDR, 1);
	i2c_master_write_byte(cmd, conf, 1);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	if (ret) {
		ESP_LOGW(TAG, "i2c_config_max9723(%02x) result: %08x", conf, ret);
	}

	return ret;
}

uint8_t accel_read_reg(uint8_t reg, esp_err_t *i2c_err)
{
	uint8_t resp;
	esp_err_t ret;
	i2c_cmd_handle_t cmd;

	resp = 0xff;
	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, MMA8452Q_I2C_ADDR, 1);
	i2c_master_write_byte(cmd, reg, 1);
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, MMA8452Q_I2C_ADDR | 1, 1);
	i2c_master_read_byte(cmd, &resp, 1);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	if (ret) {
		ESP_LOGW(TAG, "accel_read_reg(%02x) returned %08x", reg, ret);
	}

	if (i2c_err) {
		*i2c_err = ret;
	}

	return resp;
}

esp_err_t accel_write_reg(uint8_t reg, uint8_t val)
{
	uint8_t ret;
	i2c_cmd_handle_t cmd;

	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, MMA8452Q_I2C_ADDR, 1);
	i2c_master_write_byte(cmd, reg, 1);
	i2c_master_write_byte(cmd, val, 1);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	if (ret) {
		ESP_LOGW(TAG, "accel_write_reg(%02x, %02x) returned %08x", reg, val, ret);
	}

	return ret;
}

esp_err_t accel_enter_standby()
{
	esp_err_t ret;
	uint8_t status;

	status = accel_read_reg(ACCEL_REG_CTRL_REG1, &ret);
	if (ret)
		return ret;

	return accel_write_reg(ACCEL_REG_CTRL_REG1, status & 0xfe);
}

esp_err_t accel_make_active()
{
	esp_err_t ret;
	uint8_t status;

	status = accel_read_reg(ACCEL_REG_CTRL_REG1, &ret);
	if (ret)
		return ret;

	return accel_write_reg(ACCEL_REG_CTRL_REG1, status | 1);
}

esp_err_t accel_set_rate(uint8_t rate)
{
	esp_err_t ret;
	uint8_t status;

	status = accel_read_reg(ACCEL_REG_CTRL_REG1, &ret);
	if (ret)
		return ret;

	status = (status & 0xc7) | ((rate & 7) << 3);

	return accel_write_reg(ACCEL_REG_CTRL_REG1, status);
}

esp_err_t i2c_config_accel()
{
	uint8_t resp;
	esp_err_t ret;

	// FIXME: Simple test to see if the accelerometer is there
	resp = accel_read_reg(ACCEL_REG_WHO_AM_I, &ret);
	ESP_LOGI(TAG, "Accelerometer WHO_AMI_I returned %02x", resp);
	if (resp != 0x2A) {
		return 0xFFFFFF00 | resp;
	}

	/*
	// Reset accelerometer
	ret = accel_write_reg(ACCEL_REG_CTRL_REG2, (1 << 6));
	if (ret)
		return ret;

	resp = accel_read_reg(ACCEL_REG_WHO_AM_I, &ret);
	ESP_LOGI(TAG, "Accelerometer WHO_AMI_I returned %02x", resp);
	if (resp != 0x2A) {
		return 0xFFFFFF00 | resp;
	}
	*/

	/*
	ret = accel_enter_standby();
	if (ret)
		return ret;

	// Set 4g max
	ret = accel_write_reg(ACCEL_REG_XYZ_DATA_CFG, 1);
	if (ret)
		return ret;

	// Set 100Hz mode
	ret = accel_set_rate(ACCEL_RATE_100HZ);
	if (ret)
		return ret;
	*/

	// Set CTRL_REG2 to do a self-test?
	// Set CTRL_REG1 to set data rate, maybe make low noise, make active

	return 0;
}

esp_err_t i2c_send_led_cmd(uint8_t *ledBuf)
{
	esp_err_t ret;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, EFM8_I2C_ADDR, 1);
	i2c_master_write_byte(cmd, 0xBB, 1);
	i2c_master_write(cmd, ledBuf, 32, 1);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	if (ret)
		ESP_LOGW(TAG, "i2c_send_led_cmd result: %08x", ret);

	return ret;
}

esp_err_t i2c_atecc_resync()
{
	esp_err_t ret;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, 0xFF, 0);
	i2c_master_start(cmd);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	if (ret)
		ESP_LOGW(TAG, "i2c_atecc_resync result: %08x", ret);

	return ret;
}

esp_err_t i2c_atecc_wake()
{
	esp_err_t ret;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, 0x01, 0);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	vTaskDelay(10 / portTICK_PERIOD_MS);

	if (ret)
		ESP_LOGW(TAG, "i2c_atecc_wake result: %08x", ret);

	return ret;
}

esp_err_t i2c_atecc_write_bytes(uint8_t *buf, uint8_t len)
{
	esp_err_t ret;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, ATECC508A_I2C_ADDR, 1);
	i2c_master_write(cmd, buf, len, 1);
	i2c_master_stop(cmd);

	ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	if (ret)
		ESP_LOGW(TAG, "i2c_atecc_write_bytes returned: %08x", ret);

	return ret;
}

static uint16_t feed_crc(uint16_t crc, uint8_t b)
{
	crc ^= b;
	crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
	crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
	crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
	crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
	crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
	crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
	crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
	return crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
}

uint16_t reverse_bits(uint16_t crc)
{
	// efficient bit reversal for 16 bit int
	crc = (((crc & 0xaaaa) >> 1) | ((crc & 0x5555) << 1));
	crc = (((crc & 0xcccc) >> 2) | ((crc & 0x3333) << 2));
	crc = (((crc & 0xf0f0) >> 4) | ((crc & 0x0f0f) << 4));
	return (((crc & 0xff00) >> 8) | ((crc & 0x00ff) << 8));
}

uint16_t i2c_atecc_compute_crc(uint8_t cmd, uint8_t p1, uint16_t p2, uint8_t *buf, uint8_t len)
{
	uint16_t crc = 0;
	crc = feed_crc(crc, 7 + len);
	crc = feed_crc(crc, cmd);
	crc = feed_crc(crc, p1);
	crc = feed_crc(crc, p2 & 0xff);
	crc = feed_crc(crc, p2 >> 8);
	while (len--) {
		crc = feed_crc(crc, *buf++);
	}

	return reverse_bits(crc);
}

esp_err_t i2c_atecc_cmd(uint8_t c, uint8_t p1, uint16_t p2, uint8_t *buf, uint8_t len)
{
	uint16_t crc;
	esp_err_t ret;

	crc = i2c_atecc_compute_crc(c, p1, p2, buf, len);
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, ATECC508A_I2C_ADDR, 1);
	i2c_master_write_byte(cmd, 3, 1);
	i2c_master_write_byte(cmd, 7 + len, 1);
	i2c_master_write_byte(cmd, c, 1);
	i2c_master_write_byte(cmd, p1, 1);
	i2c_master_write_byte(cmd, p2 & 0xff, 1);
	i2c_master_write_byte(cmd, p2 >> 8, 1);
	i2c_master_write(cmd, buf, len, 1);
	i2c_master_write_byte(cmd, crc & 0xff, 1);
	i2c_master_write_byte(cmd, crc >> 8, 1);
	i2c_master_stop(cmd);

	ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	if (ret)
		ESP_LOGW(TAG, "i2c_atecc_cmd returned: %08x", ret);

	return ret;

}

esp_err_t i2c_atecc_check_presence()
{
	esp_err_t ret;
	i2c_cmd_handle_t cmd;
	uint8_t cmdBuf[4];
	uint8_t respBuf[4];

	//i2c_atecc_resync();
	i2c_atecc_wake();
	cmdBuf[0] = 0;
	i2c_atecc_write_bytes(cmdBuf, 1);

	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, ATECC508A_I2C_ADDR | 1, 1);
	i2c_master_read(cmd, respBuf, 3, 0);
	i2c_master_read_byte(cmd, &respBuf[3], 1);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	ESP_LOGI(TAG, "i2c_atecc_check_presence() got %02x %02x %02x %02x (%08x)",
			respBuf[0], respBuf[1], respBuf[2], respBuf[3], ret);

	if (ret)
		return ret;

	if (respBuf[1] != 0x11)
		return respBuf[1];
	else
		return 0;
}

static void i2c_poll_task(void *pvParameter)
{
	for (;;) {
		i2c_poll_efm8();
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

void i2c_init()
{
	i2c_param_config(I2C_NUM_0, &i2c_conf_master);
	i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
	xTaskCreate(&i2c_poll_task, "i2c_poll_task", 2048, NULL, 5, NULL);
}
