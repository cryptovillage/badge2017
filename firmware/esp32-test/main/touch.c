#include <stdio.h>
#include <dirent.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"

#include "board.h"

static const char* TAG = "TOUCH";
static bool s_pad_activated[TOUCH_PAD_MAX];
extern uint8_t tpTestStatus;

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
            }
        }
    }
}

/*
  Check if any of touch pads has been activated
  by reading a table updated by rtc_intr()
  If so, then print it out on a serial monitor.
  Clear related entry in the table afterwards
 */
static void tp_example_read_task(void *pvParameter)
{
    static int show_message;
    uint8_t newTpTestStatus;

    while (1) {
    	newTpTestStatus = 0;
        if (s_pad_activated[1])
        	newTpTestStatus |= 1;
    	if (s_pad_activated[7])
    		newTpTestStatus |= 2;

    	if (!(tpTestStatus & 0x80))
    		 tpTestStatus = newTpTestStatus;

        for (int i=0; i<TOUCH_PAD_MAX; i++) {
            if (s_pad_activated[i] == true) {
            	ESP_LOGI(TAG, "T%d activated!", i);
                // Wait a while for the pad being released
                vTaskDelay(150 / portTICK_PERIOD_MS);
                // Clear information on pad activation
                s_pad_activated[i] = false;
                // Reset the counter triggering a message
                // that application is running
                show_message = 1;
            }
        }

        // If no pad is touched, every couple of seconds, show a message
        // that application is running
        //if (show_message++ % 500 == 0) {
        //    ESP_LOGI(TAG, "%d Waiting for any pad being touched...", (int)time(NULL));
        //}
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void tp_init(void)
{
	uint16_t touch_value;

    touch_pad_init();

	ESP_ERROR_CHECK(touch_pad_read(BOARD_TOUCH_PAD_0, &touch_value));
	ESP_LOGI(TAG, "Touch %d = %d\n", BOARD_TOUCH_PAD_0, touch_value);
	ESP_ERROR_CHECK(touch_pad_config(BOARD_TOUCH_PAD_0, touch_value/2));

	ESP_ERROR_CHECK(touch_pad_read(BOARD_TOUCH_PAD_1, &touch_value));
	ESP_LOGI(TAG, "Touch %d = %d\n", BOARD_TOUCH_PAD_1, touch_value);
	ESP_ERROR_CHECK(touch_pad_config(BOARD_TOUCH_PAD_1, touch_value/2));

	touch_pad_isr_handler_register(tp_example_rtc_intr, NULL, 0, NULL);
	xTaskCreate(&tp_example_read_task, "touch_pad_read_task", 2048, NULL, 5, NULL);
}
