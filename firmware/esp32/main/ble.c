#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"

#include "events.h"

static const char *TAG = "BLE";

// TODO(supersat): Are these parameters (especially adv_type and own_addr_type) right?
static esp_ble_adv_params_t ble_adv_params = {
    .adv_int_min        = 0x100, // 160 ms
    .adv_int_max        = 0x500, // 800 ms
    .adv_type           = ADV_TYPE_NONCONN_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    //.peer_addr            =
    //.peer_addr_type       =
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static esp_ble_scan_params_t ble_scan_params = {
	.scan_type = BLE_SCAN_TYPE_PASSIVE,
	.own_addr_type = BLE_ADDR_TYPE_RANDOM,
	.scan_filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
	.scan_interval = 800, // 500 ms
	.scan_window = 160, // 100 ms
};

const char *ble_gap_event_names[] = {
	"ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT",                  /*!< When advertising data set complete, the event comes */
	"ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT",             /*!< When scan response data set complete, the event comes */
	"ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT",                /*!< When scan parameters set complete, the event comes */
	"ESP_GAP_BLE_SCAN_RESULT_EVT",                            /*!< When one scan result ready, the event comes each time */
	"ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT",              /*!< When raw advertising data set complete, the event comes */
	"ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT",         /*!< When raw advertising data set complete, the event comes */
	"ESP_GAP_BLE_ADV_START_COMPLETE_EVT",                     /*!< When start advertising complete, the event comes */
	"ESP_GAP_BLE_SCAN_START_COMPLETE_EVT",                    /*!< When start scan complete, the event comes */
	"ESP_GAP_BLE_AUTH_CMPL_EVT",                              /* Authentication complete indication. */
	"ESP_GAP_BLE_KEY_EVT",                                    /* BLE  key event for peer device keys */
	"ESP_GAP_BLE_SEC_REQ_EVT",                                /* BLE  security request */
	"ESP_GAP_BLE_PASSKEY_NOTIF_EVT",                          /* passkey notification event */
	"ESP_GAP_BLE_PASSKEY_REQ_EVT",                            /* passkey request event */
	"ESP_GAP_BLE_OOB_REQ_EVT",                                /* OOB request event */
	"ESP_GAP_BLE_LOCAL_IR_EVT",                               /* BLE local IR event */
	"ESP_GAP_BLE_LOCAL_ER_EVT",                               /* BLE local ER event */
	"ESP_GAP_BLE_NC_REQ_EVT",                                 /* Numeric Comparison request event */
	"ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT",                      /*!< When stop adv complete, the event comes */
	"ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT",                     /*!< When stop scan complete, the event comes */
	"ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT",                   /*!< When set the static rand address complete, the event comes */
	"ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT",                     /*!< When update connection parameters complete, the event comes */
	"ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT",                /*!< When set pkt lenght complete, the event comes */
};

// Appearance: 0x19DC, Flags: 0x06, Mfg: 0x0C97
static uint8_t ble_adv_std_data_header[] = { 0x03, 0x19, 0xDC, 0x19, 0x02, 0x01, 0x06, 0x0B, 0xFF, 0x97, 0x0C };
static uint8_t ble_adv_std_data[31];
static uint8_t ble_adv_custom_data[31];
static uint8_t ble_adv_custom_data_len;
static bool ble_adv_std_data_set = false;
static bool ble_adv_custom_data_set = false;
static bool ble_adv_std_data_enabled = false;
static bool ble_adv_custom_data_enabled = false;

typedef struct __attribute__((packed)) {
	uint8_t header[9];
	uint16_t company;
	uint8_t badgeDefined[8];
	uint8_t nameLength;
	uint8_t nameType;
	uint8_t name[10];
} badgelife_gap_pkt_t;

badgelife_gap_pkt_t *ble_adv_std_pkt = (badgelife_gap_pkt_t *)ble_adv_std_data;

void ble_gap_set_adv_std_data_enabled(bool enabled)
{
	ble_adv_std_data_enabled = enabled;
}

void ble_gap_set_adv_custom_data_enabled(bool enabled)
{
	ble_adv_std_data_enabled = enabled;
}

void ble_gap_set_adv_std_data(const char *name, size_t nameLen, const uint8_t *badgeDefinedData)
{
	// Set ble_adv_std_data_set to false to ensure ble_task sees consistent data
	ble_adv_std_data_set = false;

	// Set header and company
	memcpy(ble_adv_std_data, ble_adv_std_data_header, sizeof(ble_adv_std_data_header));
	memcpy(ble_adv_std_pkt->badgeDefined, badgeDefinedData, 8);
	if (nameLen <= 10) {
		ble_adv_std_pkt->nameLength = nameLen + 1;
		ble_adv_std_pkt->nameType = 0x09; // Full name
		memcpy(&ble_adv_std_pkt->name, name, nameLen);
	} else {
		ble_adv_std_pkt->nameLength = 11;
		ble_adv_std_pkt->nameType = 0x08; // Abbreviated name
		memcpy(&ble_adv_std_pkt->name, name, 10);
	}

	ble_adv_std_data_set = true;
}

void ble_gap_set_adv_custom_data(const uint8_t *data, size_t len)
{
	ble_adv_custom_data_set = false;

	if (len > 31) {
		len = 31;
	}

	memcpy(ble_adv_custom_data, data, len);
	ble_adv_custom_data_len = len;
	ble_adv_custom_data_set = true;
}

static void ble_gap_handle_scan_result(esp_ble_gap_cb_param_t *param)
{
	badge_event_t event;
	ESP_LOGD(TAG, "Found BLE device %02x:%02x:%02x:%02x:%02x:%02x rssi=%d",
				    param->scan_rst.bda[0], param->scan_rst.bda[1],
					param->scan_rst.bda[2], param->scan_rst.bda[3],
					param->scan_rst.bda[4], param->scan_rst.bda[5],
					param->scan_rst.rssi);
	memset(&event, 0, sizeof(event));
	event.type = BADGE_EVENT_BLE_SCAN_RESULT;
	memcpy(&event.data, param->scan_rst.ble_adv, 31);
	xQueueSendToBack(eventQueue, &event, 50 / portTICK_PERIOD_MS);
}

static void ble_gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
	ESP_LOGD(TAG, "received event %s", ble_gap_event_names[event]);

	switch (event) {
	/*
	case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT:
		ESP_LOGD(TAG, "Calling esp_ble_gap_start_scanning");
		if (esp_ble_gap_start_scanning(1000) != ESP_OK) {
			ESP_LOGW(TAG, "esp_ble_gap_start_scanning");
		}
		break;
	*/
	case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
		ESP_LOGD(TAG, "Calling esp_ble_gap_start_advertising");
	    if (esp_ble_gap_start_advertising(&ble_adv_params) != ESP_OK) {
	    	ESP_LOGW(TAG, "esp_ble_gap_start_advertising");
	    }
	    break;
	case ESP_GAP_BLE_SCAN_RESULT_EVT:
		ble_gap_handle_scan_result(param);
		break;
	default:
		break;
	}
}

static void ble_task(void *pvParameter) {
	esp_err_t ret;

	for (;;) {
		if (ble_adv_std_data_set && ble_adv_std_data_enabled) {
			ret = esp_ble_gap_config_adv_data_raw(ble_adv_std_data, 20 + ble_adv_std_pkt->nameLength);
			if (ret != ESP_OK) {
				ESP_LOGW(TAG, "esp_ble_gap_config_adv_data_raw failed");
			}
			vTaskDelay((1000 + (esp_random() & 0x3FFF)) / portTICK_PERIOD_MS);
			esp_ble_gap_stop_advertising();
			vTaskDelay(100 / portTICK_PERIOD_MS);
		}

		if (ble_adv_custom_data_set && ble_adv_custom_data_enabled) {
			ret = esp_ble_gap_config_adv_data_raw(ble_adv_custom_data, ble_adv_custom_data_len);
			if (ret != ESP_OK) {
				ESP_LOGW(TAG, "esp_ble_gap_config_adv_data_raw failed");
			}
			vTaskDelay((1000 + (esp_random() & 0x3FFF)) / portTICK_PERIOD_MS);
			esp_ble_gap_stop_advertising();
			vTaskDelay(100 / portTICK_PERIOD_MS);
		}

		if (esp_ble_gap_start_scanning(1000) != ESP_OK) {
			ESP_LOGW(TAG, "esp_ble_gap_start_scanning");
		}
		vTaskDelay((1000 + (esp_random() & 0x3FFF)) / portTICK_PERIOD_MS);
		esp_ble_gap_stop_scanning();
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

void ble_init()
{
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

    if (esp_bt_controller_init(&bt_cfg) != ESP_OK) {
        ESP_LOGW(TAG, "Bluetooth controller initialize failed");
        return;
    }

    if (esp_bt_controller_enable(ESP_BT_MODE_BTDM) != ESP_OK) {
        ESP_LOGW(TAG, "Bluetooth controller enable failed");
        return;
    }

    if (esp_bluedroid_init() != ESP_OK) {
    	ESP_LOGW(TAG, "bluedroid stack initialize failed");
    	return;
    }

    if (esp_bluedroid_enable() != ESP_OK) {
    	ESP_LOGW(TAG, "bluedroid stack enable failed");
    	return;
    }

    if (esp_ble_gap_register_callback(ble_gap_event_handler) != ESP_OK) {
    	ESP_LOGW(TAG, "esp_ble_gap_register_callback failed");
    }

    // TODO(supersat): Not sure if this is needed or not
    if (esp_ble_gap_config_local_privacy(1) != ESP_OK) {
    	ESP_LOGW(TAG, "esp_ble_gap_config_local_privacy failed");
    }

    if (esp_ble_gap_set_scan_params(&ble_scan_params) != ESP_OK) {
    	ESP_LOGW(TAG, "esp_ble_gap_set_scan_params failed");
    }

    // Create a task to cycle between scanning and advertising
    xTaskCreate(&ble_task, "ble_task", 2048, NULL, 5, NULL);
}
