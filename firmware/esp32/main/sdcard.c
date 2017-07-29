#include <stdio.h>
#include <dirent.h>
#include "esp_vfs_fat.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"

static const char* TAG = "sdcard";
sdmmc_card_t* card;

void sdcard_init()
{
	esp_err_t ret;
	sdmmc_host_t host = {
		.flags = SDMMC_HOST_FLAG_1BIT,
	    .slot = SDMMC_HOST_SLOT_1,
	    .max_freq_khz = SDMMC_FREQ_DEFAULT,
	    .io_voltage = 3.3f,
	    .init = &sdmmc_host_init,
	    .set_bus_width = &sdmmc_host_set_bus_width,
	    .set_card_clk = &sdmmc_host_set_card_clk,
	    .do_transaction = &sdmmc_host_do_transaction,
	    .deinit = &sdmmc_host_deinit
	};

    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
    		.format_if_mount_failed = false,
            .max_files = 5
    };
    ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);
    ESP_LOGI(TAG, "esp_vfs_fat_sdmmc_mount returned %d", ret);
}

void sdcard_test()
{
	DIR *dir;
	struct dirent *ent;
	dir = opendir("/sdcard/");
	if (!dir) {
		ESP_LOGI(TAG, "Unable to list contents of SD card!");
		return;
	}
	ESP_LOGI(TAG, "Listing contents of SD card:");
	while ((ent = readdir(dir))) {
		ESP_LOGI(TAG, "%s", ent->d_name);
	}
	closedir(dir);
}
