/*
 * THIS FOR DISPLAY GBK
 */
#include <string.h>
#include "main.h"
#include "lcd.h"

#define DISPLAY_Y_HIGHT 64
#define DISPLAY_X_WIDTH 128

#define DISPLAY_Y_WIDTH (DISPLAY_Y_HIGHT / 8)
#define FRAME_DATA_MAX_LEN  250

static unsigned char frame[FRAME_DATA_MAX_LEN];

static int x_idx, y_idx, scroll;

static void display_zero(void)
{
	x_idx = 0;
	y_idx = 0;
	scroll = 0;
}

static void display_clear_y(int y)
{
	int i;

	Page_set(y);
	Column_set(0);

	for (i = 0; i < DISPLAY_X_WIDTH; i++) {
		OLED_send_data(0);
	}
}

static void display_fill_raw_zero(int y)
{
	if (y >= DISPLAY_Y_WIDTH)
		return;

	y = y & (~0x01);

	display_clear_y(y);
	display_clear_y(y + 1);
}

static void display_buf_x_y(int x, int y, unsigned char *buf, int len)
{
	int i;

	Page_set(x);
	Column_set(y);

	for (i = 0; i < len; i++)
		OLED_send_data(buf[i]);
}

void display_one_char_fill(unsigned char *buf, int len)
{
	int fw = len >> 1;

	if ((len & 0x01) || (len == 0)) {
		/* bug, buf must be 2* */
		return ;
	}

	if ((len >> 1) + x_idx > DISPLAY_X_WIDTH) {
		x_idx = 0;
		y_idx = (y_idx + 2) % DISPLAY_Y_WIDTH;

		display_fill_raw_zero(y_idx);
	}

	display_buf_x_y(y_idx, x_idx, buf, fw);
	display_buf_x_y(y_idx + 1, x_idx, buf + fw, fw);

	x_idx += fw;
}

void display_cr_lf(void)
{
	x_idx = 0;
	y_idx = (y_idx + 2) % DISPLAY_Y_WIDTH;

	display_fill_raw_zero(y_idx);
}

static void handle_english_put(unsigned char *buf, unsigned char len)
{
	int i;
	for (i = 0; i < len; i++) {
		lcd_put_char(buf[i]);
	}
}

static void scroll_screen(int last_y)
{
	if (last_y != y_idx  && (y_idx << 3) == scroll) {
		scroll = (scroll + 16) % DISPLAY_Y_HIGHT;
		OLED_start_line_set(scroll);
	}
}

static void handle_hanzi_put(uint8_t *buf, uint8_t len, unsigned char type)
{

	int y = y_idx;

	if (type & 0xf0)
		display_cr_lf();

	display_one_char_fill(buf, len);

	scroll_screen(y);
}

static void lcd_to_default_setting(void)
{
	lcd_flush();
	display_zero();
}

static void packet_handle(unsigned char *buf, unsigned char len, unsigned char type)
{
	static unsigned char last_mode = PROTOCOL_TYPE_CLEAR;
	unsigned char current_mode = type & 0xf;

	if (last_mode != current_mode) {
		lcd_to_default_setting();
		last_mode = current_mode;
	}

	switch (type & 0x0f) {
	case PROTOCOL_TYPE_CLEAR:
		lcd_to_default_setting();
		break;
	case PROTOCOL_TYPE_ENG:
		handle_english_put(buf, len);
		break;
	case PROTOCOL_TYPE_HZ:
		handle_hanzi_put(buf, len, type);
		break;
	default:
		break;
	}
}

#define TIME_RECALCATE 1000
void handle_protocol_data(unsigned char c)
{
	static int idx;
	static int type;
	static int len;
	static unsigned int last_ms;
	unsigned int current;

	current = HAL_GetTick();
	if (current - last_ms > TIME_RECALCATE)
		idx = 0;
	last_ms = current;

	switch (idx) {
	case 0:
		if (c == 0xa5) {
			idx++;
		}
		break;
	case 1:
		if (c == 0x5a)
			idx++;
		else
			idx = 0;
		break;
	case 2:
		type = c;
		idx++;
		break;
	case 3:
		len = c;
		if (len > FRAME_DATA_MAX_LEN) {
			idx = 0;
			break;
		}
		if (len == 0) {
			idx = 0;
			goto data_handle;
			break;
		}
		idx++;
		break;
	default:
		frame[idx - 4] = c;
		if ((idx - 3) >= len) {
			idx = 0;
			goto data_handle;
		}
		idx++;
		break;
	}

	return;

data_handle:
	packet_handle(frame, len, type);
	return;
}
