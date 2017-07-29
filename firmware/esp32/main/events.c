#include <string.h>
#include "events.h"
#include "esp_log.h"

static const char* TAG = "eventq";
QueueHandle_t eventQueue;

void eventq_insert_test_event() {
	badge_event_t event;
	memset(&event, 0, sizeof(event));
	event.type = BADGE_EVENT_KNOB_PRESSED;
	xQueueSendToBack(eventQueue, &event, portMAX_DELAY);
}

void eventq_init() {
	eventQueue = xQueueCreate(EVENT_QUEUE_LENGTH, sizeof(badge_event_t));
	if (!eventQueue) {
		ESP_LOGW(TAG, "Could not create event queue!");
	}

	// Insert a fake event for testing
	//eventq_insert_test_event();
}
