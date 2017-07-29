#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"

#include "board.h"
#include "events.h"

static const char* TAG = "TOUCH";
static bool s_pad_activated[TOUCH_PAD_MAX];
static uint32_t pad_activation_time[TOUCH_PAD_MAX];

/*
  Handle an interrupt triggered when a pad is touched.
  Recognize what pad has been touched and save it in a table.
 */
static void tp_example_rtc_intr(void * arg)
{
    uint32_t pad_intr = READ_PERI_REG(SENS_SAR_TOUCH_CTRL2_REG) & 0x3ff;
    uint32_t rtc_intr = READ_PERI_REG(RTC_CNTL_INT_ST_REG);
    //clear interrupt
    WRITE_PERI_REG(RTC_CNTL_INT_CLR_REG, rtc_intr);
    SET_PERI_REG_MASK(SENS_SAR_TOUCH_CTRL2_REG, SENS_TOUCH_MEAS_EN_CLR);

    if (rtc_intr & RTC_CNTL_TOUCH_INT_ST) {
        for (int i = 0; i < TOUCH_PAD_MAX; i++) {
            if ((pad_intr >> i) & 0x01) {
                s_pad_activated[i] = true;
                pad_activation_time[i] = esp_log_timestamp();
            }
        }
    }
}

static void tp_example_read_task(void *pvParameter)
{
	bool last_pads_activated[TOUCH_PAD_MAX];
	badge_event_t evt;

	memset(&last_pads_activated, 0, sizeof(last_pads_activated));
	memset(&evt, 0, sizeof(evt));

    while (1) {
        if (s_pad_activated[BOARD_TOUCH_PAD_0] && !last_pads_activated[BOARD_TOUCH_PAD_0]) {
        	last_pads_activated[BOARD_TOUCH_PAD_0] = true;
        	evt.type = BADGE_EVENT_LEFT_TOUCH_PRESSED;
        	ESP_LOGD(TAG, "BADGE_EVENT_LEFT_TOUCH_PRESSED");
        	xQueueSendToBack(eventQueue, &evt, 100 / portTICK_PERIOD_MS);
        } else if (last_pads_activated[BOARD_TOUCH_PAD_0] && (pad_activation_time[BOARD_TOUCH_PAD_0] + 200) < esp_log_timestamp()) {
        	s_pad_activated[BOARD_TOUCH_PAD_0] = false;
        	last_pads_activated[BOARD_TOUCH_PAD_0] = false;
        	evt.type = BADGE_EVENT_LEFT_TOUCH_RELEASED;
        	ESP_LOGD(TAG, "BADGE_EVENT_LEFT_TOUCH_RELEASED");
           	xQueueSendToBack(eventQueue, &evt, 100 / portTICK_PERIOD_MS);
        }

        if (s_pad_activated[BOARD_TOUCH_PAD_1] && !last_pads_activated[BOARD_TOUCH_PAD_1]) {
        	last_pads_activated[BOARD_TOUCH_PAD_1] = true;
        	evt.type = BADGE_EVENT_RIGHT_TOUCH_PRESSED;
        	ESP_LOGD(TAG, "BADGE_EVENT_RIGHT_TOUCH_PRESSED");
            xQueueSendToBack(eventQueue, &evt, 100 / portTICK_PERIOD_MS);
        } else if (last_pads_activated[BOARD_TOUCH_PAD_1] && (pad_activation_time[BOARD_TOUCH_PAD_1] + 200) < esp_log_timestamp()) {
        	s_pad_activated[BOARD_TOUCH_PAD_1] = false;
        	last_pads_activated[BOARD_TOUCH_PAD_1] = false;
        	evt.type = BADGE_EVENT_RIGHT_TOUCH_RELEASED;
        	ESP_LOGD(TAG, "BADGE_EVENT_RIGHT_TOUCH_RELEASED");
        	xQueueSendToBack(eventQueue, &evt, 100 / portTICK_PERIOD_MS);
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void tp_init(void)
{
	uint16_t touch_value;

    touch_pad_init();

	ESP_ERROR_CHECK(touch_pad_read(BOARD_TOUCH_PAD_0, &touch_value));
	ESP_LOGD(TAG, "Touch %d = %d\n", BOARD_TOUCH_PAD_0, touch_value);
	ESP_ERROR_CHECK(touch_pad_config(BOARD_TOUCH_PAD_0, touch_value/2));

	ESP_ERROR_CHECK(touch_pad_read(BOARD_TOUCH_PAD_1, &touch_value));
	ESP_LOGD(TAG, "Touch %d = %d\n", BOARD_TOUCH_PAD_1, touch_value);
	ESP_ERROR_CHECK(touch_pad_config(BOARD_TOUCH_PAD_1, touch_value/2));

	touch_pad_isr_handler_register(tp_example_rtc_intr, NULL, 0, NULL);
	xTaskCreate(&tp_example_read_task, "touch_pad_read_task", 2048, NULL, 5, NULL);
}
