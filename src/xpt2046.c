/*
	Библиотека для работы с контроллером touch-screen XPT2046 (ADS7846)
	основано на https://github.com/keirf/FlashFloppy/blob/master/src/xpt2046.c
*/
#include "stm32f10x_conf.h"
#include "gpio_mode.h"
#include "xpt2046.h"

#define abs(x) ((x) < 0 ? -(x) : (x))

#define XPT2046_WAIT_RX  while((XPT2046_SPI->SR & SPI_I2S_FLAG_RXNE) == 0)

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//
//  X = ((xpt2046_readX() * xpt2046_ax) >> 16) + xpt2046_bx
//
short xpt2046_ax = 4335,  xpt2046_ay = 5871;  // множители в формате с фиксированной точкой 0.16
short xpt2046_bx = -16,   xpt2046_by = -13;  // смещение

int   xpt2046_threshold = 5;
/*
	4361 -16 5847 -13
	4335 -16 5862 -14
	4332 -16 5892 -13
	4335 -17 5900 -13
	4312 -16 5855 -12
	------------------
	4335 -16 5871 -13
*/


//----------------------------------------------------------------------------------------
void xpt2046_init(void)
{
	// SPI
	//XPT2046_SPI->CR1 = SPI_CR1_MSTR | SPI_CR1_BR_2 | SPI_CR1_SSI | SPI_CR1_SSM | SPI_CR1_DFF | SPI_CR1_SPE; //16 bit
	XPT2046_SPI->CR1 = SPI_CR1_MSTR | SPI_CR1_BR_2 | SPI_CR1_SSI | SPI_CR1_SSM | SPI_CR1_SPE; //8 bit

	// SPI - pins
	XPT2046_CS1;
	gpio_set_mode(XPT2046_CS_PORT, XPT2046_CS_PIN,   GPIO_Mode__OUT_PP | GPIO_Speed_10MHz);  // CS
	gpio_set_mode(XPT2046_SCK_PORT,  XPT2046_SCK_PIN,  GPIO_Mode_AF_PP | GPIO_Speed_10MHz);   // SCK
	gpio_set_mode(XPT2046_MOSI_PORT, XPT2046_MOSI_PIN, GPIO_Mode_AF_PP | GPIO_Speed_10MHz);   // MOSI
}
//----------------------------------------------------------------------------------------
/*
unsigned short xpt2046_read_reg(unsigned short addr)
{
	XPT2046_SPI->DR = addr;
	while((XPT2046_SPI->SR & SPI_I2S_FLAG_RXNE) == 0);
	return XPT2046_SPI->DR;
}
//----------------------------------------------------------------------------------------
unsigned short xpt2046_readX(void)
{
	unsigned short msb, lsb, ret;
	XPT2046_CS0;

	msb = xpt2046_read_reg(0xD400);
	lsb = xpt2046_read_reg(0x0000);

	ret = ((msb << 4) & 0x0FF0) | ((lsb >> 12) & 0x000F);

	XPT2046_CS1;
	return ret;
}
//----------------------------------------------------------------------------------------
unsigned short xpt2046_readY(void)
{
	unsigned short msb, lsb, ret;
	XPT2046_CS0;

	msb = xpt2046_read_reg(0x9400);
	lsb = xpt2046_read_reg(0x0000);

	ret = ((msb << 4) & 0x0FF0) | ((lsb >> 12) & 0x000F);

	XPT2046_CS1;
	return ret;
}

//----------------------------------------------------------------------------------------
unsigned long xpt2046_readXY(void)
{
	unsigned long x = xpt2046_readX();
	return (x << 16) | xpt2046_readY();
}
//----------------------------------------------------------------------------------------
void xpt2046_readXY_p(struct point *P)
{
	P->x = xpt2046_readX();
	P->y = xpt2046_readY();
}
*/
//----------------------------------------------------------------------------------------
/*
unsigned short xpt2046_read16(unsigned char addr)
{
	unsigned short ret;
	XPT2046_SPI->DR = addr;
	XPT2046_WAIT_RX;
	ret = XPT2046_SPI->DR;
	XPT2046_SPI->DR = 0;
	XPT2046_WAIT_RX;

	return (ret << 8) | (XPT2046_SPI->DR & 0xff);
}
*/
//----------------------------------------------------------------------------------------
unsigned char xpt2046_read8(unsigned char addr)
{
	XPT2046_SPI->DR = addr;
	XPT2046_WAIT_RX;
	return XPT2046_SPI->DR;
}
//----------------------------------------------------------------------------------------
void xpt2046_readXY_n(int n, short *xx, short *yy)
{
	unsigned short x,y;
	volatile int d;

	XPT2046_CS0;

	xpt2046_read8(0x94);
	while(n--)
	{
		y = (unsigned short)xpt2046_read8(0) << 8;
		y |= xpt2046_read8(0xd4);

		x = (unsigned short)xpt2046_read8(0) << 8;
		x |= xpt2046_read8(n ? 0x94 : 0);
		*xx++ = (x >> 3) & 0xfff;
		*yy++ = (y >> 3) & 0xfff;
		//for(d = 0; d < 50; d++);
	}

	XPT2046_CS1;
}
//----------------------------------------------------------------------------------------
/*
void xpt2046_readZ12(struct point *P)
{
	unsigned short us;
	XPT2046_CS0;

	xpt2046_read8(0xb4);  // start Z1 measure
	us = (unsigned short)xpt2046_read8(0) << 8;
	us |= xpt2046_read8(0xc4);  // start Z2 measure
	P->x = (us >> 3) & 0xfff;
	us = (unsigned short)xpt2046_read8(0) << 8;
	us |= xpt2046_read8(0);
	P->y = (us >> 3) & 0xfff;

	XPT2046_CS1;
}
*/
//----------------------------------------------------------------------------------------
int xpt2046_int_active(void)
{
	return (XPT2046_INT_PORT->IDR & (1 << XPT2046_INT_PIN)) == 0;
}
//----------------------------------------------------------------------------------------
short xpt2046_scaleX(unsigned short rddX)
{
	return ((rddX * xpt2046_ax) >> 16) + xpt2046_bx;
}
//----------------------------------------------------------------------------------------
short xpt2046_scaleY(unsigned short rddY)
{
	return ((rddY * xpt2046_ay) >> 16) + xpt2046_by;
}
//----------------------------------------------------------------------------------------
int XPT2046_acquireXY(struct point *P)
{
	// return:
	// 0 - not touched
	// 1 - touched, but values unreliable
	// 2 - normal values

	short xx[8], yy[8];
	int i, j;
	int ret;

	// 8 time reading touch panel coordinates
	if(!xpt2046_int_active()) return 0;

	xpt2046_readXY_n(8, xx, yy);

	if(!xpt2046_int_active()) return 0;

	// sorting received coordinates, exlude first
	for(i = 1; i < 7; i++)
	{
		for(j = i + 1; j < 8; j++)
		{
			short t = xx[i];
			if(t > xx[j]){xx[i] = xx[j]; xx[j] = t;}
			t = yy[i];
			if(t > yy[j]){yy[i] = yy[j]; yy[j] = t;}
		}
	}

	// check range of middle 3 values. if it too big - values wrong
	if (((xx[5]-xx[3]) > xpt2046_threshold) || ((yy[5]-yy[3]) > xpt2046_threshold))
	{
		return 1;
	}

	P->x = xx[4];  //xpt2046_scaleX(xpt2046_readX());
	P->y = yy[4];  //xpt2046_scaleY(xpt2046_readY());

	return 2; //ret;
}
//----------------------------------------------------------------------------------------
void XPT2046_scale_p(struct point *P)
{
	P->x = xpt2046_scaleX(P->x);
	P->y = xpt2046_scaleY(P->y);
}
//----------------------------------------------------------------------------------------
int XPT2046_set_scale(union rect *Rs, union rect *Rt)
{
	// вычисляем масштабирующие коэфф. для тач-панели
	// Rs (screen) - координаты точек на экране
	// Rt (touch)  - координаты соотв. точек на тач-панели
	// return:
	// 0 - normal
	// 1 - dx или dy координат тач-панели равно 0.
	long l1, l2;
	short dx, dy;
	dx = Rt->x1 - Rt->x2;
	dy = Rt->y1 - Rt->y2;
	if(dx == 0 || dy == 0) return 1;

	l1 = Rs->x1;
	l2 = Rs->x2;
	xpt2046_ax = ((l1 - l2) << 16) / dx;
	xpt2046_bx = 0;
	xpt2046_bx = Rs->x1 - xpt2046_scaleX(Rt->x1);

	l1 = Rs->y1;
	l2 = Rs->y2;
	xpt2046_ay = ((l1 - l2) << 16) / dy;
	xpt2046_by = 0;
	xpt2046_by = Rs->y1 - xpt2046_scaleY(Rt->y1);

	return 0;
}
//----------------------------------------------------------------------------------------
