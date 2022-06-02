/*
 * THIS FOR DISPLAY GBK
 */
#include <string.h>
#include "main.h"
#include "lcd.h"

#define DISPLAY_X_WIDTH 128
#define DISPLAY_Y_WIDTH (64 / 8)
#define FRAME_DATA_MAX_LEN  256

static unsigned char display[DISPLAY_Y_WIDTH][DISPLAY_X_WIDTH];

static int x_idx, y_idx, scroll;

static void display_zero(void)
{
	memset((void *)display, 0, sizeof(display));
	x_idx = 0;
	y_idx = 0;
	scroll = 0;
}

static void display_fill_raw_zero(int y)
{
	int i = 0;

	if (y >= DISPLAY_Y_WIDTH)
		return;

	y = y & (~0x01);

	for (i = 0; i < DISPLAY_X_WIDTH; i++) {
		display[y][i] = 0;
		display[y + 1][i] = 0;
	}
}

void display_one_char_fill(unsigned char *buf, int len)
{
	int i;

	if (len & 0x01) {
		/* bug, buf must be 2* */
		return ;
	}

	if ((len >> 1) + x_idx >= DISPLAY_X_WIDTH) {
		x_idx = 0;
		y_idx = (y_idx + 2) % DISPLAY_Y_WIDTH;

		display_fill_raw_zero(y_idx);
	}

	for (i = 0; i < len; i++) {
		if (len & 0x01) {
			display[y_idx + 1][x_idx++] = buf[len];
		} else {
			display[y_idx][x_idx] = buf[len];
		}
	}
}

void display_cr_lf(void)
{
	x_idx = 0;
	y_idx = (y_idx + 2) % DISPLAY_Y_WIDTH;

	display_fill_raw_zero(y_idx);
}

void handle_char_eng_buf(unsigned char *Buf, int Len)
{
	int i;

	for (i = 0; i < Len; i++) {
		unsigned char *next_buf_rec;

		if (pusb_rec >= usb_buf_end)
			next_buf_rec = usb_buf_start;
		else
			next_buf_rec = pusb_rec + 1;

		if (next_buf_rec == pusb_handle) {
			/* over flow skip it*/
			return;
		}

		*pusb_rec = Buf[i];
		pusb_rec = next_buf_rec;
	}
}

void handle_zero_data(unsigned int type)
{
	switch (type) {
	case PROTOCOL_TYPE_CLEAR:
		lcd_flush();
		display_zero();
		break;
	}
}

static unsigned char frame_buf[FRAME_DATA_MAX_LEN];

void handle_protocol_data(unsigned char c)
{
	static int idx;
	static int type;
	static int len;

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
		if (c == 0) {
			idx = 0;
			handle_zero_data(type);
			break;
		}
		idx++;
		break;
	default:
		/*
		 * if ((idx - 3) < len)
		 *         else
		 * idx++;
		 */
		break;
	}
}







