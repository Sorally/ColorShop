void CreateColorShopDlg(HINSTANCE, HWND);
//COLORREF hsv2rgb(float, float, float);
//void hsv2rgb_quad(float, float, float, RGBQUAD*);
//void HSVtoRGB(float h, float s, float v, RGBQUAD* quad );
void UpdateFields(DWORD flags);
BOOL CenterWindow(HWND hwndWindow);
void DrawPalette(HWND hWnd);
void DrawArrows(HDC hdc);
void SetColor(wchar_t const c[]);
void Init(HWND hwnd);


#define EDIT_HEX 1
#define EDIT_RGB 2
#define EDIT_HSB 4
#define EDIT_ALL 7
