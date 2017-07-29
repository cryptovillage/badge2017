#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include "esp_log.h"

#include "driver/i2s.h"
#include "audio_player.h"
#include "audio_renderer.h"
#include "spiram_fifo.h"
#include "http.h"
#include "url_parser.h"
#include "dac.h"

#include "audio.h"

static TaskHandle_t audio_task_handle;
static QueueHandle_t audio_request_queue;
static audio_state_t audio_state;
static player_t *player;

static const char* TAG = "audio";

static void audio_http_xfer_task(void *pvParameters)
{
    /* parse URL */
    url_t *url = url_create((char *)pvParameters);

    // blocks until end of stream
    int result = http_client_get(
            url->host, url->port, url->path,
            audio_stream_consumer,
            player);

    if(result != 0) {
        ESP_LOGD(TAG, "http_client_get error");
    } else {
        ESP_LOGD(TAG, "http_client_get completed");
    }
    // ESP_LOGI(TAG, "http_client_get stack: %d\n", uxTaskGetStackHighWaterMark(NULL));

    url_free(url);
    free(pvParameters);
    vTaskDelete(NULL);
}

static void audio_stop_impl()
{
	switch (audio_state) {
	case AUDIO_STATE_IDLE:
		return;
	case AUDIO_STATE_PLAYING_MP3_SDCARD:
	case AUDIO_STATE_PLAYING_MP3_INTERNET:
		audio_player_stop(player);
		break;
	default:
		ESP_LOGW(TAG, "audio_stop() called in unsupported state %d", audio_state);
		break;
	}
	audio_state = AUDIO_STATE_IDLE;
}

void audio_task()
{
	audio_request_t req;

	player = calloc(1, sizeof(player_t));
	player->state = STOPPED;

	renderer_config_t *renderer_config = calloc(1, sizeof(renderer_config_t));
	renderer_config->bit_depth = I2S_BITS_PER_SAMPLE_16BIT;
	renderer_config->i2s_num = I2S_NUM_0;
	renderer_config->sample_rate = 44100;
	renderer_config->output_mode = DAC_BUILT_IN;

	player->renderer_config = renderer_config;

	spiRamFifoInit();
	dac_init();
	audio_player_init(player);
	audio_state = AUDIO_STATE_IDLE;

	for (;;) {
		xQueueReceive(audio_request_queue, &req, portMAX_DELAY);
		switch (req.type) {
		case AUDIO_REQUEST_STOP:
			audio_stop_impl();
			break;
		case AUDIO_REQUEST_PLAY_MP3_INTERNET:
			audio_stop_impl();
			audio_player_start(player);
			audio_state = AUDIO_STATE_PLAYING_MP3_INTERNET;
			xTaskCreate(&audio_http_xfer_task, "audio_http_xfer", 2048, strdup(req.stream_name), 20, NULL);
			break;
		default:
			ESP_LOGI(TAG, "Unsupported audio request %d", req.type);
			break;
		}
	}
}

void audio_init()
{
	audio_request_queue = xQueueCreate(AUDIO_REQUEST_QUEUE_LENGTH, sizeof(audio_request_t));
	// We must create a new task on the APP core to initialize audio because
	// all the interrupts on the PRO core are taken
	xTaskCreatePinnedToCore(&audio_task, "audio_task", 3072, NULL, 20, &audio_task_handle, 1);
}

void audio_stop()
{
	audio_request_t req;
	req.type = AUDIO_REQUEST_STOP;
	xQueueSendToBack(audio_request_queue, &req, portMAX_DELAY);
}

void audio_play_vgm(const char *filename)
{
	audio_request_t req;
	req.type = AUDIO_REQUEST_PLAY_VGM_FILESYSTEM;
	strncpy(req.stream_name, filename, sizeof(req.stream_name) - 1);
	xQueueSendToBack(audio_request_queue, &req, portMAX_DELAY);
}

void audio_play_mp3_file(const char *filename)
{
	audio_request_t req;
	req.type = AUDIO_REQUEST_PLAY_MP3_FILESYSTEM;
	strncpy(req.stream_name, filename, sizeof(req.stream_name) - 1);
	xQueueSendToBack(audio_request_queue, &req, portMAX_DELAY);
}

void audio_stream_mp3_url(const char *url)
{
	audio_request_t req;
	req.type = AUDIO_REQUEST_PLAY_MP3_INTERNET;
	strncpy(req.stream_name, url, sizeof(req.stream_name) - 1);
	xQueueSendToBack(audio_request_queue, &req, portMAX_DELAY);
}
