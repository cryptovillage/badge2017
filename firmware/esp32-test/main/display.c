#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "u8g2.h"
#include "board.h"

#define LCD_DMA_CHAN 1

const char* tag = "DISPLAY";

//unsigned char lcdBuf[1024];

spi_device_handle_t lcd_spi;
u8g2_t u8g2;

spi_bus_config_t lcd_SPI_master_cfg = {
	.miso_io_num = -1,
	.mosi_io_num = PIN_LCD_MOSI,
	.sclk_io_num = PIN_LCD_SCK,
	.quadwp_io_num = -1,
	.quadhd_io_num = -1
};

void lcd_spi_pre_transfer_callback(spi_transaction_t *t) {
    gpio_set_level(PIN_LCD_DC, (int)t->user);
}

spi_device_interface_config_t lcd_SPI_dev_cfg = {
	.clock_speed_hz = 1000000,					// Clock out at 10 MHz
	.mode = 0,									// SPI mode 0
	.spics_io_num = PIN_LCD_CS,					// CS pin
	.queue_size = 7,							// We want to be able to queue 7 transactions at a time
	.pre_cb = lcd_spi_pre_transfer_callback,	// Specify pre-transfer callback to handle D/C line
};

void lcd_cmd_array(const uint8_t *cmds, int len) {
	spi_transaction_t t;
	memset(&t, 0, sizeof(t));
	t.length = len * 8;
	t.tx_buffer = cmds;
	t.user = (void *)0;
	ESP_ERROR_CHECK(spi_device_transmit(lcd_spi, &t));
}

void lcd_cmd(const uint8_t cmd) {
/*	spi_transaction_t t;
	memset(&t, 0, sizeof(t));
	t.length = 8;
	t.tx_buffer = &cmd;
	t.user = (void *)0;
	ESP_ERROR_CHECK(spi_device_transmit(lcd_spi, &t));
*/
	lcd_cmd_array(&cmd, 1);
}

void lcd_data(const uint8_t *data, int len) {
	spi_transaction_t t;
	memset(&t, 0, sizeof(t));
	t.length = len * 8;
	t.tx_buffer = data;
	t.user = (void *)1;
	ESP_ERROR_CHECK(spi_device_transmit(lcd_spi, &t));
}

uint8_t u8x8_esp_msg_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
	static uint8_t dc = 0;
	spi_transaction_t t;

	switch (msg) {
	case U8X8_MSG_BYTE_INIT:
		// TODO(supersat): Move SPI / GPIO initialization here
		return 1;
	case U8X8_MSG_BYTE_SET_DC:
		dc = arg_int;
		return 1;
	case U8X8_MSG_BYTE_START_TRANSFER:
	case U8X8_MSG_BYTE_END_TRANSFER:
		return 1;
	case U8X8_MSG_BYTE_SEND:
		memset(&t, 0, sizeof(t));
		t.length = arg_int * 8;
		t.tx_buffer = arg_ptr;
		t.user = (void *)dc;
		ESP_ERROR_CHECK(spi_device_transmit(lcd_spi, &t));
		return 1;
	default:
		ESP_LOGD(tag, "UNKNOWN U8X8 MSG BYTE CALLBACK %08x", msg);
	}

	return 0;
}

uint8_t u8x8_esp_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	switch (msg) {
	case U8X8_MSG_GPIO_AND_DELAY_INIT:
		return 1;
	case U8X8_MSG_GPIO_RESET:
		gpio_set_level(PIN_LCD_RESET, arg_int);
		return 1;
	case U8X8_MSG_DELAY_MILLI:
		vTaskDelay(arg_int / portTICK_PERIOD_MS);
		return 1;
	default:
		ESP_LOGD(tag, "UNKNOWN U8X8 GPIO AND DELAY MSG %08x", msg);
	}

	return 0;
}

void display_test() {
	u8g2_InitDisplay(&u8g2);
	u8g2_ClearDisplay(&u8g2);
	u8g2_SetPowerSave(&u8g2, 0);
	u8g2_ClearBuffer(&u8g2);
	u8g2_SetFont(&u8g2, u8g2_font_profont12_mr);
	u8g2_DrawStr(&u8g2, 36, 36, "TEST MODE");
	u8g2_SendBuffer(&u8g2);
}

void display_on() {
	lcd_cmd(0xA4);
	lcd_cmd(0xAF);
}

void display_off() {
	lcd_cmd(0xAE); // display off
	lcd_cmd(0xA5); // display all points
}

// Called whenever we need to re-enable the LCD
void display_enable() {
	gpio_set_level(PIN_LCD_RESET, 1);
	vTaskDelay(1 / portTICK_PERIOD_MS); // sleep for 1 ms
	gpio_set_level(PIN_LCD_RESET, 0);
	vTaskDelay(1 / portTICK_PERIOD_MS); // sleep for 1 ms
	gpio_set_level(PIN_LCD_RESET, 1);
	vTaskDelay(1 / portTICK_PERIOD_MS); // sleep for 1 ms
	lcd_cmd(0xa0); // Clear ADC (normal SEG order)
	lcd_cmd(0xc8); // Set COM order
	lcd_cmd(0xa2); // Clear bias
	lcd_cmd(0x2f); // Use internal voltage booster, regulator, and follower
	lcd_cmd(0x25); // Regulator resistor selector
	lcd_cmd(0xa7);
}

void display_disable() {
	display_off();
	gpio_set_level(PIN_LCD_RESET, 0);
}

// Called ONCE per boot
void display_init() {
	gpio_set_direction(PIN_LCD_RESET, GPIO_MODE_OUTPUT);
	gpio_set_direction(PIN_LCD_DC, GPIO_MODE_OUTPUT);
	ESP_ERROR_CHECK(spi_bus_initialize(VSPI_HOST, &lcd_SPI_master_cfg, LCD_DMA_CHAN));
	ESP_ERROR_CHECK(spi_bus_add_device(VSPI_HOST, &lcd_SPI_dev_cfg, &lcd_spi));
	u8g2_Setup_st7565_erc12864_f(&u8g2, U8G2_R0, u8x8_esp_msg_cb, u8x8_esp_gpio_and_delay);
}
