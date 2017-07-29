#include <string.h>
#include "misc.h"

#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"

static const char* tag = "misc";
static const char* nvs_namespace = "cpv2017";

static esp_err_t check_nvs_version_tag(nvs_handle handle, const char* verTag, const char* curVer) {
	char verBuf[64];
	size_t verLen;
	esp_err_t ret;

	verLen = sizeof(verBuf);
	ret = nvs_get_str(handle, verTag, verBuf, &verLen);
	if (ret == ESP_OK) {
		if (!strcmp(verBuf, curVer)) {
			return ESP_OK;
		}
	} else if (ret != ESP_ERR_NVS_NOT_FOUND) {
		ESP_LOGW(tag, "check_nvs_version_tag(\"%s\") failed: %08x", verTag, ret);
		return ret;
	}
	ret = nvs_set_str(handle, verTag, curVer);
	if (ret != ESP_OK) {
		ESP_LOGW(tag, "nvs_set_str failed: %08x", ret);
	}
	return ret;
}

esp_err_t update_nvs_version_info()
{
	esp_err_t ret;
	nvs_handle handle;

	ret = nvs_open(nvs_namespace, NVS_READWRITE, &handle);
	if (ret != ESP_OK) {
		ESP_LOGW(tag, "nvs_open failed: %08x", ret);
		return ret;
	}

	check_nvs_version_tag(handle, "app-git-ver", APP_GIT_VERSION);
	check_nvs_version_tag(handle, "esp-idf-git-ver", IDF_VER);
	nvs_close(handle);

	return ESP_OK;
}

const char *get_app_version_str()
{
	return APP_GIT_VERSION;
}
