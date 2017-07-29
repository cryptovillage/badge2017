#include <string.h>
#include <alloca.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#include "u8g2.h"

extern u8g2_t u8g2;

typedef struct _u8g2_font_t {
    mp_obj_base_t base;
    const uint8_t * font;
} u8g2_font_obj_t;

STATIC void u8g2_font_type_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    u8g2_font_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<U8G2_Font %08p>", self->font);
}

STATIC const mp_obj_type_t u8g2_font_type = {
    { &mp_type_type },
    .name = MP_QSTR_U8G2_Font,
	.print = u8g2_font_type_print,
};

STATIC const u8g2_font_obj_t u8g2_font_profont12_mr_obj = { { &u8g2_font_type }, u8g2_font_profont12_mr };
STATIC const u8g2_font_obj_t u8g2_font_profont22_mr_obj = { { &u8g2_font_type }, u8g2_font_profont22_mr };

STATIC mp_obj_t u8g2_clearBuffer() {
	u8g2_ClearBuffer(&u8g2);
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(u8g2_clearBuffer_obj, u8g2_clearBuffer);

STATIC mp_obj_t u8g2_clearDisplay() {
	u8g2_ClearDisplay(&u8g2);
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(u8g2_clearDisplay_obj, u8g2_clearDisplay);

STATIC mp_obj_t u8g2_drawBox(size_t n_args, const mp_obj_t *args) {
	u8g2_DrawBox(&u8g2, mp_obj_get_int(args[0]), mp_obj_get_int(args[1]),
			mp_obj_get_int(args[2]), mp_obj_get_int(args[3]));
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(u8g2_drawBox_obj, 4, 4, u8g2_drawBox);

STATIC mp_obj_t u8g2_drawCircle(size_t n_args, const mp_obj_t *args) {
	u8g2_DrawCircle(&u8g2, mp_obj_get_int(args[0]), mp_obj_get_int(args[1]),
			mp_obj_get_int(args[2]), mp_obj_get_int(args[3]));
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(u8g2_drawCircle_obj, 4, 4, u8g2_drawCircle);

STATIC mp_obj_t u8g2_drawDisc(size_t n_args, const mp_obj_t *args) {
	u8g2_DrawDisc(&u8g2, mp_obj_get_int(args[0]), mp_obj_get_int(args[1]),
			mp_obj_get_int(args[2]), mp_obj_get_int(args[3]));
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(u8g2_drawDisc_obj, 4, 4, u8g2_drawDisc);

STATIC mp_obj_t u8g2_drawEllipse(size_t n_args, const mp_obj_t *args) {
	u8g2_DrawEllipse(&u8g2, mp_obj_get_int(args[0]), mp_obj_get_int(args[1]),
			mp_obj_get_int(args[2]), mp_obj_get_int(args[3]), mp_obj_get_int(args[4]));
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(u8g2_drawEllipse_obj, 5, 5, u8g2_drawEllipse);

STATIC mp_obj_t u8g2_drawFilledEllipse(size_t n_args, const mp_obj_t *args) {
	u8g2_DrawFilledEllipse(&u8g2, mp_obj_get_int(args[0]), mp_obj_get_int(args[1]),
			mp_obj_get_int(args[2]), mp_obj_get_int(args[3]), mp_obj_get_int(args[4]));
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(u8g2_drawFilledEllipse_obj, 5, 5, u8g2_drawFilledEllipse);

STATIC mp_obj_t u8g2_drawFrame(size_t n_args, const mp_obj_t *args) {
	u8g2_DrawFrame(&u8g2, mp_obj_get_int(args[0]), mp_obj_get_int(args[1]),
			mp_obj_get_int(args[2]), mp_obj_get_int(args[3]));
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(u8g2_drawFrame_obj, 4, 4, u8g2_drawFrame);

STATIC mp_obj_t u8g2_drawGlyph(mp_obj_t x, mp_obj_t y, mp_obj_t encoding) {
	u8g2_DrawGlyph(&u8g2, mp_obj_get_int(x), mp_obj_get_int(y), mp_obj_get_int(encoding));
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(u8g2_drawGlyph_obj, u8g2_drawGlyph);

STATIC mp_obj_t u8g2_drawHLine(mp_obj_t x, mp_obj_t y, mp_obj_t w) {
	u8g2_DrawHLine(&u8g2, mp_obj_get_int(x), mp_obj_get_int(y), mp_obj_get_int(w));
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(u8g2_drawHLine_obj, u8g2_drawHLine);

STATIC mp_obj_t u8g2_drawLine(size_t n_args, const mp_obj_t *args) {
	u8g2_DrawLine(&u8g2, mp_obj_get_int(args[0]), mp_obj_get_int(args[1]),
			mp_obj_get_int(args[2]), mp_obj_get_int(args[3]));
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(u8g2_drawLine_obj, 4, 4, u8g2_drawLine);

STATIC mp_obj_t u8g2_drawPixel(mp_obj_t x, mp_obj_t y) {
	u8g2_DrawPixel(&u8g2, mp_obj_get_int(x), mp_obj_get_int(y));
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(u8g2_drawPixel_obj, u8g2_drawPixel);

STATIC mp_obj_t u8g2_drawRBox(size_t n_args, const mp_obj_t *args) {
	u8g2_DrawRBox(&u8g2, mp_obj_get_int(args[0]), mp_obj_get_int(args[1]),
			mp_obj_get_int(args[2]), mp_obj_get_int(args[3]), mp_obj_get_int(args[4]));
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(u8g2_drawRBox_obj, 4, 4, u8g2_drawRBox);

STATIC mp_obj_t u8g2_drawRFrame(size_t n_args, const mp_obj_t *args) {
	u8g2_DrawRFrame(&u8g2, mp_obj_get_int(args[0]), mp_obj_get_int(args[1]),
			mp_obj_get_int(args[2]), mp_obj_get_int(args[3]), mp_obj_get_int(args[4]));
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(u8g2_drawRFrame_obj, 4, 4, u8g2_drawRFrame);

STATIC mp_obj_t u8g2_drawStr(mp_obj_t x, mp_obj_t y, mp_obj_t s) {
	mp_uint_t len;
	const char *cstr = mp_obj_str_get_data(s, &len);
	u8g2_DrawStr(&u8g2, mp_obj_get_int(x), mp_obj_get_int(y), cstr);
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(u8g2_drawStr_obj, u8g2_drawStr);

STATIC mp_obj_t u8g2_drawTriangle(size_t n_args, const mp_obj_t *args) {
	u8g2_DrawTriangle(&u8g2, mp_obj_get_int(args[0]), mp_obj_get_int(args[1]), mp_obj_get_int(args[2]),
			mp_obj_get_int(args[3]), mp_obj_get_int(args[4]), mp_obj_get_int(args[5]));
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(u8g2_drawTriangle_obj, 6, 6, u8g2_drawTriangle);

STATIC mp_obj_t u8g2_drawUTF8(mp_obj_t x, mp_obj_t y, mp_obj_t s) {
	mp_uint_t len;
	const char *cstr = mp_obj_str_get_data(s, &len);
	u8g2_DrawUTF8(&u8g2, mp_obj_get_int(x), mp_obj_get_int(y), cstr);
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(u8g2_drawUTF8_obj, u8g2_drawUTF8);

STATIC mp_obj_t u8g2_drawVLine(mp_obj_t x, mp_obj_t y, mp_obj_t h) {
	u8g2_DrawVLine(&u8g2, mp_obj_get_int(x), mp_obj_get_int(y), mp_obj_get_int(h));
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(u8g2_drawVLine_obj, u8g2_drawVLine);

STATIC mp_obj_t u8g2_drawXBM(size_t n_args, const mp_obj_t *args) {
	mp_buffer_info_t bufinfo;
	mp_get_buffer_raise(args[4], &bufinfo, MP_BUFFER_READ);
	u8g2_DrawXBM(&u8g2, mp_obj_get_int(args[0]), mp_obj_get_int(args[1]),
			mp_obj_get_int(args[2]), mp_obj_get_int(args[3]), (uint8_t *)bufinfo.buf);
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(u8g2_drawXBM_obj, 5, 5, u8g2_drawXBM);

STATIC mp_obj_t u8g2_firstPage() {
	u8g2_FirstPage(&u8g2);
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(u8g2_firstPage_obj, u8g2_firstPage);

STATIC mp_obj_t u8g2_getAscent() {
	return mp_obj_new_int(u8g2_GetAscent(&u8g2));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(u8g2_getAscent_obj, u8g2_getAscent);

STATIC mp_obj_t u8g2_getDescent() {
	return mp_obj_new_int(u8g2_GetDescent(&u8g2));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(u8g2_getDescent_obj, u8g2_getDescent);

STATIC mp_obj_t u8g2_getStrWidth(mp_obj_t s) {
	mp_uint_t len;
	const char *cstr = mp_obj_str_get_data(s, &len);
	return mp_obj_new_int_from_uint(u8g2_GetStrWidth(&u8g2, cstr));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(u8g2_getStrWidth_obj, u8g2_getStrWidth);

STATIC mp_obj_t u8g2_getUTF8Width(mp_obj_t s) {
	mp_uint_t len;
	const char *cstr = mp_obj_str_get_data(s, &len);
	return mp_obj_new_int_from_uint(u8g2_GetUTF8Width(&u8g2, cstr));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(u8g2_getUTF8Width_obj, u8g2_getUTF8Width);

STATIC mp_obj_t u8g2_nextPage() {
	u8g2_NextPage(&u8g2);
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(u8g2_nextPage_obj, u8g2_nextPage);

STATIC mp_obj_t u8g2_sendBuffer() {
	u8g2_SendBuffer(&u8g2);
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(u8g2_sendBuffer_obj, u8g2_sendBuffer);

STATIC mp_obj_t u8g2_setAutoPageClear(mp_obj_t mode) {
	u8g2_SetAutoPageClear(&u8g2, mp_obj_is_true(mode));
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(u8g2_setAutoPageClear_obj, u8g2_setAutoPageClear);

STATIC mp_obj_t u8g2_setBitmapMode(mp_obj_t mode) {
	u8g2_SetBitmapMode(&u8g2, mp_obj_is_true(mode));
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(u8g2_setBitmapMode_obj, u8g2_setBitmapMode);

STATIC mp_obj_t u8g2_setDisplayRotation(mp_obj_t rotation) {
	const u8g2_cb_t *u8g2_cb;

	switch (mp_obj_get_int(rotation)) {
	case 0:
		u8g2_cb = U8G2_R0;
		break;
	case 90:
		u8g2_cb = U8G2_R1;
		break;
	case 180:
		u8g2_cb = U8G2_R2;
		break;
	case 270:
		u8g2_cb = U8G2_R3;
		break;
	default:
		nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "argument must be 0, 90, 180, or 270"));
		break;
	}
	u8g2_SetDisplayRotation(&u8g2, u8g2_cb);
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(u8g2_setDisplayRotation_obj, u8g2_setDisplayRotation);

STATIC mp_obj_t u8g2_setDrawColor(mp_obj_t color) {
	u8g2_SetDrawColor(&u8g2, mp_obj_get_int(color));
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(u8g2_setDrawColor_obj, u8g2_setDrawColor);

STATIC mp_obj_t u8g2_setFlipMode(mp_obj_t is_enable) {
	u8g2_SetFlipMode(&u8g2, mp_obj_is_true(is_enable));
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(u8g2_setFlipMode_obj, u8g2_setFlipMode);

STATIC mp_obj_t u8g2_setFont(mp_obj_t font_obj) {
	u8g2_font_obj_t *fo = font_obj;

	// TODO(supersat): Support arbitrary font data as a bytearray, but need to add to GC roots
	if (MP_OBJ_IS_TYPE(font_obj, &u8g2_font_type)) {
		u8g2_SetFont(&u8g2, fo->font);
	} else if (MP_OBJ_IS_STR(font_obj)){
		nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "argument must be a U8G2_Font"));
	}

	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(u8g2_setFont_obj, u8g2_setFont);


STATIC mp_obj_t u8g2_setFontDirection(mp_obj_t dir) {
	u8g2_SetFontDirection(&u8g2, mp_obj_get_int(dir));
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(u8g2_setFontDirection_obj, u8g2_setFontDirection);

STATIC mp_obj_t u8g2_setFontMode(mp_obj_t is_transparent) {
	u8g2_SetFontMode(&u8g2, mp_obj_get_int(is_transparent));
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(u8g2_setFontMode_obj, u8g2_setFontMode);

STATIC mp_obj_t u8g2_setFontPosBaseline() {
	u8g2_SetFontPosBaseline(&u8g2);
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(u8g2_setFontPosBaseline_obj, u8g2_setFontPosBaseline);

STATIC mp_obj_t u8g2_setFontPosBottom() {
	u8g2_SetFontPosBottom(&u8g2);
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(u8g2_setFontPosBottom_obj, u8g2_setFontPosBottom);

STATIC mp_obj_t u8g2_setFontPosTop() {
	u8g2_SetFontPosTop(&u8g2);
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(u8g2_setFontPosTop_obj, u8g2_setFontPosTop);

STATIC mp_obj_t u8g2_setFontPosCenter() {
	u8g2_SetFontPosCenter(&u8g2);
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(u8g2_setFontPosCenter_obj, u8g2_setFontPosCenter);

STATIC mp_obj_t u8g2_setFontRefHeightAll() {
	u8g2_SetFontRefHeightAll(&u8g2);
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(u8g2_setFontRefHeightAll_obj, u8g2_setFontRefHeightAll);

STATIC mp_obj_t u8g2_setFontRefHeightExtendedText() {
	u8g2_SetFontRefHeightExtendedText(&u8g2);
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(u8g2_setFontRefHeightExtendedText_obj, u8g2_setFontRefHeightExtendedText);

STATIC mp_obj_t u8g2_setFontRefHeightText() {
	u8g2_SetFontRefHeightText(&u8g2);
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(u8g2_setFontRefHeightText_obj, u8g2_setFontRefHeightText);

STATIC mp_obj_t u8g2_setPowerSave(mp_obj_t is_enable) {
	u8g2_SetPowerSave(&u8g2, mp_obj_get_int(is_enable));
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(u8g2_setPowerSave_obj, u8g2_setPowerSave);

STATIC const mp_rom_map_elem_t u8g2_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_u8g2) },

    { MP_ROM_QSTR(MP_QSTR_clearBuffer), MP_ROM_PTR(&u8g2_clearBuffer_obj) },
	{ MP_ROM_QSTR(MP_QSTR_clearDisplay), MP_ROM_PTR(&u8g2_clearDisplay_obj) },
	{ MP_ROM_QSTR(MP_QSTR_drawBox), MP_ROM_PTR(&u8g2_drawBox_obj) },
	{ MP_ROM_QSTR(MP_QSTR_drawCircle), MP_ROM_PTR(&u8g2_drawCircle_obj) },
	{ MP_ROM_QSTR(MP_QSTR_drawDisc), MP_ROM_PTR(&u8g2_drawDisc_obj) },
	{ MP_ROM_QSTR(MP_QSTR_drawEllipse), MP_ROM_PTR(&u8g2_drawEllipse_obj) },
	{ MP_ROM_QSTR(MP_QSTR_drawFilledEllipse), MP_ROM_PTR(&u8g2_drawFilledEllipse_obj) },
	{ MP_ROM_QSTR(MP_QSTR_drawFrame), MP_ROM_PTR(&u8g2_drawFrame_obj) },
	{ MP_ROM_QSTR(MP_QSTR_drawHLine), MP_ROM_PTR(&u8g2_drawHLine_obj) },
	{ MP_ROM_QSTR(MP_QSTR_drawGlyph), MP_ROM_PTR(&u8g2_drawGlyph_obj) },
	{ MP_ROM_QSTR(MP_QSTR_drawLine), MP_ROM_PTR(&u8g2_drawLine_obj) },
	{ MP_ROM_QSTR(MP_QSTR_drawPixel), MP_ROM_PTR(&u8g2_drawPixel_obj) },
	{ MP_ROM_QSTR(MP_QSTR_drawRBox), MP_ROM_PTR(&u8g2_drawRBox_obj) },
	{ MP_ROM_QSTR(MP_QSTR_drawRFrame), MP_ROM_PTR(&u8g2_drawRFrame_obj) },
	{ MP_ROM_QSTR(MP_QSTR_drawStr), MP_ROM_PTR(&u8g2_drawStr_obj) },
	{ MP_ROM_QSTR(MP_QSTR_drawTriangle), MP_ROM_PTR(&u8g2_drawTriangle_obj) },
	{ MP_ROM_QSTR(MP_QSTR_drawUTF8), MP_ROM_PTR(&u8g2_drawUTF8_obj) },
	{ MP_ROM_QSTR(MP_QSTR_drawVLine), MP_ROM_PTR(&u8g2_drawVLine_obj) },
	{ MP_ROM_QSTR(MP_QSTR_drawXBM), MP_ROM_PTR(&u8g2_drawXBM_obj) },
	{ MP_ROM_QSTR(MP_QSTR_firstPage), MP_ROM_PTR(&u8g2_firstPage_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getAscent), MP_ROM_PTR(&u8g2_getAscent_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getDescent), MP_ROM_PTR(&u8g2_getDescent_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getStrWidth), MP_ROM_PTR(&u8g2_getStrWidth_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getUTF8Width), MP_ROM_PTR(&u8g2_getUTF8Width_obj) },
	{ MP_ROM_QSTR(MP_QSTR_nextPage), MP_ROM_PTR(&u8g2_nextPage_obj) },
	{ MP_ROM_QSTR(MP_QSTR_sendBuffer), MP_ROM_PTR(&u8g2_sendBuffer_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setAutoPageClear), MP_ROM_PTR(&u8g2_setAutoPageClear_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setBitmapMode), MP_ROM_PTR(&u8g2_setBitmapMode_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setDisplayRotation), MP_ROM_PTR(&u8g2_setDisplayRotation_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setDrawColor), MP_ROM_PTR(&u8g2_setDrawColor_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setFlipMode), MP_ROM_PTR(&u8g2_setFlipMode_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setFont), MP_ROM_PTR(&u8g2_setFont_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setFontDirection), MP_ROM_PTR(&u8g2_setFontDirection_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setFontMode), MP_ROM_PTR(&u8g2_setFontMode_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setFontPosBaseline), MP_ROM_PTR(&u8g2_setFontPosBaseline_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setFontPosBottom), MP_ROM_PTR(&u8g2_setFontPosBottom_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setFontPosTop), MP_ROM_PTR(&u8g2_setFontPosTop_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setFontPosCenter), MP_ROM_PTR(&u8g2_setFontPosCenter_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setFontRefHeightAll), MP_ROM_PTR(&u8g2_setFontRefHeightAll_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setFontRefHeightExtendedText), MP_ROM_PTR(&u8g2_setFontRefHeightExtendedText_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setFontRefHeightText), MP_ROM_PTR(&u8g2_setFontRefHeightText_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setPowerSave), MP_ROM_PTR(&u8g2_setPowerSave_obj) },

	{ MP_ROM_QSTR(MP_QSTR_U8G2_DRAW_UPPER_RIGHT), MP_ROM_INT(U8G2_DRAW_UPPER_RIGHT) },
	{ MP_ROM_QSTR(MP_QSTR_U8G2_DRAW_UPPER_LEFT), MP_ROM_INT(U8G2_DRAW_UPPER_LEFT) },
	{ MP_ROM_QSTR(MP_QSTR_U8G2_DRAW_LOWER_LEFT), MP_ROM_INT(U8G2_DRAW_LOWER_LEFT) },
	{ MP_ROM_QSTR(MP_QSTR_U8G2_DRAW_LOWER_RIGHT), MP_ROM_INT(U8G2_DRAW_LOWER_RIGHT) },
	{ MP_ROM_QSTR(MP_QSTR_U8G2_DRAW_ALL), MP_ROM_INT(U8G2_DRAW_ALL) },

	{ MP_ROM_QSTR(MP_QSTR_u8g2_font_profont12_mr), MP_ROM_PTR(&u8g2_font_profont12_mr_obj) },
	{ MP_ROM_QSTR(MP_QSTR_u8g2_font_profont22_mr), MP_ROM_PTR(&u8g2_font_profont22_mr_obj) },
};

STATIC MP_DEFINE_CONST_DICT(u8g2_module_globals, u8g2_module_globals_table);

const mp_obj_module_t u8g2_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&u8g2_module_globals,
};
