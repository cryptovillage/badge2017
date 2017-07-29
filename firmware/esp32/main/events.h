#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_gap_ble_api.h"

#define EVENT_QUEUE_LENGTH	8

typedef enum {
	BADGE_EVENT_KNOB_CCW,
	BADGE_EVENT_KNOB_CW,
	BADGE_EVENT_KNOB_DOWN,
	BADGE_EVENT_KNOB_UP,
	BADGE_EVENT_KNOB_PRESSED,
	BADGE_EVENT_LEFT_TOUCH_PRESSED,
	BADGE_EVENT_LEFT_TOUCH_RELEASED,
	BADGE_EVENT_RIGHT_TOUCH_PRESSED,
	BADGE_EVENT_RIGHT_TOUCH_RELEASED,

	BADGE_EVENT_INCOMING_HID_MSG,
	BADGE_EVENT_POWER_CHANGE,

	BADGE_EVENT_BLE_SCAN_RESULT
} badge_event_type_t;

typedef struct {
	uint16_t battVoltage;
	uint8_t flags;
} badge_event_power_data_t;

typedef union {
	uint8_t hidMsg[64];
	uint8_t bleAdv[31]; // TODO(supersat): Replace with ble_scan_result_evt_param?
	badge_event_power_data_t powerData;
} badge_event_data_t;

typedef struct {
	badge_event_type_t type;
	badge_event_data_t data;
} badge_event_t;

extern QueueHandle_t eventQueue;
extern void eventq_init();
