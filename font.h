// font.h

#define CHAR_WIDTH_BITS	8


void font_test();
void font_test_1();
void font_test_2();
void font_test_3();
void font_test_4();

uint32_t *getVRaster(int n);
uint32_t *getVRasterForChar(char c);
uint32_t *getRastersForStr(char *str);
