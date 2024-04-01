/*
**
**                           Main.c
**
**
**********************************************************************/
/*
   Last committed:     $Revision: 00 $
   Last changed by:    $Author: $
   Last changed date:  $Date:  $
   ID:                 $Id:  $

**********************************************************************/
#include "stm32f10x_conf.h"
#include <stdlib.h>
#include <stdio.h>
#include "lcd.h"
#include "cdisp.h"
#include "gpio_mode.h"
#include "xpt2046.h"

#define min(a, b) ((a < b) ? a : b)
#define max(a, b) ((a > b) ? a : b)


volatile char usart_rdd, usart_received = 0;
volatile  unsigned long systick_ms = 0, tmr1 = 0;

void do_test(void);
//----------------------------------------------------------------------------------------
void USART1_IRQHandler(void)
{
	//USART1->SR;
	usart_rdd = USART1->DR;
	usart_received = 1;
}
//----------------------------------------------------------------------------------------
void SysTick_Handler(void)
{
  ++systick_ms;
	++tmr1;
	//sys_tick_flag = 1;
}
//----------------------------------------------------------------------------------------
void init_usart1(int baud)
{
	USART1->CR1 = USART_CR1_UE | USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE;
	USART1->BRR = (SystemCoreClock + baud / 2) / baud;
	gpio_set_mode(GPIOA, 9, GPIO_Mode__AF_PP | GPIO_Speed__2MHz);
	NVIC_EnableIRQ(USART1_IRQn);
}
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void erase_touch_target(struct point *P)
{
	union rect r;

	current_color.l = 0;

	r.x1 = P->x - 12;
	r.x2 = P->x + 12;
	r.y1 = P->y - 12;
	r.y2 = P->y + 12;

	cdisp_rect_fill_r(&r);

}
//----------------------------------------------------------------------------------------
void draw_touch_target(struct point *P)
{
	union rect r;

	current_color.l = 0xff0040;

	r.x1 = P->x - 1;
	r.x2 = P->x + 1;
	r.y1 = P->y - 12;
	r.y2 = P->y + 12;

	cdisp_rect_fill_r(&r);

	r.x1 = P->x - 12;
	r.x2 = P->x + 12;
	r.y1 = P->y - 1;
	r.y2 = P->y + 2;

	cdisp_rect_fill_r(&r);

	cdisp_cyrcle_holl(P->x, P->y, 8);
	cdisp_cyrcle_holl(P->x, P->y, 9);

}
//----------------------------------------------------------------------------------------
int main(void)
{

	struct text_def txt;
	long x = -1, y = -1;
	char s[64];// = {0,0};
	long cnt = 0;
	long t1, t2;
	int i;
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_USART1EN | RCC_APB2ENR_SPI1EN;
	//RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

	SysTick_Config(SystemCoreClock / 1000);  // 1000 Hz

  // INIT LCD
	init_LCD();
	init_usart1(9600);
	xpt2046_init();
	cdisp_set_font(&font_Verdana_12);
	current_font_color.l = 0xff7f00;
	current_color.l = 0;
	cdisp_rect_fill(0,0, LCD_WIDTH, LCD_HEIGHT);
	cdisp_txt_set_default(&txt);

	encoder_init();

	//txt.text = "welcome";
	//cdisp_text(0,0, &txt);
	//USART1->DR = 'w';

	current_color.l = 0x40ffff;
//	t1 = systick_ms;
//	for (x = 0; x < 200; x += 5)
//	{
//		for (y = 0; y < 280; y +=5)
//		{
//			current_color.l = (x << 4) + (y >> 2);
//			cdisp_triangle_fill(x,y, x+20, y+40, x+40, y+4);
////			struct point p1, p2, p3;
////			p1.x = x; p1.y = y;
////			p2.x = x+20;
////			p2.y = y+40;
////			p3.x = x+40;
////			p3.y = y+4;
////			cdisp_triangle_fill_p(&p1, &p2, &p3);
//
//		}
//	}

//	for (x = 0; x < 10; x++)
//	{
//		for (y = 0; y < 10; y++)
//		{
//			current_color.l = (x << 4) + (y << 12);
//			#ifdef variantA
//				struct point p1, p2, p3;
//				p1.x = x;
//				p1.y = y;
//				p2.x = x+120;
//				p2.y = y+320;
//				p3.x = x+230;
//				p3.y = y+4;
//				cdisp_triangle_fill_p(&p1, &p2, &p3);  // À
//			#endif
//
//			#ifdef variantB
//				cdisp_triangle_fill(x,y, x+120, y+320, x+230, y+4);   // Á
//			#endif
//		}
//	}
//	do_test();

//	struct point P[5] = {{1,1}, {900,160+400}, {-1185,160}, {900,160-400}, {1,319}};
//	cdisp_bezier_p(P, 5, 100);

//	struct point P[4] = {{0,80}, {240,320}, {240, 0}, {0,320-80}};
//	cdisp_bezier_p(P, 4, 50);

//	t2 = systick_ms;
//	sprintf(s, "%i ms", t2 - t1);
//	current_color.l = 0;
	txt.text = s;
	txt.position = CDISP_TXT_TURN;
//	//txt.xscale = 3;
//	//txt.yscale = 3;
//	cdisp_text(0,10, &txt);


//	// калибровка touch panel
//	union rect Rt, Rs, rr;
//	Rs.x1 = 20;
//	Rs.x2 = LCD_WIDTH - 20;
//	Rs.y1 = 20;
//	Rs.y2 = LCD_HEIGHT - 20;
//
//	xpt2046_threshold = 50;
//
//	// point 1
//	draw_touch_target(Rs.p + 0);
//
//	for (x = y = i = 0, tmr1 = 0; tmr1 < 400;)
//	{
//		if(!XPT2046_acquireXY(&Rt.p[0])) // wait assured touch
//		{
//			tmr1 = 0;
//			x = y = i = 0;
//		}
//		else if(tmr1 > 250)
//		{
//			i++;
//			x += Rt.p[0].x;
//			y += Rt.p[0].y;
//		}
//	}
//
//	Rt.p[0].x = x / i;
//	Rt.p[0].y = y / i;
//
//	erase_touch_target(Rs.p + 0);
//
//	for (tmr1 = 0; tmr1 < 200;) 	if(xpt2046_int_active()) tmr1 = 0; // wait assured detouch
//
//	// point 2
//	draw_touch_target(Rs.p + 1);
//
//	for (x = y = i = 0, tmr1 = 0; tmr1 < 400;)
//	{
//		if(!XPT2046_acquireXY(&Rt.p[1])) // wait assured touch
//		{
//			tmr1 = 0;
//			x = y = i = 0;
//		}
//		else if(tmr1 > 250)
//		{
//			i++;
//			x += Rt.p[1].x;
//			y += Rt.p[1].y;
//		}
//	}
//
//	Rt.p[1].x = x / i;
//	Rt.p[1].y = y / i;
//
//	erase_touch_target(Rs.p + 1);
//	for (tmr1 = 0; tmr1 < 200;) 	if(xpt2046_int_active()) tmr1 = 0; // wait assured detouch
//
//	XPT2046_set_scale(&Rs, &Rt);
//
//	sprintf(s, "ax=%5i, bx=%5i", xpt2046_ax, xpt2046_bx);
//	cdisp_text(100,10, &txt);
//
//	sprintf(s, "ay=%5i, by=%5i", xpt2046_ay, xpt2046_by);
//	cdisp_text(80,10, &txt);
//
	xpt2046_threshold = 2;


	tmr1 = 0;
  while(1)
  {
  	//x = xpt2046_readX();
  	//y = xpt2046_readY();
  	struct point pp;


//  	xpt2046_readZ12(&pp);
//		sprintf(s, "z1=%5i, z2=%5i       ", (int)pp.x, (int)pp.y);
//		cdisp_text(100,10, &txt);

		sprintf(s, "ENC=%5i     ", (int)encoder_get());
		cdisp_text(100,10, &txt);




  	switch(XPT2046_acquireXY(&pp))
  	{
  		case 0:// not touched
				current_font_color.l = 0;
				current_color.l = 0xffffff;
				x = y = -1;
  		break;

  		case 2: // 2 - normal
				XPT2046_scale_p(&pp);

				current_color.l = 0x00ff00;
				//cdisp_rect_fill(pp.x - 2, pp.y - 2, pp.x + 2, pp.y + 2);
				if(x >= 0 && y >= 0)
				{
					cdisp_line(x,y, pp.x, pp.y);
				}
				x = pp.x;
				y = pp.y;
				current_color.l = 0xffff00;
				cdisp_rect_fill(pp.x - 1, pp.y - 1, pp.x + 1, pp.y + 1);
				current_font_color.l = 0xffff00;
				current_color.l = 0xff0000;
  		break;
  	}

  	//sprintf(s, " x=%5i, Y=%5i      ", pp.x, pp.y);
  	//cdisp_text(0,10, &txt);

//		if (usart_received)
//		{
//			char ch = usart_rdd;
//			char newline = 0;
//			usart_received  = 0;
//			s[0] = ch;
//			s[1] = 0;
//			short dx = cdisp_get_text_width(s);
//			if(ch == '\r')
//			{
//				ch = 0;
//				newline = 1;
//			}
//			if (dx + x >= LCD_WIDTH) newline = 1;
//			if(newline)
//			{
//				short h = cdisp_get_font_height();
//
//				//current_color.l = 0x000040;
//				cdisp_rect_fill(x,y, LCD_WIDTH, y + h);
//				//current_color.l = 0xffff00;
//				x = 0;
//
//				y += h;
//				if(y + h > LCD_HEIGHT)
//				{
//					y = 0;
//					// scroll
//				}
//			}
//
//			if (ch)
//			{
//				txt.text = s;
//				cdisp_text(x,y, &txt);
//				x += dx;
//			}


//			cdisp_triangle_fill(x,y, x+5, y+10, x+10, y);
//			struct point p1, p2, p3;
//			p1.x = x; p1.y = y;
//			p2.x = x+5;
//			p2.y = y+10;
//			p3.x = x+10;
//			p3.y = y;
//			cdisp_triangle_fill_p(&p1, &p2, &p3);
//		}
  }
}
