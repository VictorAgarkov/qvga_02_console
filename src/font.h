#ifndef _font_h
#define _font_h

#ifdef __cplusplus
extern "C"{
#endif

typedef struct font_char_def
{
  // параметры прямоугольной области с изображением символа
  unsigned char wt; // ширина знакоместа
  unsigned char x;  // смещение по X
  unsigned char y;  // смещение по  Y
  unsigned char w;  // ширина
  unsigned char h;  // высота
  const char *m;    // собственно само изображение
}font_char_def_t;


typedef struct font_set_def
{
  // описывает набор из несколько подряд идущих изображений символов
  unsigned char first; // первый символ
  unsigned char num;   // количество символов
  const struct font_char_def *chars; // собственно символы
}font_set_def_t;


typedef struct font_def
{
  // включает в себя несколько наборов изображений символов
  const struct font_set_def *set; //
  unsigned char set_num;
  unsigned char th;      // высота знакоместа
  const struct font_char_def *dflt;  // символ взамен не найденного
}font_def_t;


extern const struct font_def font_Verdana_10;
extern const struct font_def font_Verdana_12;
extern const struct font_def font_Verdana_16;
extern const struct font_def font_Verdana_40;

#ifdef __cplusplus
}
#endif

#endif
