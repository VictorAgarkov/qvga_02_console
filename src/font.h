#ifndef _font_h
#define _font_h

#ifdef __cplusplus
extern "C"{
#endif

typedef struct font_char_def
{
  // ��������� ������������� ������� � ������������ �������
  unsigned char wt; // ������ ����������
  unsigned char x;  // �������� �� X
  unsigned char y;  // �������� ��  Y
  unsigned char w;  // ������
  unsigned char h;  // ������
  const char *m;    // ���������� ���� �����������
}font_char_def_t;


typedef struct font_set_def
{
  // ��������� ����� �� ��������� ������ ������ ����������� ��������
  unsigned char first; // ������ ������
  unsigned char num;   // ���������� ��������
  const struct font_char_def *chars; // ���������� �������
}font_set_def_t;


typedef struct font_def
{
  // �������� � ���� ��������� ������� ����������� ��������
  const struct font_set_def *set; //
  unsigned char set_num;
  unsigned char th;      // ������ ����������
  const struct font_char_def *dflt;  // ������ ������ �� ����������
}font_def_t;


extern const struct font_def font_Verdana_10;
extern const struct font_def font_Verdana_12;
extern const struct font_def font_Verdana_16;
extern const struct font_def font_Verdana_40;

#ifdef __cplusplus
}
#endif

#endif
