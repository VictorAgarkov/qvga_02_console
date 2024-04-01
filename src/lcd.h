#ifndef _lcd_h
#define _lcd_h

#ifdef __cplusplus
  extern "C"{
#endif

#include <stdint.h>


#define LCD_WIDTH 240
#define LCD_HEIGHT 320

#define RGB_16_to_24(a) (((a & 0xf800) << 8) | ((a & 0x07e0) << 5) | ((a & 0x001f) << 3))
#define RGB_24_to_16(a) (((a >> 8) & 0xf800) | ((a >> 5) & 0x07e0) | ((a >> 3) & 0x001f))

#define RGB_16(r, g, b) ((((r) & 0x00f8) << 8) | (((g) & 0x00fc) << 3) | (((b) & 0x00f8) >> 3))

#define LCD_RGB(r, g, b) (((((r) >> 3) & 0x1f) << 11) | ((((g) >> 2) & 0x3f) << 5) | ((((b) >> 3) & 0x1f) << 0))

//typedef  void (*ext_setpixel_t)(short, short, unsigned long);

//void setsetpixel(ext_setpixel_t);


void init_LCD(void);
void LCD_wr(uint16_t data, int type);
uint16_t LCD_rd(void);


int  LCD_setxy(short x, short y);
void LCD_setpixel_24(short x, short y, unsigned long c);
void LCD_setpixel(short x, short y, unsigned short c);
void LCD_put_vline_16(short x, short y, int num, unsigned short *src);
void LCD_put_vconst_16(short x, short y, int num, unsigned short constant);
void LCD_get_vline_16(short x, short y, int num, unsigned short *dst);


#ifdef __cplusplus
  }
#endif

#endif
