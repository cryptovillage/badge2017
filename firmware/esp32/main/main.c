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

#include "audio.h"
#include "events.h"
#include "ble.h"
#include "i2c.h"
#include "leds.h"
#include "display.h"
#include "vgm.h"
#include "touch.h"
#include "sdcard.h"
#include "misc.h"

#include "apps/sntp/sntp.h"

static const char* TAG = "CPV2017";

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

extern void python_start(void);

void app_main(void)
{
	ESP_LOGI(TAG, "Booting the ESP32 / CPV 2017 Badge...");

	ESP_LOGI(TAG, "Initializing display...");
    display_init();
    //display_show_logo();

    // Initialize everything else
    ESP_LOGI(TAG, "Initializing nvs...");
    nvs_flash_init();
    update_nvs_version_info();

    ESP_LOGI(TAG, "Initializing event queue...");
    eventq_init();

    ESP_LOGI(TAG, "Initializing I2C...");
	i2c_init();
	i2c_config_accel();

    ESP_LOGI(TAG, "Initializing TCP/IP...");
    tcpip_adapter_init();

    ESP_LOGI(TAG, "Initializing event loop...");
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    ESP_LOGI(TAG, "Initializing SD card...");
    sdcard_init();

    ESP_LOGI(TAG, "OS heap free is currently %d bytes", xPortGetFreeHeapSize());

    ESP_LOGI(TAG, "Initializing touch...");
    tp_init();

    ESP_LOGI(TAG, "Initializing BLE...");
    ble_init();

    ESP_LOGI(TAG, "OS heap free is currently %d bytes", xPortGetFreeHeapSize());

    ESP_LOGI(TAG, "Initializing WiFi...");
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    wifi_config_t sta_config = {
        .sta = {
            .ssid = CONFIG_WIFI_SSID,
            .password = CONFIG_WIFI_PASSWORD,
        	.bssid_set = false
        }
    };

    ESP_LOGI(TAG, "OS heap free is currently %d bytes", xPortGetFreeHeapSize());

    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_LOGI(TAG, "OS heap free is currently %d bytes", xPortGetFreeHeapSize());
    ESP_ERROR_CHECK( esp_wifi_set_auto_connect(true) );
    ESP_ERROR_CHECK( esp_wifi_connect() );

    ESP_LOGI(TAG, "OS heap free is currently %d bytes", xPortGetFreeHeapSize());

    ESP_LOGI(TAG, "Initializing SNTP...");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();

    ESP_LOGI(TAG, "OS heap free is currently %d bytes", xPortGetFreeHeapSize());
    audio_init();
    led_init_udp();

    ESP_LOGI(TAG, "Done initializing!");
    ESP_LOGI(TAG, "OS heap free is currently %d bytes", xPortGetFreeHeapSize());
    python_start();

    while (1) {
    	vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
