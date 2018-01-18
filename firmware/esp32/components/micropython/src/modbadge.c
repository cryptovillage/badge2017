#include <string.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_gap_ble_api.h"

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"

// FIXME(supersat): This is a little broken because components shoudn't depend
// on the main application... But this is #badgelife
#include "../../../main/ble.h"
#include "../../../main/events.h"
#include "../../../main/leds.h"
#include "../../../main/i2c.h"
#include "../../../main/audio.h"
#include "../../../main/gf.h"
#include "../../../main/misc.h"

static const char* TAG = "modbadge";

typedef struct _badge_event_type_obj_t {
    mp_obj_base_t base;
    badge_event_t event;
} badge_event_obj_t;

// TODO(supersat): Make this return an actual type instead of a number?
STATIC mp_obj_t badge_event_get_type(const mp_obj_t event) {
    badge_event_obj_t *eo = MP_OBJ_TO_PTR(event);
    return mp_obj_new_int(eo->event.type);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(badge_event_get_type_obj, badge_event_get_type);

// TODO(supersat): Return useful Python objects instead of a bytearray
STATIC mp_obj_t badge_event_get_data(const mp_obj_t event) {
    badge_event_obj_t *eo = MP_OBJ_TO_PTR(event);
    return mp_obj_new_bytes((const uint8_t *)&eo->event.data, sizeof(badge_event_data_t));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(badge_event_get_data_obj, badge_event_get_data);

STATIC const mp_map_elem_t badge_event_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_type), (mp_obj_t)&badge_event_get_type_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_data), (mp_obj_t)&badge_event_get_data_obj },
};

STATIC MP_DEFINE_CONST_DICT(badge_event_locals_dict, badge_event_locals_dict_table);

STATIC void badge_event_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    badge_event_obj_t *eo = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<Event type=%d>", eo->event.type);
}

STATIC const mp_obj_type_t badge_event_type = {
    { &mp_type_type },
    .name = MP_QSTR_Event,
	.print = badge_event_print,
	.locals_dict = (mp_obj_t)&badge_event_locals_dict
};

// TODO(supersat): Make this take an optional timeout value
STATIC mp_obj_t badge_get_event() {
	badge_event_obj_t *eventObj;
	eventObj = m_new_obj(badge_event_obj_t);
	MP_THREAD_GIL_EXIT();
	xQueueReceive(eventQueue, &eventObj->event, portMAX_DELAY);
	eventObj->base.type = &badge_event_type;
	MP_THREAD_GIL_ENTER();
	return MP_OBJ_FROM_PTR(eventObj);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(badge_get_event_obj, badge_get_event);

STATIC mp_obj_t badge_set_backlight_brightness(mp_obj_t value_in) {
	int value = mp_obj_get_int(value_in);
	if (value < 0 || value > 255) mp_raise_ValueError("Value out of range");
	i2c_set_backlight_brightness(value);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(badge_set_backlight_brightness_obj, badge_set_backlight_brightness);

STATIC mp_obj_t badge_config_max9723(mp_obj_t value_in) {
	int value = mp_obj_get_int(value_in);
	if (value < 0 || value > 255) mp_raise_ValueError("Value out of range");
	i2c_config_max9723(value);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(badge_config_max9723_obj, badge_config_max9723);

STATIC mp_obj_t badge_update_leds(mp_obj_t led_buf_in) {
	mp_buffer_info_t bufinfo;
	mp_get_buffer_raise(led_buf_in, &bufinfo, MP_BUFFER_READ);
	memcpy(ledBuf, bufinfo.buf, bufinfo.len > 32 ? 32 : bufinfo.len);
	i2c_send_led_cmd(ledBuf);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(badge_update_leds_obj, badge_update_leds);

STATIC mp_obj_t badge_ble_set_addr(mp_obj_t ble_adv_data_obj) {
	mp_buffer_info_t bufinfo;
	mp_get_buffer_raise(ble_adv_data_obj, &bufinfo, MP_BUFFER_READ);
	if (bufinfo.len != 6) {
		mp_raise_ValueError("Address must be exactly six bytes long");
	}
	esp_err_t ret = esp_ble_gap_set_rand_addr(*(esp_bd_addr_t *)bufinfo.buf);
	if (ret != ESP_OK) {
		ESP_LOGW(TAG, "esp_ble_gap_set_rand_addr returned %08x", ret);
	}

	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(badge_ble_set_addr_obj, badge_ble_set_addr);

STATIC mp_obj_t badge_ble_set_adv_std_data(mp_obj_t name_obj, mp_obj_t badge_defined_data_obj) {
	mp_buffer_info_t badge_defined_data_info;
	size_t nameLen;
	const char *name = mp_obj_str_get_data(name_obj, &nameLen);
	mp_get_buffer_raise(badge_defined_data_obj, &badge_defined_data_info, MP_BUFFER_READ);
	if (badge_defined_data_info.len != 8) mp_raise_ValueError("Badge-defined data must be exactly 8 bytes");
	ble_gap_set_adv_std_data(name, nameLen, badge_defined_data_info.buf);

	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(badge_ble_set_adv_std_data_obj, badge_ble_set_adv_std_data);

STATIC mp_obj_t badge_ble_set_adv_custom_data(mp_obj_t ble_adv_custom_data_obj) {
	mp_buffer_info_t bufinfo;
	mp_get_buffer_raise(ble_adv_custom_data_obj, &bufinfo, MP_BUFFER_READ);
	if (bufinfo.len > 31) mp_raise_ValueError("GAP data can not be more than 31 bytes");
	ble_gap_set_adv_custom_data(bufinfo.buf, bufinfo.len);

	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(badge_ble_set_adv_custom_data_obj, badge_ble_set_adv_custom_data);

STATIC mp_obj_t badge_ble_set_adv_std_data_enabled(mp_obj_t enabled_obj) {
	ble_gap_set_adv_std_data_enabled(mp_obj_is_true(enabled_obj));
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(badge_ble_set_adv_std_data_enabled_obj, badge_ble_set_adv_std_data_enabled);

STATIC mp_obj_t badge_ble_set_adv_custom_data_enabled(mp_obj_t enabled_obj) {
	ble_gap_set_adv_custom_data_enabled(mp_obj_is_true(enabled_obj));
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(badge_ble_set_adv_custom_data_enabled_obj, badge_ble_set_adv_custom_data_enabled);

STATIC mp_obj_t badge_get_os_heap_free() {
	return mp_obj_new_int(xPortGetFreeHeapSize());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(badge_get_os_heap_free_obj, badge_get_os_heap_free);

STATIC mp_obj_t badge_get_os_minimum_ever_heap_free() {
	return mp_obj_new_int(xPortGetMinimumEverFreeHeapSize());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(badge_get_os_minimum_ever_heap_free_obj, badge_get_os_minimum_ever_heap_free);

STATIC mp_obj_t badge_audio_stop() {
	audio_stop();
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(badge_audio_stop_obj, badge_audio_stop);

STATIC mp_obj_t badge_audio_stream_mp3_url(mp_obj_t url) {
	audio_stream_mp3_url(mp_obj_str_get_str(url));
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(badge_audio_stream_mp3_url_obj, badge_audio_stream_mp3_url);

STATIC mp_obj_t badge_gf_computeShards(mp_obj_t key_obj, mp_obj_t threshold_obj, mp_obj_t num_shards_obj)
{
	size_t threshold = mp_obj_get_int(threshold_obj);
	size_t num_shards = mp_obj_get_int(num_shards_obj);
	mp_buffer_info_t key_buf_info;
	uint8_t** raw_shards;
	mp_obj_t shardList;
	mp_obj_t *shards;
	size_t i;

	mp_get_buffer_raise(key_obj, &key_buf_info, MP_BUFFER_READ);
	shards = alloca(sizeof(mp_obj_t *) * num_shards);
	raw_shards = alloca(sizeof(uint8_t *) * num_shards);
	for (i = 0; i < num_shards; i++) {
		raw_shards[i] = m_new(byte, key_buf_info.len);
		shards[i] = mp_obj_new_bytearray_by_ref(key_buf_info.len, raw_shards[i]);
	}
	shardList = mp_obj_new_list(num_shards, shards);
	gf_computeShards((uint8_t *)key_buf_info.buf, key_buf_info.len, raw_shards, threshold, num_shards);

	return shardList;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(badge_gf_computeShards_obj, badge_gf_computeShards);

STATIC mp_obj_t badge_gf_computeCombine(mp_obj_t shares_x_obj, mp_obj_t shards_obj)
{
	mp_buffer_info_t shard_buf_info;
	//mp_buffer_info_t shares_x_info;
	size_t shard_len = 0;
	size_t num_x_coords = 0;
	size_t num_shards = 0;
	mp_obj_t *x_coords;
	mp_obj_t *shards;
	uint8_t** raw_shards = NULL;
	uint8_t* out = NULL;
	uint8_t* x_coords_buf = NULL;
	size_t i;

	//mp_get_buffer_raise(shares_x_obj, &shares_x_info, MP_BUFFER_READ);
	mp_obj_get_array(shares_x_obj, &num_x_coords, &x_coords);
	mp_obj_get_array(shards_obj, &num_shards, &shards);
	if (num_x_coords != num_shards) {
		mp_raise_ValueError("x coordinate list and shard list must be the same length");
	}

	x_coords_buf = alloca(num_x_coords);
	for (i = 0; i < num_x_coords; i++) {
		x_coords_buf[i] = mp_obj_get_int(x_coords[i]);
	}

	for (i = 0; i < num_shards; i++) {
		mp_get_buffer_raise(shards[i], &shard_buf_info, MP_BUFFER_READ);
		if (i == 0) {
			shard_len = shard_buf_info.len;
			out = alloca(shard_buf_info.len);
			raw_shards = alloca(sizeof(uint8_t *) * num_shards);
		} else {
			if (shard_buf_info.len != shard_len) {
				mp_raise_ValueError("All shard lengths must be the same");
			}
		}
		raw_shards[i] = (uint8_t *)shard_buf_info.buf;
	}
	gf_computeCombine(x_coords_buf, raw_shards, num_shards, shard_len, out);
	return mp_obj_new_bytearray(shard_len, out);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(badge_gf_computeCombine_obj, badge_gf_computeCombine);

// TODO: Make this a QSTR in flash
STATIC mp_obj_t badge_get_esp_idf_version()
{
	return mp_obj_new_str(IDF_VER, strlen(IDF_VER), false);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(badge_get_esp_idf_version_obj, badge_get_esp_idf_version);

STATIC mp_obj_t badge_get_app_version()
{
	const char* verStr = get_app_version_str();
	return mp_obj_new_str(verStr, strlen(verStr), false);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(badge_get_app_version_obj, badge_get_app_version);

STATIC const mp_rom_map_elem_t badge_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_badge) },

	{ MP_ROM_QSTR(MP_QSTR_get_event), MP_ROM_PTR(&badge_get_event_obj) },
	{ MP_ROM_QSTR(MP_QSTR_set_backlight_brightness), MP_ROM_PTR(&badge_set_backlight_brightness_obj) },
	{ MP_ROM_QSTR(MP_QSTR_config_max9723), MP_ROM_PTR(&badge_config_max9723_obj) },
    { MP_ROM_QSTR(MP_QSTR_update_leds), MP_ROM_PTR(&badge_update_leds_obj) },
	{ MP_ROM_QSTR(MP_QSTR_ble_set_addr), MP_ROM_PTR(&badge_ble_set_addr_obj) },
	{ MP_ROM_QSTR(MP_QSTR_ble_set_adv_std_data), MP_ROM_PTR(&badge_ble_set_adv_std_data_obj) },
	{ MP_ROM_QSTR(MP_QSTR_ble_set_adv_custom_data), MP_ROM_PTR(&badge_ble_set_adv_custom_data_obj) },
	{ MP_ROM_QSTR(MP_QSTR_ble_set_adv_std_data_enabled), MP_ROM_PTR(&badge_ble_set_adv_std_data_enabled_obj) },
	{ MP_ROM_QSTR(MP_QSTR_ble_set_adv_custom_data_enabled), MP_ROM_PTR(&badge_ble_set_adv_custom_data_enabled_obj) },
	{ MP_ROM_QSTR(MP_QSTR_get_os_heap_free), MP_ROM_PTR(&badge_get_os_heap_free_obj) },
	{ MP_ROM_QSTR(MP_QSTR_get_os_minimum_ever_heap_free), MP_ROM_PTR(&badge_get_os_minimum_ever_heap_free_obj) },
	{ MP_ROM_QSTR(MP_QSTR_audio_stop), MP_ROM_PTR(&badge_audio_stop_obj) },
	{ MP_ROM_QSTR(MP_QSTR_audio_stream_mp3_url), MP_ROM_PTR(&badge_audio_stream_mp3_url_obj) },
	{ MP_ROM_QSTR(MP_QSTR_gf_computeShards), MP_ROM_PTR(&badge_gf_computeShards_obj) },
	{ MP_ROM_QSTR(MP_QSTR_gf_computeCombine), MP_ROM_PTR(&badge_gf_computeCombine_obj) },
	{ MP_ROM_QSTR(MP_QSTR_get_esp_idf_version), MP_ROM_PTR(&badge_get_esp_idf_version_obj) },
	{ MP_ROM_QSTR(MP_QSTR_get_app_version), MP_ROM_PTR(&badge_get_app_version_obj) },
};

STATIC MP_DEFINE_CONST_DICT(badge_module_globals, badge_module_globals_table);

const mp_obj_module_t badge_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&badge_module_globals,
};
