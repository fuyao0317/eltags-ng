#include "stdio.h"
#include "main.h"
#include "i2c.h"
#include "font.h"


#define IIC_SCK_PIN 5
#define IIC_SDA_PIN 6

#define OLED_COLUMN_NUMBER 128
#define OLED_LINE_NUMBER   64
#define OLED_COLUMN_OFFSET 2
#define OLED_PAGE_NUMBER   (OLED_LINE_NUMBER/8)

const unsigned char  *point;
static unsigned char cstart_line;
unsigned char ACK=0;
const unsigned char  OLED_init_cmd[25]=			//SH1106
{
  
  /*0xae,0X00,0X10,0x40,0X81,0XCF,0xff,0xa1,0xa4,
  0xA6,0xc8,0xa8,0x3F,0xd5,0x80,0xd3,0x00,0XDA,0X12,
  0x8d,0x14,0xdb,0x40,0X20,0X02,0xd9,0xf1,0xAF*/
       0xAE,//¹Ø±ÕÏÔÊ¾
	
       0xD5,//ÉèÖÃÊ±ÖÓ·ÖÆµÒò×Ó,Õðµ´ÆµÂÊ
       0x80,  //[3:0],·ÖÆµÒò×Ó;[7:4],Õðµ´ÆµÂÊ

       0xA8,//ÉèÖÃÇý¶¯Â·Êý
       0X3F,//Ä¬ÈÏ(1/64)
	
       0xD3,//ÉèÖÃÏÔÊ¾Æ«ÒÆ
       0X00,//Ä¬ÈÏÎª0
	
       0x40,//ÉèÖÃÏÔÊ¾¿ªÊ¼ÐÐ [5:0],ÐÐÊý.
	
       0x8D,//µçºÉ±ÃÉèÖÃ
       0x14,//bit2£¬¿ªÆô/¹Ø±Õ
       0x20,//ÉèÖÃÄÚ´æµØÖ·Ä£Ê½
       0x02,//[1:0],00£¬ÁÐµØÖ·Ä£Ê½;01£¬ÐÐµØÖ·Ä£Ê½;10,Ò³µØÖ·Ä£Ê½;Ä¬ÈÏ10;
       0xA1,//¶ÎÖØ¶¨ÒåÉèÖÃ,bit0:0,0->0;1,0->127;  A1
	
       0xC8,//ÉèÖÃCOMÉ¨Ãè·½Ïò;bit3:0,ÆÕÍ¨Ä£Ê½;1,ÖØ¶¨ÒåÄ£Ê½ COM[N-1]->COM0;N:Çý¶¯Â·Êý (C0 ·­×ªÏÔÊ¾) C8
	   
       0xDA,//ÉèÖÃCOMÓ²¼þÒý½ÅÅäÖÃ
       0x12,//[5:4]ÅäÖÃ  
	   
       0x81,//¶Ô±È¶ÈÉèÖÃ
       0x66,//1~255;Ä¬ÈÏ0X7F (ÁÁ¶ÈÉèÖÃ,Ô½´óÔ½ÁÁ)
	   
       0xD9,//ÉèÖÃÔ¤³äµçÖÜÆÚ
       0xf1,//[3:0],PHASE 1;[7:4],PHASE 2;
	   
       0xDB,//ÉèÖÃVCOMH µçÑ¹±¶ÂÊ
       0x30,//[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
	   
       0xA4,//È«¾ÖÏÔÊ¾¿ªÆô;bit0:1,¿ªÆô;0,¹Ø±Õ;(°×ÆÁ/ºÚÆÁ)
	   
       0xA6,//ÉèÖÃÏÔÊ¾·½Ê½;bit0:1,·´ÏàÏÔÊ¾;0,Õý³£ÏÔÊ¾ 
       
       0xAF,//¿ªÆôÏÔÊ¾     
};

const unsigned char  picture_tab[]={
/*------------------------------------------------------------------------------
;  Ô´ÎÄ¼þ / ÎÄ×Ö : C:\Documents and Settings\Administrator\×ÀÃæ\ÐÂ½¨ÎÄ¼þ¼Ð (2)\logo.bmp×ÖÄ£
;  ¿í¡Á¸ß£¨ÏñËØ£©: 128¡Á64
------------------------------------------------------------------------------*/
//0x80,0x40,0x10,//¿íµÄÏñËØÊý,¸ßµÄÏñËØÊý£¬¿íµÄ×Ö½ÚÊý£¬²ÎÊýÉèÖÃ¿ÉÑ¡
0xFF,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xFF,
0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x20,0x44,0x08,0x00,0x18,0x48,0x28,0xC8,0x08,0x28,0x48,0x18,0x00,
0x40,0x40,0xFC,0x40,0x40,0xFC,0x00,0x00,0xF8,0x00,0x00,0xFC,0x00,0x40,0x40,0xA0,
0x90,0x88,0x84,0x88,0x90,0x20,0x40,0x40,0x00,0x00,0x40,0x44,0xD8,0x20,0xF0,0xAC,
0xA8,0xE8,0xB8,0xA8,0xE0,0x00,0x00,0x00,0xC0,0x7C,0x54,0x54,0x54,0x54,0x54,0x54,
0x7C,0x40,0x40,0x00,0x00,0xF0,0x90,0x90,0x90,0xFC,0x90,0x90,0x90,0xF0,0x00,0x00,
0x00,0x80,0x88,0x88,0x88,0x88,0x88,0xE8,0xA8,0x98,0x8C,0x88,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x04,0x3E,0x01,0x10,0x11,0x09,0x05,0x3F,0x05,0x09,0x11,0x11,0x00,
0x08,0x18,0x0F,0x24,0x14,0x0F,0x00,0x00,0x0F,0x00,0x00,0x3F,0x00,0x20,0x22,0x2A,
0x32,0x22,0x3F,0x22,0x32,0x2A,0x22,0x20,0x00,0x00,0x20,0x10,0x0F,0x10,0x28,0x24,
0x23,0x20,0x2F,0x28,0x2A,0x2C,0x00,0x30,0x0F,0x04,0x3D,0x25,0x15,0x15,0x0D,0x15,
0x2D,0x24,0x24,0x00,0x00,0x07,0x04,0x04,0x04,0x1F,0x24,0x24,0x24,0x27,0x20,0x38,
0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
0xFF,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xFF,
0xFF,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x81,0x41,0x21,0x21,0x61,0x01,0x01,0x21,0xE1,0xE1,0x01,0xE1,0xE1,
0x21,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x21,0xE1,0x21,0x21,0x21,0x61,0x01,0x01,
0x21,0x21,0xE1,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0xC1,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x21,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xFF,
0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x0F,0x10,0x20,0x24,0x1C,0x04,0x00,0x20,0x3F,0x01,0x3E,0x01,0x3F,
0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x3F,0x22,0x22,0x27,0x30,0x00,0x00,
0x20,0x20,0x3F,0x20,0x20,0x00,0x00,0x1E,0x25,0x25,0x25,0x16,0x00,0x00,0x1E,0x21,
0x21,0x21,0x13,0x00,0x01,0x01,0x1F,0x21,0x21,0x00,0x00,0x00,0x21,0x3F,0x22,0x21,
0x01,0x00,0x00,0x1E,0x21,0x21,0x21,0x1E,0x00,0x21,0x3F,0x22,0x01,0x01,0x3E,0x20,
0x00,0x21,0x21,0x3F,0x20,0x20,0x00,0x00,0x1E,0x21,0x21,0x21,0x13,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xF0,0x08,0x04,0x04,0x04,0x0C,0x00,0xF0,0x08,0x04,0x04,0x08,0xF0,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0xFC,0x04,0x00,
0x00,0x00,0x00,0x00,0x0C,0x04,0xFC,0x04,0x0C,0x00,0x04,0xFC,0x04,0x04,0x08,0xF0,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
0xFF,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x81,0x82,0x84,0x84,0x84,0x82,0x80,0x81,0x82,0x84,0x84,0x82,0x81,
0x80,0x80,0x86,0x86,0x80,0x80,0x80,0x80,0x80,0x85,0x83,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x84,0x87,0x84,0x84,
0x84,0x86,0x80,0x80,0x80,0x84,0x87,0x84,0x80,0x80,0x84,0x87,0x84,0x84,0x82,0x81,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xFF
};


void OLED_send_cmd(unsigned char o_command)
  {
    
	  unsigned char buf[2];
	  buf[0] = 0x00;
	  buf[1] = o_command;

    /*
     * IIC_write(0x00); 
     * IIC_write(o_command);
     */


    HAL_I2C_Master_Transmit(&hi2c1, 0x78, buf, 2, 200);
    
  }
void OLED_send_data(unsigned char o_data)
  { 
	  unsigned char buf[2];
	  buf[0] = 0x40;
	  buf[1] = o_data;
    /*
     * IIC_write(0x40);
     * IIC_write(o_data);
     */
    HAL_I2C_Master_Transmit(&hi2c1, 0x78, buf, 2, 200);
   }
void Column_set(unsigned char column)
  {
	column+=OLED_COLUMN_OFFSET;
    OLED_send_cmd(0x10|(column>>4));    //ÉèÖÃÁÐµØÖ·¸ßÎ»
    OLED_send_cmd(0x00|(column&0x0f));   //ÉèÖÃÁÐµØÖ·µÍÎ»  
    	 
  }

void OLED_start_line_set(unsigned char line)
{
	OLED_send_cmd(0x40 | (line & 0x3f)); //ÉèÖÃÁÐµØÖ·¸ßÎ»
	cstart_line = line;
}

void Page_set(unsigned char page)
  {
    OLED_send_cmd(0xb0+page);
  }
void OLED_clear(void)
  {
    unsigned char page,column;
    for(page=0;page<OLED_PAGE_NUMBER;page++)             //page loop
      { 
          Page_set(page);
          Column_set(0);	  
          for(column=0;column<OLED_COLUMN_NUMBER;column++)	//column loop
            {
              OLED_send_data(0x00);
            }
      }
  }
void OLED_full(void)
  {
    unsigned char page,column;
    for(page=0;page<OLED_PAGE_NUMBER;page++)             //page loop
      { 
        Page_set(page);
        Column_set(0);	  
	for(column=0;column<OLED_COLUMN_NUMBER;column++)	//column loop
          {
            OLED_send_data(0xff);
          }
      }
  }
void OLED_init(void)
  {
    unsigned char i;
    for(i=0;i<25;i++)
      {
        OLED_send_cmd(OLED_init_cmd[i]);
      }
  }

void Picture_display(void)
  {
    unsigned char page,column;
	unsigned char const *ptr_pic = picture_tab;
    for(page=0;page<(OLED_LINE_NUMBER/8);page++)        //page loop
      { 
	Page_set(page);
	Column_set(0);	  
	for(column=0;column<OLED_COLUMN_NUMBER;column++)	//column loop
          {
            OLED_send_data(*ptr_pic++);
          }
      }
  }
void Picture_ReverseDisplay(void)
{
	unsigned char const *ptr_pic = picture_tab;
    unsigned char page,column,data;
    for(page=0;page<(OLED_LINE_NUMBER/8);page++)        //page loop
      { 
	Page_set(page);
	Column_set(0);	  
	for(column=0;column<OLED_COLUMN_NUMBER;column++)	//column loop
          {
            data=*ptr_pic++;
            data=~data;
            OLED_send_data(data);
          }
      }
  }


static int current_pos_x;
static int current_pos_y;
static unsigned char cstart_line = 0;


	/*
	 * Page_set(page);
	 * Column_set(0);	  
	 */
void lcd_put_char(unsigned char s)
{
	const unsigned char *p;
	int i = 0;
	int start_page;
	int next_line_flag = 0;

	if (current_pos_x >= OLED_COLUMN_NUMBER / 6 || s == '\n' || s == '\r') {
		current_pos_x = 0;
		current_pos_y++;
		next_line_flag = 1;
	}

	if (current_pos_y >= OLED_LINE_NUMBER / 8) {
		current_pos_y = 0;
		current_pos_x = 0;
		next_line_flag = 1;
	}

	if (current_pos_x == 0) {
		Page_set(current_pos_y);
		Column_set(0);

		/* clear this page */
		for (i = 0; i < OLED_COLUMN_NUMBER; i++) {
			OLED_send_data(0);
		}
		Column_set(0);
	}

	if (next_line_flag == 1) {
		start_page = cstart_line / 8;
		if (start_page == current_pos_y) {
			cstart_line = (cstart_line + 8) % OLED_LINE_NUMBER;
			OLED_start_line_set(cstart_line);
		}
	}

	if (s == '\n' || s == '\r') {
		return;
	}


	p = get_font_buf(s);

	for (i = 0; i < 5; i++)
		OLED_send_data(p[i]);

	OLED_send_data(0);

	current_pos_x++;
}

void lcd_flush(void)
{
	current_pos_x = 0;
	current_pos_y = 0;
	cstart_line = 0;
	OLED_clear();
	Page_set(0);
	Column_set(0);
	OLED_start_line_set(0);
}

void lcd_put_string(char *s)
{
	if (s == NULL || *s == 0) {
		return;
	}

	do {
		lcd_put_char(*s);
	} while (*s++ != 0);
}

void lcd_put_size(char *s, int len)
{
	int i = 0;

	for (i = 0 ; i < len; i++)
		lcd_put_char(*s++);
}

/*
 * int _main()
 * { 
 *   point= &picture_tab[0];
 *   IO_init();
 * 
 *   OLED_init();
 *   OLED_full();
 *   OLED_clear();
 *   while(1)
 *   {
 *         
 *     Picture_display(point);
 *     
 *     Picture_ReverseDisplay(point);
 *     
 *   }
 * }
 */
