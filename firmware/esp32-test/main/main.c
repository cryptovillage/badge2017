#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "driver/touch_pad.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_io_reg.h"
#include "soc/sens_reg.h"

#include "board.h"
#include "sdkconfig.h"

#include "i2c.h"
#include "display.h"
#include "vgm.h"
#include "touch.h"
#include "sdcard.h"

static const char* TAG = "TEST";

#define TEST_PENDING 0xff
#define TEST_PASS	 0
#define TEST_FAIL	 1

uint8_t ledTestStatus = TEST_PENDING;
uint8_t max9723TestStatus = TEST_PENDING;
uint8_t mma8452QTestStatus = TEST_PENDING;
uint8_t atecc508ATestStatus = TEST_PENDING;
uint8_t tpTestStatus = 0;
uint8_t tpTestState = 0;

esp_err_t event_handler(void *ctx, system_event_t *event)
{
	return ESP_OK;
}

typedef struct {
	uint8_t ones:3;
	uint8_t brightness:5;
	uint8_t blue;
	uint8_t green;
	uint8_t red;
} LEDFrame;

uint8_t ledBuf[32];
LEDFrame *ledFrames = (LEDFrame *)ledBuf;

void led_test()
{
	uint8_t i;

	for (i = 0; i < 8; i++) {
		ledFrames[i].ones = 7;
		ledFrames[i].brightness = 16;
		ledFrames[i].blue = 0;
		ledFrames[i].green = 0xff;
		ledFrames[i].red = 0xff;
	}

	i2c_send_led_cmd(ledBuf);
	vTaskDelay(500 / portTICK_PERIOD_MS);

	for (i = 0; i < 8; i++) {
		ledFrames[i].ones = 7;
		ledFrames[i].brightness = 16;
		ledFrames[i].blue = 0xff;
		ledFrames[i].green = 0;
		ledFrames[i].red = 0;
	}

	i2c_send_led_cmd(ledBuf);
	vTaskDelay(500 / portTICK_PERIOD_MS);

	for (i = 0; i < 8; i++) {
		ledFrames[i].ones = 7;
		ledFrames[i].brightness = 16;
		ledFrames[i].blue = 0;
		ledFrames[i].green = 0xff;
		ledFrames[i].red = 0;
	}
}

void led_update_test_status()
{
	if (max9723TestStatus == TEST_PENDING) {
		ledFrames[0].red = 0;
		ledFrames[0].green = 0;
		ledFrames[0].blue = 0xff;
	} else if (max9723TestStatus == TEST_PASS) {
		ledFrames[0].red = 0;
		ledFrames[0].green = 0xff;
		ledFrames[0].blue = 0;
	} else {
		ledFrames[0].red = 0xff;
		ledFrames[0].green = 0;
		ledFrames[0].blue = 0;
	}

	if (mma8452QTestStatus == TEST_PENDING) {
		ledFrames[1].red = 0;
		ledFrames[1].green = 0;
		ledFrames[1].blue = 0xff;
	} else if (mma8452QTestStatus == TEST_PASS) {
		ledFrames[1].red = 0;
		ledFrames[1].green = 0xff;
		ledFrames[1].blue = 0;
	} else {
		ledFrames[1].red = 0xff;
		ledFrames[1].green = 0;
		ledFrames[1].blue = 0;
	}

	if (atecc508ATestStatus == TEST_PENDING) {
		ledFrames[2].red = 0;
		ledFrames[2].green = 0;
		ledFrames[2].blue = 0xff;
	} else if (atecc508ATestStatus == TEST_PASS) {
		ledFrames[2].red = 0;
		ledFrames[2].green = 0xff;
		ledFrames[2].blue = 0;
	} else {
		ledFrames[2].red = 0xff;
		ledFrames[2].green = 0;
		ledFrames[2].blue = 0;
	}

	if (tpTestState == 8) {
		ledFrames[3].red = 0x00;
		ledFrames[3].green = 0xff;
		ledFrames[3].blue = 0;

	} else {
		ledFrames[3].red = (tpTestStatus & 1) ? 0xff : 0x00;
		ledFrames[3].green = (tpTestStatus & 2) ? 0xff : 0x00;
		ledFrames[3].blue = 0xff;
	}

	if (i2c_send_led_cmd(ledBuf)) {
		ESP_LOGW(TAG, "Unable to update LEDs!");
	}
}

static void audio_test(void *pvParameter)
{
	playVGM();
}

void accel_test()
{
	esp_err_t ret;
	ret = i2c_config_accel();
	if (!ret) {
		mma8452QTestStatus = TEST_PASS;
	} else {
		mma8452QTestStatus = TEST_FAIL;
	}
}

void app_main(void)
{
	ESP_LOGI(TAG, "Booting into TEST MODE...");

	ESP_LOGI(TAG, "Initializing display...");
	// Put something on the screen while booting
	display_init();
	display_test();

	ESP_LOGI(TAG, "Initializing I2C...");
	i2c_init();
	i2c_config_max9723(0x10);

	// Initialize everything else
	ESP_LOGI(TAG, "Initializing nvs...");
	nvs_flash_init();
	ESP_LOGI(TAG, "Initializing TCP/IP...");
	tcpip_adapter_init();
	ESP_LOGI(TAG, "Initializing event loop...");
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

	//ESP_LOGI(TAG, "Initializing touch...");
	tp_init();

	led_test();

	if (i2c_config_max9723(0x98)) {
		max9723TestStatus = TEST_FAIL;
	} else {
		max9723TestStatus = TEST_PASS;
	}

	accel_test();

	if (i2c_atecc_check_presence()) {
		atecc508ATestStatus = TEST_FAIL;
	} else {
		atecc508ATestStatus = TEST_PASS;
	}

	xTaskCreatePinnedToCore(audio_test, "audio_test", 4096, NULL, 5, NULL, 1);

	while (1) {
		switch (tpTestState) {
		case 0:
			if (tpTestStatus == 1) {
				tpTestState = 1;
			}
			break;
		case 1:
			if (tpTestStatus == 0) {
				tpTestState = 2;
			} else if (tpTestStatus != 1) {
				tpTestState = 0;
			}
			break;
		case 2:
			if (tpTestStatus == 2) {
				tpTestState = 3;
			} else if (tpTestStatus != 0) {
				tpTestState = 0;
			}
			break;
		case 3:
			if (tpTestStatus == 0) {
				tpTestState = 4;
			} else if (tpTestStatus != 2) {
				tpTestState = 0;
			}
			break;
		case 4:
			if (tpTestStatus == 2) {
				tpTestState = 5;
			} else if (tpTestStatus != 0) {
				tpTestState = 0;
			}
			break;
		case 5:
			if (tpTestStatus == 0) {
				tpTestState = 6;
			} else if (tpTestStatus != 2) {
				tpTestState = 0;
			}
			break;
		case 6:
			if (tpTestStatus == 1) {
				tpTestState = 7;
			} else if (tpTestStatus != 0) {
				tpTestState = 0;
			}
			break;
		case 7:
			if (tpTestStatus == 0) {
				tpTestState = 8;
			} else if (tpTestStatus != 1) {
				tpTestState = 0;
			}
			break;
		case 8:
			tpTestState = 9;
			break;
		}

		vTaskDelay(50 / portTICK_PERIOD_MS);
		if (tpTestState != 9) {
			led_update_test_status();
		}
	}
}
