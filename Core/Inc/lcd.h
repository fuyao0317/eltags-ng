#ifndef __LCD_H
#define __LCD_H


void OLED_init(void);
void OLED_full(void);
void OLED_clear(void);
void Picture_display(void);
void Picture_ReverseDisplay(void);

extern const unsigned char  *picture_tab;
void lcd_put_char(unsigned char s);
void lcd_flush(void);
void lcd_put_string(char *s);
void lcd_put_size(char *s, int len);
void Page_set(unsigned char page);
void Column_set(unsigned char column);
void OLED_send_data(unsigned char o_data);
void OLED_start_line_set(unsigned char line);

#endif /* __LCD_H */
