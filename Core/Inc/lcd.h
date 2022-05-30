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

#endif /* __LCD_H */
