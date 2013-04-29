#include <Windows.h>
#include <Windowsx.h>
#include <math.h>
#include <strsafe.h>
#include <Shlwapi.h>
#include "resource.h"
#include "ColorShop.h"
#include "ColorFunctions.h"
//#include <wingdi.h>

//#define PALETTEWIDTH	255.0f
//#define PALETTEHEIGHT	255.0f
//#define HUEHEIGHT		255.0f
//#define HUEWIDTH		 30.0f

HWND hwndColHueCtrl;
HWND hwndColPalCtrl;
HWND hwndColPrevCtrl;

HINSTANCE hInstance;

DWORD* dwArray;
BITMAPINFO bmInfo;
HBITMAP myBitmap;
HDC myCompatibleDC;
HWND hwndEdit, hwndHSB, hwndRGB, hwndHEX, hwndHASH, hwndParent;

HBITMAP arrow_left = NULL;
HBITMAP arrow_left_mask = NULL;

static int HorizMargin = 8;
static int VertMargin = 5;
static float PalleteSize = 255.0f;
static int HueWidth = 30;
int CurrentX, CurrentY;
float CurrentH, CurrentS, CurrentB;
wchar_t hex_value[80] = L"FF0000";
int bm_height;

wchar_t valid_chars[] = {
	L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', 
	L'a', L'b', L'c', L'd', L'e', L'f', 
	L'A', L'B', L'C', L'D', L'E', L'F', 0x16,
	//L'#',
	0x0008, 0x007f, // backspace, del
	0x0000
};


WNDPROC wpOrigEditProc;


/****************************************************************************
* is_valid_input()
*
*
****************************************************************************/
int is_valid_input(wchar_t str[]) {


	for (UINT i = 0; i < wcslen(str); ++i)
	{
		if (!wcschr(valid_chars, str[i])) {
			// is valid
			return 0;
		}
	}

	return 1;
}


/****************************************************************************
* is_valid_hexcode()
*
*
****************************************************************************/
int is_valid_hexcode(wchar_t str[]) {

	return ( (wcslen(str) == 6) && (is_valid_input(str)) );
}


/****************************************************************************
* EditSubclassProc()
*
* Subclass procedure
****************************************************************************/
LRESULT APIENTRY EditSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{ 

	switch(uMsg) 
	{

	case WM_CHAR:
		{
			wchar_t c = (wchar_t)wParam;

			GetDlgItemText(hwndParent, IDC_EDT_HEX, hex_value, 80);

			if (c == 0xd) { // ENTER key
				if (is_valid_hexcode(hex_value)) {
					SetColor(hex_value);
					UpdateFields(EDIT_HSB | EDIT_RGB);
				} else {
					MessageBox(hwnd, L"Please enter values between 000000 and FFFFFF", L"Invalid Color", MB_ICONINFORMATION);
					return 0;
				}

			} else if (!wcschr(valid_chars, c)) {
				return 0;
			} else {
				CallWindowProc(wpOrigEditProc, hwnd, uMsg, wParam, lParam);
				GetDlgItemText(hwndParent, IDC_EDT_HEX, hex_value, 80);
				if (is_valid_hexcode(hex_value)) {
					SetColor(hex_value);
					UpdateFields(EDIT_HSB | EDIT_RGB);
				} 
				return 1;
			}
		}
		break;
	}

	return CallWindowProc(wpOrigEditProc, hwnd, uMsg, wParam, lParam); 
} 


/****************************************************************************
* filter()
*
*
****************************************************************************/
//wchar_t* filter(wchar_t const src[])
//{
//	//wchar_t dest[80];
//	wchar_t* dest = (wchar_t*)malloc(sizeof(wchar_t) * 80);
//	//wchar_t filter_chars[] = L"24680";
//
//	size_t i = 0, iDest = 0;
//	for(i = 0 ; src[i] != L'\0' ; ++i)
//	{
//		if(wcschr(valid_chars, src[i])) {
//			dest[iDest++] = src[i];
//		}
//	}
//
//	dest[iDest] = L'\0';
//
//	return dest;
//}


/****************************************************************************
* trim()
*
* trims spaces and # around Hex color code
****************************************************************************/
wchar_t* trim(wchar_t const src[])
{
	wchar_t dest[32];
	//wchar_t* dest = (wchar_t*)malloc(sizeof(wchar_t) * 80);
	int i = 0;

	while(src[i] == L' ' || src[i] == L'#') {
		i++;
	}

	int j = wcslen(src);

	while(src[j - 1] == L' ') {
		j--;
	}

	int k;

	for (k = 0; i < j; i++) {
		dest[k++] = src[i];
	}

	dest[k] = L'\0';

	return dest;

}


/****************************************************************************
* wc2i()
*
* converts wide char to int
****************************************************************************/
int wc2i(wchar_t wc) {

	char c = (char) wc;

	if (c <= 0x39) {
		return (c - 0x30);
	} else if (c <= 0x46) {
		return (c - 55);
	} else {
		return (c - 87);
	}
}


/****************************************************************************
* SetColor()
*
*
****************************************************************************/
void SetColor(wchar_t const c[]) {

	byte r = wc2i(c[0]) << 4 | wc2i(c[1]);
	byte g = wc2i(c[2]) << 4 | wc2i(c[3]);
	byte b = wc2i(c[4]) << 4 | wc2i(c[5]);

	HSB hsb;

	rgb2hsb(r, b, g, &hsb);

	CurrentH = 360.0f - hsb.h;
	CurrentS = hsb.s / 100.0f;
	CurrentB = hsb.b / 100.0f;

	InvalidateRect(hwndColPalCtrl, NULL, TRUE);
	InvalidateRect(hwndColHueCtrl, NULL, TRUE);
	InvalidateRect(hwndColPrevCtrl, NULL, TRUE);

	//UpdateFields();


}

void Init(HWND hwnd) {

	CenterWindow(hwnd);

	arrow_left = NULL;
	arrow_left_mask = NULL;

	HorizMargin = 8;
	VertMargin = 5;
	PalleteSize = 255.0f;
	HueWidth = 30;
	CurrentX = PalleteSize;
	CurrentY = 0;
	CurrentH = 360.0f;
	CurrentS = 1.0f;
	CurrentB = 1.0f;

	//CurrentH, CurrentS, CurrentB;
	//hex_value[80] = L"FF0000";

	hwndHSB = GetDlgItem(hwnd, IDC_EDT_HSB);
	hwndRGB = GetDlgItem(hwnd, IDC_EDT_RGB);
	hwndHEX = GetDlgItem(hwnd, IDC_EDT_HEX);
	hwndHASH = GetDlgItem(hwnd, IDC_CHK_HASH);

	SendMessage(hwndHEX, EM_LIMITTEXT, 9, 0);

	// Subclass the edit control. 
	wpOrigEditProc = (WNDPROC) SetWindowLong(hwndHEX, GWL_WNDPROC, (LONG) EditSubclassProc); 

	//UpdateFields();

}


/****************************************************************************
* WndProc()
*
*
****************************************************************************/
INT_PTR CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	switch(uMsg)
	{

	case WM_INITDIALOG:
		Init(hwnd);
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
			//case IDC_CHK_HASH:
			//	{
			//		if (HIWORD(wParam) == BN_CLICKED)
			//		{
			//			UpdateFields();
			//		}
			//		break;
			//	}
			//	break;

			//case IDC_EDT_HEX:
			//	if (HIWORD(wParam) == EN_CHANGE) {
			//		wchar_t buf[32];

			//		GetDlgItemText(hwnd, IDC_EDT_HEX, buf, 32);

			//		if (GetCapture() != hwndColPalCtrl && GetCapture() != hwndColSelCtrl) {
			//			SetColor(buf);
			//		}
			//	}
			//	break;

			//case IDC_EDT_HEX:
			//	if (HIWORD (wParam) == EN_UPDATE) {
			//		// hex edit control updated
			//		wchar_t buf[32];

			//		GetDlgItemText(hwnd, IDC_EDT_HEX, buf, 32);

			//		if (is_valid_hexcode(buf)) {

			//			if (GetCapture() != hwndColPalCtrl && GetCapture() != hwndColSelCtrl) {
			//				SetColor(buf);
			//			}
			//		} //else {

			//		//wcscpy(buf, trim(buf));

			//		//if (is_valid_hexcode(buf)) {
			//		//	SetDlgItemText(hwnd, IDC_EDT_HEX, buf);
			//		//	SetColor(buf);
			//		//}
			//		//}
			//	}
			//	break;


		case IDOK:
		case IDCANCEL:
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			return TRUE;
		}
		break;


	case WM_CLOSE:
		DestroyWindow(hwnd);
		return TRUE;

	case WM_DESTROY:
		//PostQuitMessage(0);
		return TRUE;
	}

	return FALSE;
}


/****************************************************************************
* DrawPreview()
*
*
****************************************************************************/
void DrawPreview(HWND hwnd) 
{
	HDC hdc = GetDC(hwnd);


	RECT rect;
	GetClientRect(hwnd, &rect);
	int size = rect.right - rect.left;

	dwArray = (DWORD*) malloc(size * size * sizeof(DWORD));


	for (int b = 0; b < size; b++) {
		for (int s = 0; s < size; s++) {
			dwArray[s + (b * size)] = hsv2rgb(CurrentH / 360.0f, CurrentS, CurrentB); 
		}
	}


	ZeroMemory(&bmInfo, sizeof(BITMAPINFO));
	bmInfo.bmiHeader.biBitCount = 32;
	bmInfo.bmiHeader.biHeight = size;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth = size;
	bmInfo.bmiHeader.biCompression = BI_RGB;


	myBitmap = CreateCompatibleBitmap(GetDC(hwnd), size, size);
	if (myBitmap == NULL) {
		MessageBox(0, L"error1", 0,  0);
	}

	myCompatibleDC = CreateCompatibleDC(GetDC(hwnd));
	if (myCompatibleDC == NULL) {
		MessageBox(0, L"error2", 0,  0);
	}

	SelectObject(myCompatibleDC, myBitmap);

	int nResult = SetDIBits(myCompatibleDC, myBitmap, 0, size, dwArray, &bmInfo, 0);

	if (nResult == 0) {
		MessageBox(0, L"error3", 0,  0);
	}

	BitBlt(hdc, 0, 0, size, size, myCompatibleDC, 0, 0, SRCCOPY);   

	free(dwArray);
	DeleteObject(myBitmap);
	DeleteObject(myCompatibleDC);

}


/****************************************************************************
* DrawPalette()
*
*
****************************************************************************/
void DrawPalette(HWND hwnd) 
{
	HDC hdc = GetDC(hwnd);

	//int size;

	//size = PALETTEWIDTH;

	dwArray = (DWORD*) malloc(PalleteSize * PalleteSize * sizeof(DWORD));

	DWORD dw = COLORREF(RGB(0xff, 1, 0xfe));

	for (int b = 0; b < PalleteSize; b++) {
		for (int s = 0; s < PalleteSize; s++) {
			dwArray[s + (int)(b * PalleteSize)] = hsv2rgb(CurrentH / 360.0f, s / PalleteSize, b / PalleteSize); 
		}
	}

	//int offset;

	//for (int y = 0; y < 12; y++) {
	//	for (int x = 0; x < 12; x++) {
	//		offset = x + CurrentX - 6 + (int)(((PalleteSize - CurrentY) + y - 6) * PalleteSize);
	//		if (offset > 0 && offset < (PalleteSize * PalleteSize) && circle_array[x + y * 12] != 0x0) {
	//			dwArray[offset] = CurrentY < 120? 0:-1;
	//		}
	//	}
	//}


	//for (double i = 0.0; i < 360.0; i += 0.1)

	//{

	//	int radius = 20;
	//	float pi = 3.141597f;
	//	double angle = i * pi / 180;

	//	int x = (int)(CurrentX +  radius * cos(angle));

	//	int y = (int)(CurrentY + radius * sin(angle));

	//	int position = x + (int)(y * PalleteSize);

	//	if (position > PalleteSize * PalleteSize) position = 0;

	//	dwArray[position] = hsv2rgb(0, 0, 1);

	//	//PutPixel(myGraphics, x, y, Color.Red);

	//	//System.Threading.Thread.Sleep(1); // If you want to draw circle very slowly.

	//}


	//int x, y;
	//int r = 20;
	//int rx = 20;
	//int ry = 20;
	//int f = 5;

	//for(x=rx-r; x<=rx+r; x++) {
	//	for(y=ry-r; y<=ry+r; y++) {
	//		if ( (((x*x)+(y*y))>(r*r)-(f/2)) && (((x*x)+(y*y))<(r*r)+(f/2)) ) { 
	//			dwArray[x + (int)(y * HueWidth)] = hsv2rgb(0, 0, 1);
	//		}
	//	}
	//}



	//for (int x = rx - r; x <= rx + r; x++) {
	//		for (int y = ry - r; y <= ry + r; y++) {
	//			if (((x * x) + (y * y)) == (r * r)) {
	//				dwArray[x + (int)(y * PalleteSize)] = hsv2rgb(0, 0, 0);
	//			}
	//		}
	//	}

	ZeroMemory(&bmInfo, sizeof(BITMAPINFO));
	bmInfo.bmiHeader.biBitCount = 32;
	bmInfo.bmiHeader.biHeight = PalleteSize;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth = PalleteSize;
	bmInfo.bmiHeader.biCompression = BI_RGB;


	myBitmap = CreateCompatibleBitmap(GetDC(hwnd), PalleteSize, PalleteSize);
	if (myBitmap == NULL) {
		MessageBox(0, L"error1", 0,  0);
	}

	myCompatibleDC = CreateCompatibleDC(GetDC(hwnd));
	if (myCompatibleDC == NULL) {
		MessageBox(0, L"error2", 0,  0);
	}

	SelectObject(myCompatibleDC, myBitmap);

	int nResult = SetDIBits(myCompatibleDC, myBitmap, 0, PalleteSize, dwArray, &bmInfo, 0);

	if (nResult == 0) {
		MessageBox(0, L"error3", 0,  0);
	}

	BitBlt(hdc, 0, 0, PalleteSize, PalleteSize, myCompatibleDC, 0, 0, SRCCOPY); 

	//int ellipseSize = 40;
	//RECT ellipse;
	//ellipse.left = (long) (CurrentX - (ellipseSize / 2.0f));
	//ellipse.top = (long) (CurrentY - (ellipseSize / 2.0f));
	//ellipse.right = ellipse.left + ellipseSize;
	//ellipse.bottom = ellipse.top + ellipseSize;
	//SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));

	//if (CurrentY < 100) {
	//	SelectObject(hdc, GetStockObject(BLACK_PEN));
	//} else {
	//	SelectObject(hdc, GetStockObject(WHITE_PEN));
	//}

	//Ellipse(hdc, ellipse.left, ellipse.top, ellipse.right, ellipse.bottom); 





	free(dwArray);
	DeleteObject(myBitmap);
	DeleteObject(myCompatibleDC);

}


/****************************************************************************
* DrawColorSelector()
*
*
****************************************************************************/
void DrawHueSelector(HWND hwnd) 
{
	HDC hdc = GetDC(hwnd);

	RECT size;
	GetClientRect(hwnd, &size);

	RECT left = {0, 0, HorizMargin, size.bottom};

	int sizex, sizey;
	sizex = size.right - size.left;
	sizey = size.bottom - size.top;


	dwArray = (DWORD*) malloc(sizex * sizey * sizeof(DWORD));
	//ZeroMemory(&dwArray, sizeof(DWORD));

	// Convert COLOR_3DFACE to RGB values for drawing
	HBRUSH hBrush = GetSysColorBrush(COLOR_3DFACE);
	LOGBRUSH lb;
	GetObject(hBrush,sizeof(LOGBRUSH),(LPSTR)&lb);
	COLORREF bgCol = RGB(GetRValue(lb.lbColor), GetGValue(lb.lbColor), GetBValue(lb.lbColor));

	memset(dwArray, bgCol, sizeof(COLORREF) * sizex * sizey);

	for (int y = VertMargin; y < PalleteSize + VertMargin; y++) {
		for (int x = HorizMargin; x < HueWidth + HorizMargin; x++) {
			dwArray[x + (y * sizex)] = hsv2rgb((y - VertMargin)/PalleteSize, 1.0f, 1.0f);
		}
	}



	//for (int y = 0; y < sizey; y++) {
	//	for (int x = 0; x < sizex; x++) {
	//		if (y < VertMargin - 1 || y >= PalleteSize + VertMargin || x < HorizMargin || x > HueWidth + HorizMargin) {
	//			//dwArray[x + (y * sizex)] = bgCol;
	//		} else {
	//			//dwArray[x + (y * sizex)] = hsv2rgb(360.0f * (float)y / sizey, 1.0f, 1.0f);
	//			dwArray[x + (y * sizex)] = hsv2rgb((float)y * 360.0f / PalleteSize, 1.0f, 1.0f);
	//		}
	//	}
	//}

	ZeroMemory(&bmInfo, sizeof(BITMAPINFO));
	bmInfo.bmiHeader.biBitCount = 32;
	bmInfo.bmiHeader.biWidth = sizex;
	bmInfo.bmiHeader.biHeight = sizey;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biCompression = BI_RGB;

	myBitmap = CreateCompatibleBitmap(GetDC(hwnd), sizex, sizey);
	if (myBitmap == NULL) {
		MessageBox(0, L"error1", 0,  0);
	}

	myCompatibleDC = CreateCompatibleDC(hdc);
	if (myCompatibleDC == NULL) {
		MessageBox(0, L"error2", 0,  0);
	}

	SelectObject(myCompatibleDC, myBitmap);

	int nResult = SetDIBits(myCompatibleDC, myBitmap, 0, sizey, dwArray, &bmInfo, 0);

	if (nResult == 0) {
		MessageBox(0, L"error3", 0,  0);
	}

	//BitBlt(hdc, horizMargin, vertMargin, sizex, sizey, myCompatibleDC, 0, 0, SRCCOPY);   
	BitBlt(hdc, 0, 0, sizex, sizey, myCompatibleDC, 0, 0, SRCCOPY);   

	free(dwArray);
	DeleteObject(myBitmap);
	DeleteObject(myCompatibleDC);
	ReleaseDC(hwnd, myCompatibleDC);
	ReleaseDC(hwnd, hdc);


}


/****************************************************************************
* UpdateFields()
*
*
****************************************************************************/
void UpdateFields(DWORD flags) 
{

	//int h,s,v,r,g,b;
	RGBQUAD quad;
	wchar_t buffer[64];
	LPWSTR hash = L"";

	int currenth = (int)(CurrentH + 0.5);

	hsv2rgb_quad(currenth / 360.0f, CurrentS, CurrentB, &quad);

	currenth = currenth==360?0:currenth;

	if (flags & EDIT_HSB) {
		StringCchPrintf(buffer, sizeof(buffer), L"%d, %d, %d", currenth, (int)(CurrentS * 100.0f), (int)(CurrentB * 100.0f));
		SendMessage(hwndHSB, WM_SETTEXT, NULL, (LPARAM)buffer);
	}

	if (flags & EDIT_RGB) {
		StringCchPrintf(buffer, sizeof(buffer), L"%d, %d, %d", quad.rgbRed, quad.rgbGreen, quad.rgbBlue);
		SendMessage(hwndRGB, WM_SETTEXT, NULL, (LPARAM)buffer);
	}

	if (flags & EDIT_HEX) {
		StringCchPrintf(buffer, sizeof(buffer), L"%s%02x%02x%02x", hash, quad.rgbRed, quad.rgbGreen, quad.rgbBlue);
		SendMessage(hwndHEX, WM_SETTEXT, NULL, (LPARAM)buffer);
	}

}


/****************************************************************************
* ColPalWndProc()
*
*
****************************************************************************/
LRESULT CALLBACK ColPalWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	PAINTSTRUCT ps;          // paint data for BeginPaint and EndPaint  
	static HDC hdcCompat;

	switch(msg)
	{

	case WM_CREATE:
		{
			HCURSOR hCurs = LoadCursor(hInstance, MAKEINTRESOURCE(102));
			SetClassLong(hwnd, GCL_HCURSOR, (LONG) hCurs);   // new cursor 
		}
		break;

	case WM_LBUTTONUP:
		ReleaseCapture();
		break;

	case WM_MOUSEMOVE:
		if (wParam ^ MK_LBUTTON) {
			break;
		}

	case WM_LBUTTONDOWN:
		{
			int xPos = GET_X_LPARAM(lParam); 
			int yPos = GET_Y_LPARAM(lParam);

			SetCapture(hwnd);

			if (xPos < 0) xPos = 0;
			if (xPos > PalleteSize) xPos = PalleteSize;
			if (yPos < 0) yPos = 0;
			if (yPos > PalleteSize) yPos = PalleteSize;

			CurrentX = xPos;
			CurrentY = yPos;
			CurrentS = xPos / PalleteSize;
			CurrentB = (PalleteSize - yPos) / PalleteSize;

			UpdateFields(EDIT_ALL);

			InvalidateRect(hwndColPalCtrl, NULL, TRUE);
			InvalidateRect(hwndColPrevCtrl, NULL, TRUE);

			//SendMessage(hwnd, WM_ERASEBKGND, 0, 0);
			//InvalidateRect(hwnd, NULL, TRUE);
			//UpdateWindow(hwnd);
			//free(buffer);
			//return 1;
		}
		break;


	case WM_PAINT:
		{
			// draw color indicator
			BeginPaint(hwnd, &ps);
			int ellipseSize = 10;

			RECT ellipse;
			ellipse.left = (long) (CurrentX - (ellipseSize / 2.0f));
			ellipse.top = (long) (CurrentY - (ellipseSize / 2.0f));
			ellipse.right = ellipse.left + ellipseSize;
			ellipse.bottom = ellipse.top + ellipseSize;
			SelectObject(ps.hdc, GetStockObject(HOLLOW_BRUSH));

			if (CurrentY < 100) {
				SelectObject(ps.hdc, GetStockObject(BLACK_PEN));
			} else {
				SelectObject(ps.hdc, GetStockObject(WHITE_PEN));
			}

			Ellipse(ps.hdc, ellipse.left, ellipse.top, ellipse.right, ellipse.bottom); 

			EndPaint(hwnd, &ps);
			//return 1;

		}
		break;


	case WM_ERASEBKGND:
		DrawPalette(hwnd);

		return 1;

	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


/****************************************************************************
* ColSelWndProc()
*
*
****************************************************************************/
LRESULT CALLBACK ColHueWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;          // paint data for BeginPaint and EndPaint  
	static HDC hdcCompat;
	switch(msg)
	{

	case WM_CREATE:
		arrow_left = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_ARROWL));
		//arrow_left_mask = CreateBitmapMask(arrow_left, RGB(0, 0, 0));

		if(arrow_left == NULL)
			MessageBox(hwnd, L"Could not load IDB_ARROWL!", L"Error", MB_OK | MB_ICONEXCLAMATION);

		break;

	case WM_MOUSEMOVE:
		if (wParam ^ MK_LBUTTON) {
			break;
		}

	case WM_LBUTTONDOWN:
		{
			int xPos = GET_X_LPARAM(lParam); 
			int yPos = GET_Y_LPARAM(lParam) - VertMargin;

			SetCapture(hwnd);

			if (yPos < 0) yPos = 0;
			if (yPos > PalleteSize) yPos = PalleteSize;


			CurrentH =  (255 - yPos) * 360.0f / PalleteSize;

			UpdateFields(EDIT_ALL);

			InvalidateRect(hwndColPalCtrl, NULL, TRUE);
			InvalidateRect(hwndColPrevCtrl, NULL, TRUE);
			InvalidateRect(hwndColHueCtrl, NULL, TRUE);

		}
		break;

	case WM_LBUTTONUP:
		ReleaseCapture();
		break;

	case WM_PAINT:
		{
			//RECT rcClient;
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			DrawArrows(hdc);
			EndPaint(hwnd, &ps);


			//// draw color indicator
			//BeginPaint(hwnd, &ps);
			//HBRUSH hBrush;

			////HDC hdc = GetDC(hwnd);
			//RECT rLeft, rRight;
			//rLeft.left = 0;
			//rRight.left = HueWidth + horizMargin;
			//rLeft.top = rRight.top = (int)(vertMargin + col * PalleteSize / 360.0);
			//rLeft.right = rLeft.left + horizMargin;
			//rRight.right = rRight.left + horizMargin;
			//rLeft.bottom = rRight.bottom = rLeft.top + 1;

			//hBrush = CreateSolidBrush(RGB(0,0,0));
			////FrameRect (hdc, &r, hBrush);
			//FillRect (ps.hdc, &rLeft, hBrush);
			//FillRect (ps.hdc, &rRight, hBrush);
			//DeleteObject (hBrush);
			////DeleteDC(hdc);

			//EndPaint(hwnd, &ps);

		}
		break;

	case WM_ERASEBKGND:
		{
			//HDC hdc = (HDC) wParam; 
			//HDC hdc = GetDC(hwnd); 
			//RECT rc;
			//GetClientRect(hwnd, &rc); 
			//SetMapMode(hdc, MM_ANISOTROPIC); 
			//SetWindowExtEx(hdc, 100, 100, NULL); 
			//SetViewportExtEx(hdc, rc.right, rc.bottom, NULL); 
			//HBRUSH hBrush;
			//hBrush = GetSysColorBrush(COLOR_3DFACE);

			//FillRect(hdc, &rc, hBrush); 
			//DeleteObject(hBrush);
			DrawHueSelector(hwnd);
		}
		return 1;


	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


/****************************************************************************
* ColPrevWndProc()
*
*
****************************************************************************/
LRESULT CALLBACK ColPrevWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{


	case WM_ERASEBKGND:
		DrawPreview(hwnd);
		return 1;

	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


/****************************************************************************
* RegisterCustomClass()
*
*
****************************************************************************/
void RegisterCustomClass(TCHAR* szClassName, WNDPROC wndProc) {

	WNDCLASSEX wc;

	wc.cbSize         = sizeof(wc);
	wc.lpszClassName  = szClassName;
	wc.hInstance      = GetModuleHandle(0);
	wc.lpfnWndProc    = wndProc;
	wc.hCursor        = LoadCursor (NULL, IDC_ARROW);
	wc.hIcon          = 0;
	wc.lpszMenuName   = 0;
	wc.hbrBackground  = (HBRUSH)GetSysColorBrush(COLOR_BTNFACE);
	wc.style          = 0;
	wc.cbClsExtra     = 0;
	wc.cbWndExtra     = 0;
	wc.hIconSm        = 0;

	RegisterClassEx(&wc);

}


/****************************************************************************
* CreateColorShopDlg
*
*
****************************************************************************/
void CreateColorShopDlg(HINSTANCE hInst, HWND hWnd) 
{
	HWND hDlg;

	hInstance = hInst;

	TCHAR szColPalClassName[] = L"ColPalClass";
	TCHAR szColHueClassName[] = L"ColSelClass";
	TCHAR szColPrevClassName[] = L"ColPreviewClass";


	RegisterCustomClass(szColPalClassName, ColPalWndProc);
	RegisterCustomClass(szColHueClassName, ColHueWndProc);
	RegisterCustomClass(szColPrevClassName, ColPrevWndProc);


	hDlg = CreateDialog(hInst, MAKEINTRESOURCE (IDD_DLG1), hWnd, WndProc);
	if (hDlg == NULL) {
		MessageBox(hWnd, L"Could not create dialog", 0, 0);
		return;
	}

	hwndColPalCtrl = CreateWindowEx(
		WS_EX_STATICEDGE,
		szColPalClassName,
		L"A custom control",
		WS_VISIBLE | WS_CHILD,
		10, 10, PalleteSize, PalleteSize,
		hDlg,
		NULL, GetModuleHandle(0), NULL
		);

	hwndColHueCtrl = CreateWindowEx(
		0,
		szColHueClassName,
		L"A custom control",
		WS_VISIBLE | WS_CHILD,
		//270, 10 - vertMargin, HueWidth + 20, PalleteSize + vertMargin + vertMargin,
		270, 10 - VertMargin, HueWidth + HorizMargin + HorizMargin, PalleteSize + VertMargin + VertMargin,
		hDlg,
		NULL, GetModuleHandle(0), NULL
		);

	hwndColPrevCtrl = CreateWindowEx(
		WS_EX_STATICEDGE,
		szColPrevClassName,
		L"A custom control",
		WS_VISIBLE | WS_CHILD,
		330, 10, 100, 100,
		hDlg,
		NULL, GetModuleHandle(0), NULL
		);

	//ShowWindow(hwndCtrl, 1);
	//UpdateWindow(hwndCtrl);
	hwndParent = hDlg;

	ShowWindow(hDlg, SW_SHOW);

}

/****************************************************************************
* CenterWindow()
*
* positions dialog central to parent window
****************************************************************************/
int CenterWindow(HWND hwndWindow)
{
	HWND hwndParent;
	RECT rectWindow, rectParent;

	// make the window relative to its parent
	if ((hwndParent = GetParent(hwndWindow)) != NULL)
	{
		GetWindowRect(hwndWindow, &rectWindow);
		GetWindowRect(hwndParent, &rectParent);

		int nWidth = rectWindow.right - rectWindow.left;
		int nHeight = rectWindow.bottom - rectWindow.top;

		int nX = ((rectParent.right - rectParent.left) - nWidth) / 2 + rectParent.left;
		int nY = ((rectParent.bottom - rectParent.top) - nHeight) / 4 + rectParent.top; // higher than center

		int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

		// make sure that the dialog box never moves outside of the screen
		if (nX < 0) nX = 0;
		if (nY < 0) nY = 0;
		if (nX + nWidth > nScreenWidth) nX = nScreenWidth - nWidth;
		if (nY + nHeight > nScreenHeight) nY = nScreenHeight - nHeight;

		MoveWindow(hwndWindow, nX, nY, nWidth, nHeight, FALSE);

		return 1;
	}

	return 0;
}

void DrawArrows(HDC hdc)
{
	BITMAP bm;

	GetObject(arrow_left, sizeof(bm), &bm);

	HDC hdcBuffer = CreateCompatibleDC(hdc);
	//HDC hdcReverse = CreateCompatibleDC(hdc);
	HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, bm.bmWidth, bm.bmHeight);
	HBITMAP hbmOldBuffer = (HBITMAP)SelectObject(hdcBuffer, hbmBuffer);

	HDC hdcMem = CreateCompatibleDC(hdc);


	SelectObject(hdcMem, arrow_left);
	BitBlt(hdcBuffer, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCPAINT);
	//GdiTransparentBlt(hdc, 0, (int)(vertMargin + col * (PalleteSize / 360.0f)), bm.bmWidth, bm.bmHeight, hdcBuffer, 0, 0, bm.bmWidth, bm.bmHeight, RGB(0,0xff,0));
	GdiTransparentBlt(hdc, 0, (int)((360.0f - CurrentH) * (PalleteSize / 360.0f) + (VertMargin * 2 - bm.bmHeight)), bm.bmWidth, bm.bmHeight, hdcBuffer, 0, 0, bm.bmWidth, bm.bmHeight, RGB(0,0xff,0));

	DeleteDC(hdcMem);

	SelectObject(hdcBuffer, hbmOldBuffer);
	DeleteDC(hdcBuffer);
	DeleteObject(hbmBuffer);

}