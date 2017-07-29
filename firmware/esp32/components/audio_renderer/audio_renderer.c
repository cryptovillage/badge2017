/*
 * audio_renderer.c
 *
 *  Created on: 12.03.2017
 *      Author: michaelboeckling
 */

#include <stdbool.h>

#include "freertos/FreeRTOS.h"

#include "esp_log.h"
#include "driver/i2s.h"

#include "audio_renderer.h"

#define TAG "audio_renderer"

typedef enum {
     RENDER_ACTIVE, RENDER_STOPPED
} renderer_state_t;


/* TODO: refactor */
static renderer_config_t *curr_config;
static renderer_state_t state;

static int convert_16bit_stereo_to_16bit_stereo(short left, short right)
{
    left += 0x8000;
    right += 0x8000;
    return (left << 16 & 0xffff0000) | ((unsigned short) right);
}

/* render callback for the libmad synth */
void render_sample_block(short *sample_buff_ch0, short *sample_buff_ch1, int num_samples, unsigned int num_channels)
{

    // if mono: simply duplicate the left channel
    if(num_channels == 1) {
        sample_buff_ch1 = sample_buff_ch0;
    }

    // max delay: 50ms instead of portMAX_DELAY
    TickType_t delay = 50 / portTICK_PERIOD_MS;

    if (curr_config->bit_depth != 16) {
    	ESP_LOGE(TAG, "Unsupported bit depth: %d", curr_config->bit_depth);
    	return;
    }

    for (int i=0; i < num_samples; i++) {
    	if(state == RENDER_STOPPED)
    		break;

        int samp16 = convert_16bit_stereo_to_16bit_stereo(sample_buff_ch0[i], sample_buff_ch1[i]);
        int bytes_pushed = i2s_push_sample(curr_config->i2s_num,  (char *)&samp16, delay);

        // DMA buffer full - retry
        if(bytes_pushed == 0) {
            i--;
        }
    }
}


// Called by the NXP modifications of libmad. Sets the needed output sample rate.
static int prevRate;
void set_dac_sample_rate(int rate)
{
    if(rate == prevRate)
        return;
    prevRate = rate;

    ESP_LOGD(TAG, "setting sample rate to %d\n", rate);
    i2s_set_sample_rates(curr_config->i2s_num, rate);
}

/* init renderer sink */
void audio_renderer_init(renderer_config_t *config)
{
    // update global
    curr_config = config;
    state = RENDER_STOPPED;
}

void audio_renderer_start(renderer_config_t *config)
{
    // update global
    curr_config = config;
    state = RENDER_ACTIVE;
    i2s_start(config->i2s_num);

    // buffer might contain noise
    i2s_zero_dma_buffer(config->i2s_num);
}

void audio_renderer_stop(renderer_config_t *config)
{
    state = RENDER_STOPPED;
    i2s_stop(config->i2s_num);
}
