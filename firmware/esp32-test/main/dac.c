#include "freertos/FreeRTOS.h"
#include "driver/i2s.h"
#include "board.h"

static const i2s_config_t i2s_config = {
     .mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN,
     .sample_rate = 44100,
     .bits_per_sample = 16,
     .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
     .communication_format = I2S_COMM_FORMAT_I2S_MSB,
     .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // high interrupt priority
     .dma_buf_count = 8,
     .dma_buf_len = 64
};

void dac_init() {
	ESP_ERROR_CHECK(i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL));
	ESP_ERROR_CHECK(i2s_set_pin(I2S_NUM_0, NULL)); // NULL for internal DAC
	ESP_ERROR_CHECK(i2s_set_sample_rates(I2S_NUM_0, 44100));
	ESP_ERROR_CHECK(i2s_zero_dma_buffer(I2S_NUM_0));
	//ESP_ERROR_CHECK(i2s_stop(I2S_NUM_0));
}

void dac_start() {
	ESP_ERROR_CHECK(i2s_start(I2S_NUM_0));
}

void dac_stop() {
	ESP_ERROR_CHECK(i2s_stop(I2S_NUM_0));
}

void dac_sample(uint8_t left, uint8_t right) {
	char samples[2];
	samples[0] = left;
	samples[1] = right;
	i2s_push_sample(I2S_NUM_0, &samples, portMAX_DELAY);
}

void dac_samples(const char *buf, int len) {
	i2s_write_bytes(I2S_NUM_0, buf, len * 4, portMAX_DELAY);
}
