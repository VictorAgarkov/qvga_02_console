#ifndef _disp_h
#define _disp_h

#include "font.h"
#include "point_types.h"
#pragma anon_unions

//#define variantB
//#define variant2


//------------------------------------------------------------------------------
#ifdef __cplusplus
    extern "C"{
#endif

#define CDISP_TXT_TURN   1
#define CDISP_TXT_MIRR_X 2
#define CDISP_TXT_MIRR_Y 4
#define CDISP_TXT_MIRR_XY (CDISP_TXT_MIRR_X | CDISP_TXT_MIRR_Y)

#define CDISP_TXT_ALGN_LEFT   0x10
#define CDISP_TXT_ALGN_CENTER 0x00
#define CDISP_TXT_ALGN_RIGHT  0x20
#define CDISP_TXT_ALGN_TOP    0x40
#define CDISP_TXT_ALGN_MID    0x00
#define CDISP_TXT_ALGN_BOT    0x80

//------------------------------------------------------------------------------
union color
{
  unsigned long l;
  unsigned char ch[4];
  struct
  {
    unsigned char r;  // R
    unsigned char g;  // G
    unsigned char b;  // B
    unsigned char a;  // alpha (opacity)
  };
};

//------------------------------------------------------------------------------
struct text_def
{
  // параметры, заваемые пользователем
  char *text;     // string
  char xscale, yscale;  // text scale factor by axis (with normal text size == 1,1)
  unsigned char position;         // direction & allign:
                                  // bit 0: 0 - left -> right, 1 - top -> bottom
                                  // bit 1: mirror around X axis
                                  // bit 2: mirror around Y axis
                                  // bit 3: -
                                  // bit 4,5: horizontal allign, 0 - center, 1 - left, 2 - right
                                  // bit 6,7: vertical allign,   0 - middle, 1 - top,  2 - bottom

  // параметры, изменяющиеся во вермя инициализации и работы
  short left;       // left offset
  short top;        // top offset
  short pix_width;  // ширина текста в пикселях
  const struct font_char_def *fcd;
  unsigned char ccol;       // current collumn in current char
  unsigned char cchar;      // current char
};


extern union color current_color;      // color for graphics & text background
extern union color current_font_color; // color for text

//------------------------------------------------------------------------------
int  cdisp_setxy(unsigned short x, unsigned short y);
void cdisp_sort_rect_xy(union rect *R);

// Graphical primitives

void cdisp_rect_fill(short x1, short y1, short x2, short y2);
void cdisp_rect_fill_p(struct point *P1, struct point *P2);
void cdisp_rect_fill_r(union rect *R);
void cdisp_rect_holl(short x1, short y1, short x2, short y2);
void cdisp_rect_holl_p(struct point *P1, struct point *P2);
void cdisp_rect_holl_r(union rect *R);

void cdisp_cyrcle_fill(short x, short y, unsigned short radius);
void cdisp_cyrcle_holl(short x, short y, unsigned short radius);

void cdisp_line(short x1, short y1, short x2, short y2);
void cdisp_line_p(struct point *P1, struct point *P2);
void cdisp_line_r(union rect *R);

void cdisp_triangle_fill(short x1, short y1, short x2, short y2, short x3, short y3);
void cdisp_triangle_fill_p(struct point *P1, struct point *P2, struct point *P3);
void cdisp_triangle_holl(short x1, short y1, short x2, short y2, short x3, short y3);
void cdisp_triangle_holl_p(struct point *P1, struct point *P2, struct point *P3);

void cdisp_bezier_p(struct point *P, int p_num, int t_num);

void cdisp_put_bitmap16_r(union rect *R, unsigned short *src);
void cdisp_put_bitmap16(short x1, short y1, short x2, short y2, unsigned short *src);
void cdisp_get_bitmap16_r(union rect *R, unsigned short *dst);
void cdisp_get_bitmap16(short x1, short y1, short x2, short y2, unsigned short *src);


// Text output

void cdisp_txt_set_default(struct text_def *txt);
void cdisp_set_font(const struct font_def *font);
int cdisp_get_font_height(void);
int cdisp_get_text_width(char *text);
const struct font_char_def *cdisp_find_font_char(unsigned char ch);
void cdisp_allign_text_in_rect(union rect *r, struct text_def *txt);
int cdisp_font_fill_col(const struct font_char_def *cd, unsigned char col, unsigned char *dst, short num, char yscale);
void cdisp_rect_text(short x1, short y1, short x2, short y2, struct text_def *txt);
void cdisp_rect_text_r(union rect *R, struct text_def *txt);
void cdisp_text(short x1, short y1, struct text_def *txt);
void cdisp_text_p(struct point *P, struct text_def *txt);

//------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif
