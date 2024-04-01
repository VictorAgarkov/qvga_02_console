#include <stm32f10x.h>    // Device header
#include "lcd.h"

/*
#define GPIO_IN 4
#define GPIO_OUT 1

#define LCD_CTRL_PORT
#define LCD_PIN_RS 8
#define LCD_PIN_CS 9
#define LCD_PIN_WR 10
#define LCD_PIN_RD 11
*/

// LCD work without FSMC
#define LCD_CTL_TO_OUT  GPIOC->CRH = (GPIOC->CRH & 0xffff0000) | 0x00001111
#define LCD_DATA_TO_OUT GPIOB->CRH = GPIOC->CRL = 0x11111111
#define LCD_DATA_TO_IN  GPIOB->CRH = GPIOC->CRL = 0x44444444

#define LCD_CSWR_ON     GPIOC->BSRR = 0x06000000
#define LCD_CSRD_ON     GPIOC->BSRR = 0x0a000000
#define LCD_CSRDWR_OFF  GPIOC->BSRR = 0x00000e00
#define LCD_RS_ON_v     0x00000100
#define LCD_RS_OFF_v    0x01000000
#define LCD_DATA_OUT(a) {GPIOC->ODR = (GPIOC->ODR & 0xffffff00) | ((a) & 0x00ff); GPIOB->ODR = (GPIOB->ODR & 0xffff00ff) | ((a) & 0xff00);}
#define LCD_DATA_IN  ((GPIOC->IDR & 0x000000ff) | (GPIOB->IDR & 0x0000ff00))

#define LCD_IS_ADDR 0
#define LCD_IS_DATA 1

#define LCD_WR_ADDR(addr) LCD_wr(addr, LCD_IS_ADDR)
#define LCD_WR_DATA(data) LCD_wr(data, LCD_IS_DATA)

#define INIT_SEQ seq_SSD1289


typedef struct InitSeq
{
	uint16_t addr, val;
} t_InitSeq;

//-------------------------------------------------------------------------

uint16_t g_LCD_id;
const t_InitSeq seq_SSD1289[] =  // SSD1289
{
	{0x0000,0x0001},
	{0x0003,0xA8A4},
	{0x000C,0x0000},
	{0x000D,0x080C},
	{0x000E,0x2B00},
	{0x001E,0x00B0},
	{0x0001,0x2b3F},
	{0x0002,0x0600},
	{0x0010,0x0000},
	{0x0011,0x6078},
	{0x0005,0x0000},
	{0x0006,0x0000},
	{0x0016,0xEF1C},
	{0x0017,0x0003},
	{0x0007,0x0233},
	{0x000B,0x0000},
	{0x000F,0x0000},
	{0x0041,0x0000},
	{0x0042,0x0000},
	{0x0048,0x0000},
	{0x0049,0x013F},
	{0x004A,0x0000},
	{0x004B,0x0000},
	{0x0044,0xEF00},
	{0x0045,0x0000},
	{0x0046,0x013F},
	{0x0030,0x0707},
	{0x0031,0x0204},
	{0x0032,0x0204},
	{0x0033,0x0502},
	{0x0034,0x0507},
	{0x0035,0x0204},
	{0x0036,0x0204},
	{0x0037,0x0502},
	{0x003A,0x0302},
	{0x003B,0x0302},
	{0x0023,0x0000},
	{0x0024,0x0000},
	{0x0025,0x8000},
	{0x004f,0x0000},
	{0x004e,0x0000}
};

void LCD_wr(uint16_t data, int type)
{
	//volatile int i;

	GPIOC->BSRR = (type) ? LCD_RS_ON_v : LCD_RS_OFF_v;
	LCD_CSWR_ON;
	LCD_DATA_OUT(data);
	LCD_DATA_TO_OUT;
	//for(i = 5; i; i--);
	LCD_CSRDWR_OFF;
	//for(i = 1; i; i--);
	LCD_DATA_TO_IN;

}
//-------------------------------------------------------------------------
void LCD_put_array(int num, unsigned short *data)
{
	int i;
	LCD_DATA_TO_OUT;
	for(i = 0; i < num; i++)
	{
		LCD_CSWR_ON;
		LCD_DATA_OUT(data[i]);
		//for(i = 5; i; i--);
		LCD_CSRDWR_OFF;
		//for(i = 1; i; i--);
	}
	LCD_DATA_TO_IN;
}
//-------------------------------------------------------------------------
void LCD_get_array(int num, unsigned short *data)
{
	int i;

	LCD_DATA_TO_IN;
	for(i = 0; i < num; i++)
	{
		LCD_CSRD_ON;
		data[i] = LCD_DATA_IN;
		//for(i = 5; i; i--);
		LCD_CSRDWR_OFF;
		//for(i = 1; i; i--);
	}

}
//-------------------------------------------------------------------------
uint16_t LCD_rd(void)
{
	int i;
	uint16_t ret;

	GPIOC->BSRR = LCD_RS_ON_v;
	LCD_DATA_TO_IN;
	LCD_CSRD_ON;
	for(i = 3; i; i--);
	ret = LCD_DATA_IN;
	LCD_CSRDWR_OFF;
	for(i = 2; i; i--);
	return ret;

}
//-------------------------------------------------------------------------
void delay_(int v)
{
	// ~83nS/1 @8Mhz (???)
	while(--v);
}
//-------------------------------------------------------------------------
void LCD_wr_reg(unsigned short addr, unsigned short data)
{
	LCD_WR_ADDR(addr);
	LCD_WR_DATA(data);
}
//-------------------------------------------------------------------------

void init_LCD(void)
{
	int i;
/*
	int color_strip[] =
	{
		LCD_RGB( 0,  0,  0),
		LCD_RGB( 0,  0, -1),
		LCD_RGB(-1,  0,  0),
		LCD_RGB(-1,  0, -1),
		LCD_RGB( 0, -1,  0),
		LCD_RGB( 0, -1, -1),
		LCD_RGB(-1, -1,  0),
		LCD_RGB(-1, -1, -1)
	};
*/
	LCD_CSRDWR_OFF;
	LCD_CTL_TO_OUT;

	delay_(610000); //50mS

	LCD_WR_ADDR(0);
	g_LCD_id = LCD_rd();

	for(i = 0; i < sizeof(INIT_SEQ) / sizeof(INIT_SEQ[0]); i++)
	{
		LCD_WR_ADDR(INIT_SEQ[i].addr);
		LCD_WR_DATA(INIT_SEQ[i].val);
		delay_(61000); //5mS
	}
	/*
	LCD_WR_ADDR(0x22);
	for(i = 0; i < 320*240; i++)
	{
		//LCD_WR_DATA(1 << ((i /20) & 0x000f));
		LCD_WR_DATA(color_strip[(i / (320 * 30)) & 7]);
		//LCD_WR_DATA(i % 320 < 50 ? 0 : 63 << 5);
	}
	*/
}
//-------------------------------------------------------------------------


int LCD_setxy(short x, short y)
{
	if(x < 0 || x >= LCD_WIDTH) return 1;
	if(y < 0 || y >= LCD_HEIGHT) return 2;
	LCD_wr_reg(0x4e, x);
	LCD_wr_reg(0x4f, y);
	return 0;
}
//-------------------------------------------------------------------------
void LCD_setpixel_24(short x, short y, unsigned long c)
{

	LCD_setpixel(x, y, RGB_24_to_16(c));
}
//-------------------------------------------------------------------------
void LCD_setpixel(short x, short y, unsigned short c)
{
	if(LCD_setxy(x, y)) return;

	LCD_WR_ADDR(0x22);

	GPIOC->BSRR = LCD_RS_ON_v;

	LCD_DATA_TO_OUT;
	LCD_CSWR_ON;
	LCD_DATA_OUT(c);
	LCD_CSRDWR_OFF;
	LCD_DATA_TO_IN;

}
//-------------------------------------------------------------------------
void LCD_put_vline_16(short x, short y, int num, unsigned short *src)
{
	if(y < 0)
	{
		num += y;
		src -= y;
		y = 0;
	}

	if(num <= 0) return;

	if(LCD_setxy(x, y)) return;
	LCD_WR_ADDR(0x22);

	GPIOC->BSRR = LCD_RS_ON_v;
	if(num + y >= LCD_HEIGHT) num = LCD_HEIGHT - y;
	LCD_put_array(num, src);
}
//-------------------------------------------------------------------------
void LCD_put_vconst_16(short x, short y, int num, unsigned short constant)
{
	int i;
	if(y < 0)
	{
		num += y;
		y = 0;
	}

	if(num <= 0) return;

	if(LCD_setxy(x, y)) return;

	LCD_WR_ADDR(0x22);

	GPIOC->BSRR = LCD_RS_ON_v;
	if(num + y >= LCD_HEIGHT) num = LCD_HEIGHT - y;

	LCD_DATA_OUT(constant);
	LCD_DATA_TO_OUT;
	for(i = 0; i < num; i++)
	{
		LCD_CSWR_ON;
		//for(i = 5; i; i--);
		LCD_CSRDWR_OFF;
		//for(i = 1; i; i--);
	}
	LCD_DATA_TO_IN;
}
//-------------------------------------------------------------------------
void LCD_get_vline_16(short x, short y, int num, unsigned short *dst)
{
	if(LCD_setxy(x, y)) return;

	LCD_WR_ADDR(0x22);

	GPIOC->BSRR = LCD_RS_ON_v;
	if(num + y >= LCD_HEIGHT) num = LCD_HEIGHT - y;
	LCD_get_array(num, dst);
}
//-------------------------------------------------------------------------
