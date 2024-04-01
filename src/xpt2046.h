#ifndef XPT2046_H_INCLUDED
#define XPT2046_H_INCLUDED

	#include "point_types.h"

	#define XPT2046_CS_PORT    GPIOA
	#define XPT2046_CS_PIN     4
	#define XPT2046_SCK_PORT   GPIOA
	#define XPT2046_SCK_PIN    5
	#define XPT2046_MOSI_PORT  GPIOA
	#define XPT2046_MOSI_PIN   7
	#define XPT2046_MISO_PORT  GPIOA
	#define XPT2046_MISO_PIN   6
	#define XPT2046_INT_PORT   GPIOC
	#define XPT2046_INT_PIN    13

	#define XPT2046_SPI        SPI1

	#define XPT2046_CS1       XPT2046_CS_PORT->BSRR = 1 << XPT2046_CS_PIN
	#define XPT2046_CS0       XPT2046_CS_PORT->BRR = 1 << XPT2046_CS_PIN

	extern short xpt2046_ax, xpt2046_ay;  // множители в формате с фиксированной точкой 0.16
	extern short xpt2046_bx, xpt2046_by;  // смещение
	extern int   xpt2046_threshold;       // порог помехозащищённости


	void xpt2046_init(void);
	//unsigned short xpt2046_readX(void);
	//unsigned short xpt2046_readY(void);
	//unsigned long xpt2046_readXY(void);

	void xpt2046_readXY_n(int n, short *xx, short *yy);
	void xpt2046_readZ12(struct point *P);
	int xpt2046_int_active(void);
	short xpt2046_scaleX(unsigned short rddX);
	short xpt2046_scaleY(unsigned short rddY);
	int XPT2046_acquireXY(struct point *P);
	void XPT2046_scale_p(struct point *P);
	int XPT2046_set_scale(union rect *Rs, union rect *Rt);

#endif /* XPT2046_H_INCLUDED */
