#include <Windows.h>


typedef struct {
  float h;
  float s;
  float b;
} HSB;

void rgb2hsb(BYTE r, BYTE g, BYTE b, HSB* hsb);

HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent);
//void hsv2rgb_quad(float hue, float saturation, float brightness, RGBQUAD* quad);
//void HSVtoRGB(float h, float s, float b, RGBQUAD* quad);
COLORREF hsv2rgb(float hue, float saturation, float brightness);
void hsv2rgb_quad(float h, float s, float b, RGBQUAD* quad );
