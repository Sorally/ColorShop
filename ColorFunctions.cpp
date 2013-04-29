#include "ColorFunctions.h"
#include <math.h>


/****************************************************************************
* rgb2hsb()
*
* 
****************************************************************************/
void rgb2hsb(BYTE r, BYTE g, BYTE b, HSB* hsb)
{
	float x, f, i, hue, sat, bright;
	
	x = min( min(r, g), b );
	bright = max( max( r, g ), b );

	if (x == bright) {
		hsb->h = 0; 
		hsb->s = 0;
		hsb->b = bright * 100.0f / 255.0f;
		return;
	}

	f = (r == x) ? g - b : ((g == x) ? b - r : r - g);
	i = (r == x) ? 3 : ((g == x) ? 5 : 1);
	
	hsb->h = (int)floor((i - f / (bright - x)) * 60) % 360;
	hsb->s = floor(((bright - x) / bright) * 100);
	hsb->b = bright * 100.0f / 255.0f;

}



/****************************************************************************
* hsv2rgb()
*
*
****************************************************************************/
COLORREF hsv2rgb(float hue, float saturation, float brightness) 
{
	RGBQUAD quad;

	//hsv2rgb_quad(hue, saturation, brightness, &quad);
	hsv2rgb_quad(hue, saturation, brightness, &quad);

	// reverse the buffer contents from RGB to BGR
	return RGB((int)(quad.rgbBlue), (int)(quad.rgbGreen), (int)(quad.rgbRed));

}
//
//void hsv2rgb_quad(float hue, float saturation, float brightness, RGBQUAD* quad) {
//
//	if (hue < 0.0f || hue > 360.0f || saturation < 0.0f || saturation > 1.0f ||	brightness < 0.0f || brightness > 1.0f) {
//		MessageBox(0, L"hsv2rgb error", 0, 0);
//		return;
//	}
//
//	float r, g, b;
//	if (saturation == 0) {
//		r = g = b = brightness;
//	} else {
//		if (hue == 360) hue = 0;
//		hue /= 60;
//		int i = (int)hue;
//		float f = hue - i;
//		float p = brightness * (1 - saturation);
//		float q = brightness * (1 - saturation * f);
//		float t = brightness * (1 - saturation * (1 - f));
//		switch(i) {
//		case 0:
//			r = brightness;
//			g = t;
//			b = p;
//			break;
//		case 1:
//			r = q;
//			g = brightness;
//			b = p;
//			break;
//		case 2:
//			r = p;
//			g = brightness;
//			b = t;
//			break;
//		case 3:
//			r = p;
//			g = q;
//			b = brightness;
//			break;
//		case 4:
//			r = t;
//			g = p;
//			b = brightness;
//			break;
//		case 5:
//		default:
//			r = brightness;
//			g = p;
//			b = q;
//			break;
//		}
//	}
//
//	quad->rgbRed = r * 0xff;
//	quad->rgbGreen = g * 0xff;
//	quad->rgbBlue = b * 0xff;
//
//
//}


/****************************************************************************
* hsv2rgb_quad()
*
*
****************************************************************************/
//void hsv2rgb_quad(float hue, float saturation, float brightness, RGBQUAD* quad) 
//{
//	if (hue < 0 || hue > 360 || saturation < 0 || saturation > 1 ||	brightness < 0 || brightness > 1) {
//		MessageBox(0, L"hsv2rgb error", 0, 0);
//	}
//
//	float r, g, b;
//	if (saturation == 0) {
//		r = g = b = brightness;
//	} else {
//		if (hue == 360) hue = 0;
//		hue /= 60;
//		int i = (int)hue;
//		float f = hue - i;
//		float p = brightness * (1 - saturation);
//		float q = brightness * (1 - saturation * f);
//		float t = brightness * (1 - saturation * (1 - f));
//		switch(i) {
//		case 0:
//			r = brightness;
//			g = t;
//			b = p;
//			break;
//		case 1:
//			r = q;
//			g = brightness;
//			b = p;
//			break;
//		case 2:
//			r = p;
//			g = brightness;
//			b = t;
//			break;
//		case 3:
//			r = p;
//			g = q;
//			b = brightness;
//			break;
//		case 4:
//			r = t;
//			g = p;
//			b = brightness;
//			break;
//		case 5:
//		default:
//			r = brightness;
//			g = p;
//			b = q;
//			break;
//		}
//	}
//
//	//// reverse the buffer contents from RGB to BGR
//	quad->rgbBlue = (BYTE)(b * 255 + 0.5);
//	quad->rgbGreen = (BYTE)(g * 255 + 0.5);
//	quad->rgbBlue = (BYTE)(r * 255 + 0.5);
//
//}


/****************************************************************************
* HSVtoRGB()
*
*
****************************************************************************/
void hsv2rgb_quad(float h, float s, float b, RGBQUAD* quad )
{
	int         i;
	double      f;
	double      bb;
	u_char      p;
	u_char      q;
	u_char      t;

	h -= floor(h);		/* remove anything over 1 */
	h *= 6.0;
	i = (int)floor(h);		/* 0..5 */
	f = h - (float) i;		/* f = fractional part of H */
	bb = 255.0 * b;
	p = (u_char) (bb * (1.0 - s));
	q = (u_char) (bb * (1.0 - (s * f)));
	t = (u_char) (bb * (1.0 - (s * (1.0 - f))));
	switch (i) {
	case 0:
		quad->rgbRed = (u_char) bb;
		quad->rgbGreen = t;
		quad->rgbBlue = p;
		break;
	case 1:
		quad->rgbRed = q;
		quad->rgbGreen = (u_char) bb;
		quad->rgbBlue = p;
		break;
	case 2:
		quad->rgbRed = p;
		quad->rgbGreen = (u_char) bb;
		quad->rgbBlue = t;
		break;
	case 3:
		quad->rgbRed = p;
		quad->rgbGreen = q;
		quad->rgbBlue = (u_char) bb;
		break;
	case 4:
		quad->rgbRed = t;
		quad->rgbGreen = p;
		quad->rgbBlue = (u_char) bb;
		break;
	case 5:
		quad->rgbRed = (u_char) bb;
		quad->rgbGreen = p;
		quad->rgbBlue = q;
		break;
	}
}


HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent)
{
	HDC hdcMem, hdcMem2;
	HBITMAP hbmMask;
	BITMAP bm;

	GetObject(hbmColour, sizeof(BITMAP), &bm);
	hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

	hdcMem = CreateCompatibleDC(0);
	hdcMem2 = CreateCompatibleDC(0);

	SelectObject(hdcMem, hbmColour);
	SelectObject(hdcMem2, hbmMask);

	SetBkColor(hdcMem, crTransparent);

	BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

	BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);

	DeleteDC(hdcMem);
	DeleteDC(hdcMem2);

	return hbmMask;
}