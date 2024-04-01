
#include <string.h>
#include "cdisp.h"
#include "lcd.h"
//------------------------------------------------------------------------------
//static unsigned short curr_x, curr_y; // current X, Y

const struct font_def *current_font = NULL;
//const struct font_def *current_font = &font_Verdana_10;
//const struct font_def *current_font = &font_Verdana_12;
//const struct font_def *current_font = &font_Verdana_16;
union color current_color = {0x00ffffff};
union color current_font_color = {0x00404040};

unsigned short line16_buff[320];
unsigned char  line_txt8[320];

// internal functions
int cdisp_get_next_Vtext_vline(struct text_def *txt, unsigned char *dst);
int cdisp_get_next_Htext_vline(struct text_def *txt, unsigned char *dst, short num);
long cdisp_get_incline_16p16(short x1, short y1, short x2, short y2);

#define abs(x) ((x) < 0 ? -(x) : (x))
#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)
#define sign(a) ((a) < 0 ? -1 : 1)

//------------------------------------------------------------------------------
void do_test(void)
{
	int x = 0, y = 0;
	for (x = 0; x < 100; x++)
	{
		for (y = 0; y < 100; y++)
		{
			current_color.l = (x << 2) | (y << 10) | 0xff0000;
			//cdisp_line(0, x * 3, 239, y * 3);  // длиные линии - от края до края
			cdisp_line(x * 2, y * 3, x * 2 + 3, y * 3 + 3);  // короткие линии - от края до края
			//cdisp_line(239, x * 3, 180, 160);  // test
//			#ifdef variantA
//				struct point p1, p2, p3;
//				p1.x = x;
//				p1.y = y;
//				p2.x = x+120;
//				p2.y = y+320;
//				p3.x = x+230;
//				p3.y = y+4;
//				cdisp_triangle_fill_p(&p1, &p2, &p3);  // А
//			#endif
//
//			#ifdef variantB
//				cdisp_triangle_fill(x,y, x+120, y+320, x+230, y+4);   // Б
//			#endif
		}
	}
//	current_color.l = 0x00ffff;
//	cdisp_line(120, 160, 120, 0);
//	current_color.l = 0xff0000;
//	cdisp_line(120, 0, 120, 160);
}

//------------------------------------------------------------------------------

int cdisp_setxy(unsigned short x, unsigned short y)
{
	int ret = LCD_setxy(x, y);
	if(!ret)
	{
//    curr_x = x;
//    curr_y = y;
	}
	return ret;
}
//------------------------------------------------------------------------------
void cdisp_sort_rect_xy(union rect *R)
{
	// смотрим, что бы x1 < x2, y1 < y2
	short t;
	#define swap(a, b)  {t = a; a = b; b = t;}
	if(R->x1 > R->x2)	swap(R->x1, R->x2)
	if(R->y1 > R->y2) swap(R->y1, R->y2)
	#undef swap
}
//------------------------------------------------------------------------------
void cdisp_rect_fill(short x1, short y1, short x2, short y2)
{
	union rect R;
	R.x1 = x1;
	R.x2 = x2;
	R.y1 = y1;
	R.y2 = y2;
	cdisp_rect_fill_r(&R);
}
//------------------------------------------------------------------------------
void cdisp_rect_fill_p(struct point *P1, struct point *P2)
{
	union rect R;
	R.x1 = P1->x;
	R.y1 = P1->y;

	R.x2 = P2->x;
	R.y2 = P2->y;

	cdisp_rect_fill_r(&R);
}
//------------------------------------------------------------------------------
void cdisp_rect_fill_r(union rect *R)
{
	// заливаем прямоугольник цветом current_color
	short h;
	int i;
	cdisp_sort_rect_xy(R);

	h = min(R->y2 - R->y1 + 1, LCD_HEIGHT);

	for(i = R->x1; i <= R->x2; i++)
	{
		LCD_put_vconst_16(i, R->y1, h, RGB_24_to_16(current_color.l));
	}
}
//------------------------------------------------------------------------------
void cdisp_rect_holl(short x1, short y1, short x2, short y2)
{
	union rect R;
	R.x1 = x1;
	R.y1 = y1;
	R.x2 = x2;
	R.y2 = y2;
	cdisp_rect_holl_r(&R);
}
//------------------------------------------------------------------------------
void cdisp_rect_holl_p(struct point *P1, struct point *P2)
{
	union rect R;
	R.x1 = P1->x;
	R.y1 = P1->y;

	R.x2 = P2->x;
	R.y2 = P2->y;
	cdisp_rect_holl_r(&R);
}
//------------------------------------------------------------------------------
void cdisp_rect_holl_r(union rect *R)
{
	short i, h;
	unsigned short b16 = RGB_24_to_16(current_color.l);

	cdisp_sort_rect_xy(R);

	h = R->y2 - R->y1 + 1;

	LCD_put_vconst_16(R->x1, R->y1, h, b16);

	for(i = R->x1; i <= R->x2; i++)
	{
		LCD_setpixel(i, R->y1, b16);
		LCD_setpixel(i, R->y2, b16);
	}

	LCD_put_vconst_16(R->x2, R->y1, h, b16);
}
//------------------------------------------------------------------------------
void  cdisp_cyrcle_fill(short x, short y, unsigned short radius)
{
	short a, b, P;
	short h;
	unsigned short b16 = RGB_24_to_16(current_color.l);

	h = min(radius * 2 + 1, LCD_HEIGHT);
	for (a = 0; a < h; a++)
	{
		line16_buff[a] = b16;
	}

	a = 0;
	b = radius;
	P = 1 - radius;

	do
	{
		// draw cyrcle with vertical lines
		if(P >= 0 && a != b)
		{
			h = a * 2 + 1;
			LCD_put_vconst_16(x+b, y-a, h, b16);
			LCD_put_vconst_16(x-b, y-a, h, b16);
		}

		LCD_put_vconst_16(x+a, y-b, b * 2 + 1, b16);
		if(a)
		{
			LCD_put_vconst_16(x-a, y-b, b * 2 + 1, b16);
		}

		if(P < 0) P += 3 + 2 * a++;
		else      P += 5 + 2 * (a++ - b--);
	}
	while(a <= b);
}
//------------------------------------------------------------------------------
void  cdisp_cyrcle_holl(short x, short y, unsigned short radius)
{
	short a, b, P;
	unsigned short b16 = RGB_24_to_16(current_color.l);

	a = 0;
	b = radius;
	P = 1 - radius;

	do
	{
		LCD_setpixel(x+a, y+b, b16);
		LCD_setpixel(x+a, y-b, b16);

		if(a)
		{
			LCD_setpixel(x-a, y-b, b16);
			LCD_setpixel(x-a, y+b, b16);
		}

		if(a != b)
		{
			LCD_setpixel(x+b, y+a, b16);
			LCD_setpixel(x-b, y+a, b16);

			if(a)
			{
				LCD_setpixel(x+b, y-a, b16);
				LCD_setpixel(x-b, y-a, b16);
			}
		}

		if(P < 0) P += 3 + 2 * a++;
		else      P += 5 + 2 * (a++ - b--);
	}
	while(a <= b);
}
//------------------------------------------------------------------------------
void cdisp_line(short x1, short y1, short x2, short y2)
{
	union rect R;
	R.x1 = x1;
	R.y1 = y1;
	R.x2 = x2;
	R.y2 = y2;
	cdisp_line_r(&R);
}
//------------------------------------------------------------------------------
void cdisp_line_p(struct point *P1, struct point *P2)
{
	union rect R;

	R.x1 = P1->x;
	R.y1 = P1->y;
	R.x2 = P2->x;
	R.y2 = P2->y;

	cdisp_line_r(&R);
}
//------------------------------------------------------------------------------
void cdisp_line_r(union rect *R)
{
	unsigned short b16 = RGB_24_to_16(current_color.l);
//#if 1 == 1
	short dy, dx;

	short	P, diff;
	short xx, yy;

	short	addx = 1;
	short	addy = 1;

	dx = R->x2 - R->x1;
	if(dx < 0)
	{
		dx = -dx;
		addx = -1;
	}

	dy = R->y2 - R->y1;
	if(dy < 0)
	{
		dy = -dy;
		addy = -1;
	}

	xx = R->x1;
	yy = R->y1;


	if(dx >= dy)
	{
		dy *= 2;
		P = dy - dx;

		diff = P - dx;

		for(; xx != R->x2; )
		{
			LCD_setpixel(xx, yy, b16);

			xx += addx;
			if(P < 0)
			{
				P  += dy;
			}
			else
			{
				P  += diff;
				yy += addy;
			}
		}
	}
	else
	{
		dx *= 2;
		P = dx - dy;
		diff = P - dy;

		for(; yy != R->y2; )
		{
			LCD_setpixel(xx, yy, b16);
			yy += addy;
			if(P < 0)
			{
				P  += dx;
			}
			else
			{
				P  += diff;
				xx += addx;
			}
		}
	}
//#else
//  // 1% faster on long lines
//  // 27% slower on short linnes :(
//	long dc, c;
//	short i, di, dest;
//	if(abs(R->x1 - R->x2) > abs(R->y1 - R->y2))
//	{
//		// step with X
//		di = sign(R->x2 - R->x1);
//		dc = cdisp_get_incline_16p16(R->x1, R->y1, R->x2, R->y2) * di;   // dY
//		dest = R->x2 + 1;
//		c = (long)R->y1 << 16;
//		for(i = R->x1; i != dest; i += di)
//		{
//			LCD_setpixel(i, c >> 16, b16);
//			c += dc;
//		}
//	}
//	else
//	{
//		// step with Y
//		di = sign(R->y2 - R->y1);
//		dc = cdisp_get_incline_16p16(R->y1, R->x1, R->y2, R->x2) * di;   // dY
//		dest = R->y2 + 1;
//		c = (long)R->x1 << 16;
//		for(i = R->y1; i != dest; i += di)
//		{
//			LCD_setpixel(c >> 16, i, b16);
//			c += dc;
//		}
//	}
//#endif
}

//------------------------------------------------------------------------------
long cdisp_get_incline_16p16(short x1, short y1, short x2, short y2)
{
	// get dX by one step Y by 2 points
	// return - fixed point 16.16 value
	long dy = (long)y2 - y1;
	dy = (dy << 16) / ((long)x2 - x1);
	return dy;
}
//------------------------------------------------------------------------------
void cdisp_triangle_fill_p(struct point *P1, struct point *P2, struct point *P3)
{
	// filled triangle
	// sort by X

//#ifdef variant1
	short t;
	#define swap(a, b)  {t = a; a = b; b = t;}
	if(P1->x > P2->x)
	{
		swap(P1->x, P2->x);
		swap(P1->y, P2->y);
	}

	if(P2->x > P3->x)
	{
		swap(P2->x, P3->x);
		swap(P2->y, P3->y);
	}

	if(P1->x > P2->x)
	{
		swap(P1->x, P2->x);
		swap(P1->y, P2->y);
	}
	#undef swap
	if(P3->x == P1->x)
	{
		// прямая вертикальная линия
		short yh, yl;
		yh = max(P1->y, max(P2->y, P3->y));
		yl = min(P1->y, min(P2->y, P3->y));
		cdisp_rect_fill(P1->x, yh, P1->x, yl);
	}
	else
	{
		long dy12, dy23, dy13; //long dP1->P2->y, dP2->P3->y, dP1->P3->y;
		long yh, yl;
		short xx;
		dy13 = cdisp_get_incline_16p16(P1->x, P1->y, P3->x, P3->y);

		yh = (long)P1->y << 16;

		if(P1->x != P2->x)
		{
			dy12 = cdisp_get_incline_16p16(P1->x, P1->y, P2->x, P2->y);
			yl = yh;

			for(xx = P1->x; xx < P2->x; xx++)
			{
				cdisp_rect_fill(xx, yh >> 16, xx, yl >> 16);
				yh += dy13;
				yl += dy12;
			}
		}

		if(P3->x != P2->x)
		{
			dy23 = cdisp_get_incline_16p16(P2->x, P2->y, P3->x, P3->y);
			yl = (long)P2->y << 16;

			for(xx = P2->x; xx <= P3->x; xx++)
			{
				cdisp_rect_fill(xx, yh >> 16, xx, yl >> 16);
				yh += dy13;
				yl += dy23;
			}
		}
	}
//#endif
//#ifdef variant2
//	cdisp_triangle_fill(P1->x, P1->y, P2->x, P2->y, P3->x, P3->y);
//#endif
}
//------------------------------------------------------------------------------
void  cdisp_triangle_fill(short x1, short y1, short x2, short y2, short x3, short y3)
{

//#ifdef variant1
	struct point P1, P2, P3;
	P1.x = x1;
	P1.y = y1;
	P2.x = x2;
	P2.y = y2;
	P3.x = x3;
	P3.y = y3;
	cdisp_triangle_fill_p(&P1, &P2, &P3);
//#endif
//
//#ifdef variant2
//
//
//// filled triangle
//	// sort by X
//	short t;
//	#define swap(a, b)  {t = a; a = b; b = t;}
//	if(x1 > x2)
//	{
//		swap(x1, x2);
//		swap(y1, y2);
//	}
//
//	if(x2 > x3)
//	{
//		swap(x2, x3);
//		swap(y2, y3);
//	}
//
//	if(x1 > x2)
//	{
//		swap(x1, x2);
//		swap(y1, y2);
//	}
//	#undef swap
//	if(x3 == x1)
//	{
//		// прямая вертикальная линия
//		short yh, yl;
//		yh = max(y1, max(y2, y3));
//		yl = min(y1, min(y2, y3));
//		cdisp_rect_fill(x1, yh, x1, yl);
//	}
//	else
//	{
//		long dy12, dy23, dy13;
//		long yh, yl;
//		short xx;
//		dy13 = cdisp_get_incline_16p16(x1, y1, x3, y3);
//
//		yh = (long)y1 << 16;
//
//		if(x1 != x2)
//		{
//			dy12 = cdisp_get_incline_16p16(x1, y1, x2, y2);
//			yl = yh;
//
//			for(xx = x1; xx < x2; xx++)
//			{
//				cdisp_rect_fill(xx, yh >> 16, xx, yl >> 16);
//				yh += dy13;
//				yl += dy12;
//			}
//		}
//
//		if(x3 != x2)
//		{
//			dy23 = cdisp_get_incline_16p16(x2, y2, x3, y3);
//			yl = (long)y2 << 16;
//
//			for(xx = x2; xx <= x3; xx++)
//			{
//				cdisp_rect_fill(xx, yh >> 16, xx, yl >> 16);
//				yh += dy13;
//				yl += dy23;
//			}
//		}
//	}
//#endif
}
//------------------------------------------------------------------------------
void cdisp_triangle_holl_p(struct point *P1, struct point *P2, struct point *P3)
{
	cdisp_line_p(P1, P2);
	cdisp_line_p(P2, P3);
	cdisp_line_p(P1, P3);
}
//------------------------------------------------------------------------------
void  cdisp_triangle_holl(short x1, short y1, short x2, short y2, short x3, short y3)
{
	cdisp_line(x1, y1, x2, y2);
	cdisp_line(x2, y2, x3, y3);
	cdisp_line(x3, y3, x1, y1);
}
//------------------------------------------------------------------------------
long cdisp_interpolate_16_16_by_1_31(long v1, long v2, unsigned long factor)
{
	// v1, v2 - fixed point 16.16
	// factor - fixed point 1.31
	// return - fixed point 16.16
	long long ll = (long long)v2 * factor;
	ll += (long long)v1 * (0x80000000UL - factor);
	return ll >> 31;
}
//------------------------------------------------------------------------------
void cdisp_bezier_p(struct point *P, int p_num, int t_num)
{
	// рисуем кривые Безье методом Поля де Фаже де Кастельжо.
	// P - массив опорных точек
	// p_num - кол-во опорных точек
	// t_num - кол-во отрезков интерполяции

	if(p_num < 1 || p_num > 12) return;

	// используем line16_buff в каечстве буфера для хранения промежуточных точек
	// точки буду хранится в формате с фиксированной запятой 16.16
	// для построения кривой 12-го порядка используется 1+2+3+..+12=78 точек
	// что занимает 312 из 320 элементов line16_buff.

	long *lp = (long*)line16_buff;
	long *lpp;
	unsigned long t, dt = 0x80000000UL / t_num;   // приращение с фиксированной точкой 8.24
	int i, j;

	long x0, y0;

	// get the points
	for(i = 0; i < p_num; i++)
	{
		lp[i * 2 + 0] = (long)P[i].x << 16;
		lp[i * 2 + 1] = (long)P[i].y << 16;
	}

	// interpolate by t_num
	lpp = lp;
	for(t = dt; t < 0x80000000UL; t += dt)
	{
		x0 = lpp[0];
		y0 = lpp[1];
		lpp = lp;
		for(i = p_num - 1; i > 0; i--)
		{
			for(j = 1; j <= i * 2; j++)
			{
				lpp[i * 2 + 2] = cdisp_interpolate_16_16_by_1_31(lpp[0], lpp[2], t);
				lpp++;
			}
			lpp += 2;
		}
		cdisp_line(x0 >> 16, y0 >> 16, lpp[0] >> 16, lpp[1] >> 16);
	}
}
//------------------------------------------------------------------------------
void cdisp_put_bitmap16_r(union rect *R, unsigned short *src)
{
	int i;
	short h;

	cdisp_sort_rect_xy(R);
	h = R->y2 - R->y1 + 1;

	for(i = R->x1; i <= R->x2; i++)
	{
		LCD_put_vline_16(i, R->y1, h, src);
		src += h;
	}
}
//------------------------------------------------------------------------------
void cdisp_put_bitmap16(short x1, short y1, short x2, short y2, unsigned short *src)
{
	union rect R;
	R.x1 = x1;
	R.y1 = y1;
	R.x2 = x2;
	R.y2 = y2;
	cdisp_put_bitmap16_r(&R, src);
}
//------------------------------------------------------------------------------
void cdisp_get_bitmap16_r(union rect *R, unsigned short *dst)
{
	int i;
	short h;

	cdisp_sort_rect_xy(R);
	h = R->y2 - R->y1 + 1;

	for(i = R->x1; i <= R->x2; i++)
	{
		LCD_get_vline_16(i, R->y1, h, dst);
		dst += h;
	}
}
//------------------------------------------------------------------------------
void cdisp_get_bitmap16(short x1, short y1, short x2, short y2, unsigned short *src)
{
	union rect R;
	R.x1 = x1;
	R.y1 = y1;
	R.x2 = x2;
	R.y2 = y2;
	cdisp_get_bitmap16_r(&R, src);
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------












//---------------------------------------------------------------------------------------------------------------------------------


/*
void cdisp_out_text(short x, short y, char *text)
{
	// белые буквы на черном фоне
	if(!current_font) return;
	while(*text)
	{
		const struct font_char_def *ch = cdisp_find_font_char((unsigned char)(*text));
		if(ch)
		{
			int i;
			for(i = 0; i < ch->wt; i++)
			{
				int yy;
				disp_font_fill_col(ch, i, line_txt8);

				for(yy = 0; yy < current_font->th; yy++)
				{
					line16_buff[yy] = RGB_16(line_txt8[yy], line_txt8[yy], line_txt8[yy]);
				}
				LCD_put_vline_16(x, y, current_font->th, line16_buff);

				x++;
			}
		}
		text++;
	}
}
*/

//------------------------------------------------------------------------------
void cdisp_txt_set_default(struct text_def *txt)
{
	txt->left = txt->top = 0;
	txt->ccol = txt->cchar = 0;
	txt->position = 0;
	txt->xscale = txt->yscale = 1;
}
//------------------------------------------------------------------------------
void cdisp_set_font(const struct font_def *font)
{
	current_font = font;
}
//------------------------------------------------------------------------------
int cdisp_get_font_height()
{
	return current_font->th;
}
//------------------------------------------------------------------------------
int cdisp_get_text_width(char *text)
{
	// вычисляем длину в пикселах, которую займет указаный текст текущим шрифтом
	int ret = 0;
	if(current_font && text)
	{
		while(*text)
		{
			const struct font_char_def *ch = cdisp_find_font_char((unsigned char)(*text));
			if(ch) ret += ch->wt;
			text++;
		}
	}
	return ret;
}
//------------------------------------------------------------------------------
void cdisp_txt_check_scale(struct text_def *txt)
{
	if(txt)
	{
		if(txt->xscale <= 0) txt->xscale = 1;
		if(txt->yscale <= 0) txt->yscale = 1;
	}
}
//------------------------------------------------------------------------------
const struct font_char_def * cdisp_find_font_char(unsigned char ch)
{
	// ищем битмап символа в текущем шрифте
	const struct font_char_def *ret = 0;
	if(current_font)
	{
		int i;
		for(i = 0; i < current_font->set_num; i++)
		{
			const struct font_set_def *fsd =  current_font->set + i;
			unsigned char first = fsd->first;
			unsigned char last  = first + fsd->num - 1;

			if(ch >= first && ch <= last)
			{
				ret =  fsd->chars + ch - first;
				break;
			}
		}
	}
	if(!ret) ret = current_font->dflt;
	return ret;
}
//------------------------------------------------------------------------------
void cdisp_allign_text_in_rect(union rect *r, struct text_def *txt)
{
	// задаём некоторые параметры текста перед его выводом
	int i, o, ri;
	int ww = 0, hh = 0;
	char vscale, hscale;

	if(!txt | !r) return;


	cdisp_txt_check_scale(txt);
	cdisp_sort_rect_xy(r);

	i = cdisp_get_text_width(txt->text);

	// ширина и высота в зависимости от направления
	if ((txt->position & CDISP_TXT_TURN) == 0)
	{
		vscale = txt->yscale;
		hscale = txt->xscale;
		ww = i * hscale;
		if(current_font) hh = current_font->th * vscale;
		txt->pix_width = ww;
	}
	else
	{
		vscale = txt->xscale;
		hscale = txt->yscale;
		hh = i * hscale;
		if(current_font) ww = current_font->th * vscale;
		txt->pix_width = hh;
	}

	// отступ слева
	ri = r->x2 - r->x1;
	switch (txt->position & (CDISP_TXT_ALGN_LEFT | CDISP_TXT_ALGN_RIGHT))
	{
		case CDISP_TXT_ALGN_LEFT: // left
		{
			o = 0;
			break;
		}
		case CDISP_TXT_ALGN_RIGHT:  // right
		{
			o = ri - ww;
			break;
		}
		default: // center
		{
			o = (ri - ww) / 2;
			break;
		}
	}

	txt->left = max(0, o);

	// отступ сверху
	ri = r->y2 - r->y1;
	switch (txt->position & (CDISP_TXT_ALGN_TOP | CDISP_TXT_ALGN_BOT))
	{
		case CDISP_TXT_ALGN_TOP:  // top
		{
			o = 0;
			break;
		}
		case CDISP_TXT_ALGN_BOT:  // bottom
		{
			o = ri - hh;
			break;
		}
		default: // center
		{
			o = (ri - hh) / 2;
			break;
		}
	}

	txt->top = max(0, o);


	// set to begin of string
	txt->ccol = 0;
	txt->cchar = 0;
}
//------------------------------------------------------------------------------
int cdisp_get_next_Vtext_vline(struct text_def *txt, unsigned char *dst)
{
  // заливаем 1 вертикальную линию пикселей ВЕРТКАЛЬНЫМ текстом

  // возврат:
  // 0 - нет фрагментов символа
  // 1 - есть фрагмент символа

	int ret = 0;


	memset(dst, 0, txt->pix_width + txt->top);


	// движемся по всем символам снизу вверх по полю ccol
	if(txt->ccol < current_font->th)
	{
		int i, row;
		unsigned char ch;
		for (row = txt->top, i = 0; (ch = txt->text[i]) != 0; i++)
		{
			const struct font_char_def *cd = cdisp_find_font_char(ch);
			int k, ccy;

			if(cd && cd->m)
			{
				ccy = txt->ccol - (current_font->th - cd->y - cd->h);

				if (ccy >= 0 && ccy <  cd->h)
				{
					int mstep = (cd->h + 1) / 2;

					char *cp = (char*)cd->m + ((cd->h - ccy - 1) / 2);
					unsigned char *dstp = dst + row + cd->x * txt->yscale;
					for(k = cd->x; k < cd->w + cd->x; k++)
					{
						unsigned char nibble = *cp;
						if(((ccy ^ cd->h) & 1) != 0) nibble >>= 4;
						nibble = (nibble & 0x0f) | (nibble << 4);  // expand 4-bit value to 8-bit
						if (nibble) ret = 1;
						int ii;

						for(ii = 0; ii < txt->yscale; ii++)
						{
							*(dstp++) = nibble;
						}
						cp += mstep;
					}
				}
				row += cd->wt * txt->yscale;
			}
		}
		txt->ccol++;
	}

	return ret;
}
//------------------------------------------------------------------------------
int cdisp_get_next_Htext_vline(struct text_def *txt, unsigned char *dst, short num)
{
	// заливаем 1 вертикальную линию пикселей горизонтальнам текстом (белым по черному фону)
	// возврат:
	// 0 - нет фрагментов символа
	// 1 - есть фрагмент символа
	int ret = 0;

	__check_next_char:
	if(txt->text[txt->cchar]) // still have chars?
	{
		if(!txt->ccol) //  letter is begin?
		{
			txt->fcd = cdisp_find_font_char(txt->text[txt->cchar]);
			if(!txt->fcd) // letter invalid - try next char
			{
				txt->cchar++;
				goto __check_next_char;
			}
		}

		// letter valid - draw it
		ret = cdisp_font_fill_col(txt->fcd, txt->ccol, dst + txt->top, num, txt->yscale);

		txt->ccol++;
		if(txt->ccol >= txt->fcd->wt) // letter complete - set next char
		{
			txt->ccol = 0;
			txt->cchar++;
		}
	}

	return ret;
}

//------------------------------------------------------------------------------
int cdisp_font_fill_col(const struct font_char_def *cd, unsigned char col, unsigned char *dst, short num, char yscale)
{
	// заливаем 1 вертикальный ряд пикселов в соотв. со шрифтом
	// возврат:
	// 0 - ничего не залили
	// 1 - что-то залили
	int ret = 0;
	memset(dst, 0, num);
	if(cd && cd->m && col >= cd->x && col < cd->x + cd->w)
	{
		short i, yscalecnt = 0;
		short ii = 0;
		unsigned char *dstp = dst + cd->y * yscale;
		short cnt = cd->h * yscale;
		char *cp = (char*)cd->m + ((cd->h + 1) / 2) * (col - cd->x);
		for(i = 0; i < cnt; i++)
		{
			unsigned char nibble;
			if(!yscalecnt)
			{
				yscalecnt = yscale;
				nibble = cp[ii >> 1];
				if((ii & 1) == 0) nibble >>= 4;
				nibble = (nibble & 0x0f) | (nibble << 4);
				if (nibble) ret = 1;
				ii++;
			}
			*(dstp++) = nibble;
			yscalecnt--;
		}
	}
	return ret;
}
//------------------------------------------------------------------------------
void cdisp_rect_text(short x1, short y1, short x2, short y2, struct text_def *txt)
{
	union rect R;
	R.x1 = x1;
	R.y1 = y1;
	R.x2 = x2;
	R.y2 = y2;
	cdisp_rect_text_r(&R, txt);
}
//------------------------------------------------------------------------------

void cdisp_rect_text_r(union rect *R, struct text_def *txt)
{
	// заливаем прямоугольник  цветом current_bkg_color
	// если есть текст - вставляем его туда
	short xx, yy, b16;
	char xscale_cnt = 0;

	//disp_sort_rect_xy(R);

	if(!R || !txt) return;

	cdisp_txt_check_scale(txt);


	b16 = RGB_24_to_16(current_color.l);


	short xx_s, xx_e, xx_d;
	if ((txt->position & CDISP_TXT_MIRR_X) == 0)
	{
		// no mirror X
		xx_s = R->x1;
		xx_e = R->x2 + 1;
		xx_d = 1;
	}
	else
	{
		// mirror X
		xx_s = R->x2;
		xx_e = R->x1 - 1;
		xx_d = -1;
	}

	for(xx = xx_s; xx != xx_e; xx += xx_d)
	{
		if(!xscale_cnt)
		{
			xscale_cnt = txt->xscale;
			unsigned char txt_rdy = 0;
			if(!txt->left)
			{
				if ((txt->position & CDISP_TXT_TURN) == 0) txt_rdy = cdisp_get_next_Htext_vline(txt, line_txt8, R->y2 - R->y1);
				else                                      txt_rdy = cdisp_get_next_Vtext_vline(txt, line_txt8);
			}
			unsigned char *ltx;
			short dltx;
			if ((txt->position & CDISP_TXT_MIRR_Y) == 0)
			{
				// No mirror Y
				ltx = line_txt8;
				dltx = 1;
			}
			else
			{
				// Mirror Y
				ltx = line_txt8 + R->y2 - R->y1 - 1;
				dltx = -1;
			}

			int lastyy = min(R->y2, sizeof(line_txt8) - 1);
			for(yy = R->y1; yy <= lastyy; yy++)
			{
				unsigned char mul = *ltx;
				ltx += dltx;
				if((txt_rdy != 0) && (mul != 0))  // применяем текст
				{
					int i;
					union color cc;
					for(i = 0; i < 3; i++)
					{
						int v1, v2;
						v1 = current_color.ch[i];
						v1 *= 0xff - mul;
						v2 = current_font_color.ch[i];
						v2 *= mul;
						cc.ch[i] = (v1 + v2 + 0x80) >> 8; // 0xff;
					}
					line16_buff[yy] = RGB_24_to_16(cc.l);
				}
				else
				{
					line16_buff[yy] = b16;
				}
			}
		}
		if(txt->left) txt->left--;
		xscale_cnt--;
		LCD_put_vline_16(xx, R->y1, R->y2 - R->y1 + 1, line16_buff + R->y1);
	}
}
//------------------------------------------------------------------------------
void cdisp_text(short x, short y, struct text_def *txt)
{
	struct point P;
	P.x = x;
	P.y = y;
	cdisp_text_p(&P, txt);
}
//------------------------------------------------------------------------------
void cdisp_text_p(struct point *P, struct text_def *txt)
{
	union rect R;
	int ww;

	if(!current_font || !txt) return;

	R.x1 = P->x;
	R.y1 = P->y;

	cdisp_txt_check_scale(txt);

	txt->left = txt->top = 0;
	txt->ccol = txt->cchar = 0;

	ww = cdisp_get_text_width(txt->text);

	if((txt->position & CDISP_TXT_TURN) == 0) // horisontal
	{
		txt->pix_width = ww * txt->yscale;
		R.x2 = R.x1 + txt->pix_width;
		R.y2 = R.y1 + current_font->th * txt->xscale;

	}
	else  // vertical
	{
		txt->pix_width = ww * txt->xscale;
		R.x2 = R.x1 + current_font->th * txt->yscale;
		R.y2 = R.y1 + txt->pix_width;
	}

	cdisp_rect_text_r(&R, txt);
}
//------------------------------------------------------------------------------

