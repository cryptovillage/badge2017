/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 * and Mnemote Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016, 2017 Nick Moore @mnemote
 * Copyright (c) 2017 "Eric Poulsen" <eric@zyxod.com>
 *
 * Based on esp8266/modnetwork.c which is Copyright (c) 2015 Paul Sokolovsky
 * And the ESP IDF example code which is Public Domain / CC0
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "py/nlr.h"
#include "py/objlist.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "netutils.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_log.h"
#include "esp_log.h"
#include "lwip/dns.h"

#include "modnetwork.h"

#define MODNETWORK_INCLUDE_CONSTANTS (1)

NORETURN void _esp_exceptions(esp_err_t e) {
   switch (e) {
      case ESP_ERR_WIFI_NOT_INIT: 
        mp_raise_msg(&mp_type_OSError, "Wifi Not Initialized");
      case ESP_ERR_WIFI_NOT_STARTED:
        mp_raise_msg(&mp_type_OSError, "Wifi Not Started");
      case ESP_ERR_WIFI_NOT_STOPPED:
        mp_raise_msg(&mp_type_OSError, "Wifi Not Stopped");
      case ESP_ERR_WIFI_IF:
        mp_raise_msg(&mp_type_OSError, "Wifi Invalid Interface");
      case ESP_ERR_WIFI_MODE:
        mp_raise_msg(&mp_type_OSError, "Wifi Invalid Mode");
      case ESP_ERR_WIFI_STATE:
        mp_raise_msg(&mp_type_OSError, "Wifi Internal State Error");
      case ESP_ERR_WIFI_CONN:
        mp_raise_msg(&mp_type_OSError, "Wifi Internal Error");
      case ESP_ERR_WIFI_NVS:
        mp_raise_msg(&mp_type_OSError, "Wifi Internal NVS Error");
      case ESP_ERR_WIFI_MAC:
        mp_raise_msg(&mp_type_OSError, "Wifi Invalid MAC Address");
      case ESP_ERR_WIFI_SSID:
        mp_raise_msg(&mp_type_OSError, "Wifi SSID Invalid");
      case ESP_ERR_WIFI_PASSWORD:
        mp_raise_msg(&mp_type_OSError, "Wifi Invalid Password");
      case ESP_ERR_WIFI_TIMEOUT:
        mp_raise_OSError(MP_ETIMEDOUT);
      case ESP_ERR_WIFI_WAKE_FAIL:
        mp_raise_msg(&mp_type_OSError, "Wifi Wakeup Failure");
      case ESP_ERR_WIFI_WOULD_BLOCK:
        mp_raise_msg(&mp_type_OSError, "Wifi Would Block");
      case ESP_ERR_WIFI_NOT_CONNECT:
        mp_raise_msg(&mp_type_OSError, "Wifi Not Connected");
      case ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS:
        mp_raise_msg(&mp_type_OSError, "TCP/IP Invalid Parameters");
      case ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY:
        mp_raise_msg(&mp_type_OSError, "TCP/IP IF Not Ready");
      case ESP_ERR_TCPIP_ADAPTER_DHCPC_START_FAILED:
        mp_raise_msg(&mp_type_OSError, "TCP/IP DHCP Client Start Failed");
      case ESP_ERR_TCPIP_ADAPTER_NO_MEM:
        mp_raise_OSError(MP_ENOMEM); 
      default:
        nlr_raise(mp_obj_new_exception_msg_varg(
          &mp_type_RuntimeError, "Wifi Unknown Error 0x%04x", e
        ));
   }
}

static inline void esp_exceptions(esp_err_t e) {
    if (e != ESP_OK) _esp_exceptions(e);
}

#define ESP_EXCEPTIONS(x) do { esp_exceptions(x); } while (0);

STATIC mp_obj_t esp_connect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_ssid, ARG_password, ARG_bssid };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ssid, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_password, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_bssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    wifi_config_t wifi_sta_config = {{{0}}};

    // configure any parameters that are given
    if (n_args > 1) {
        mp_uint_t len;
        const char *p;
        if (args[ARG_ssid].u_obj != mp_const_none) {
            p = mp_obj_str_get_data(args[ARG_ssid].u_obj, &len);
            memcpy(wifi_sta_config.sta.ssid, p, MIN(len, sizeof(wifi_sta_config.sta.ssid)));
        }
        if (args[ARG_password].u_obj != mp_const_none) {
            p = mp_obj_str_get_data(args[ARG_password].u_obj, &len);
            memcpy(wifi_sta_config.sta.password, p, MIN(len, sizeof(wifi_sta_config.sta.password)));
        }
        if (args[ARG_bssid].u_obj != mp_const_none) {
            p = mp_obj_str_get_data(args[ARG_bssid].u_obj, &len);
            if (len != sizeof(wifi_sta_config.sta.bssid)) {
                mp_raise_ValueError(NULL);
            }
            wifi_sta_config.sta.bssid_set = 1;
            memcpy(wifi_sta_config.sta.bssid, p, sizeof(wifi_sta_config.sta.bssid));
        }
        ESP_EXCEPTIONS( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_sta_config) );
    }

    // connect to the WiFi AP
    MP_THREAD_GIL_EXIT();
    ESP_EXCEPTIONS( esp_wifi_connect() );
    MP_THREAD_GIL_ENTER();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(esp_connect_obj, 0, esp_connect);

STATIC mp_obj_t esp_disconnect() {
    ESP_EXCEPTIONS( esp_wifi_disconnect() );
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_disconnect_obj, esp_disconnect);

STATIC mp_obj_t esp_scan() {
    // check that STA mode is active
    wifi_mode_t mode;
    ESP_EXCEPTIONS(esp_wifi_get_mode(&mode));
    if ((mode & WIFI_MODE_STA) == 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "STA must be active"));
    }

    mp_obj_t list = mp_obj_new_list(0, NULL);
    wifi_scan_config_t config = { 0 };
    // XXX how do we scan hidden APs (and if we can scan them, are they really hidden?)
    MP_THREAD_GIL_EXIT();
    esp_err_t status = esp_wifi_scan_start(&config, 1);
    MP_THREAD_GIL_ENTER();
    if (status == 0) {
        uint16_t count = 0;
        ESP_EXCEPTIONS( esp_wifi_scan_get_ap_num(&count) );
        wifi_ap_record_t *wifi_ap_records = calloc(count, sizeof(wifi_ap_record_t));
        ESP_EXCEPTIONS( esp_wifi_scan_get_ap_records(&count, wifi_ap_records) );
        for (uint16_t i = 0; i < count; i++) {
            mp_obj_tuple_t *t = mp_obj_new_tuple(6, NULL);
            uint8_t *x = memchr(wifi_ap_records[i].ssid, 0, sizeof(wifi_ap_records[i].ssid));
            int ssid_len = x ? x - wifi_ap_records[i].ssid : sizeof(wifi_ap_records[i].ssid);
            t->items[0] = mp_obj_new_bytes(wifi_ap_records[i].ssid, ssid_len);
            t->items[1] = mp_obj_new_bytes(wifi_ap_records[i].bssid, sizeof(wifi_ap_records[i].bssid));
            t->items[2] = MP_OBJ_NEW_SMALL_INT(wifi_ap_records[i].primary);
            t->items[3] = MP_OBJ_NEW_SMALL_INT(wifi_ap_records[i].rssi);
            t->items[4] = MP_OBJ_NEW_SMALL_INT(wifi_ap_records[i].authmode);
            t->items[5] = mp_const_false; // XXX hidden?
            mp_obj_list_append(list, MP_OBJ_FROM_PTR(t));
        }
        free(wifi_ap_records);
    }
    return list;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_scan_obj, esp_scan);



STATIC const mp_rom_map_elem_t mp_module_network_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_network) },
    //{ MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&esp_initialize_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&esp_connect_obj) },
	{ MP_ROM_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&esp_disconnect_obj) },
	{ MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&esp_scan_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_network_globals, mp_module_network_globals_table);

const mp_obj_module_t mp_module_network = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_network_globals,
};
