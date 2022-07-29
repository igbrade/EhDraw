#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <windowsx.h>

struct win32Window
{
	HWND hwnd;
	HDC hdc;
};

bool win32Init();
bool win32createWindow(win32Window *w);
bool win32SetupGlContext(win32Window *w);
void win32ShowWindow(win32Window *w);
