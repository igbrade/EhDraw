#include "win32window.hpp"
#include <cstdio>

//https://devblogs.microsoft.com/oldnewthing/20041025-00/?p=37483
EXTERN_C IMAGE_DOS_HEADER __ImageBase;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static const char *className = "EhDraw Main Window Class";

void (*drawCallback)();
void (*mouseMoveCallback)(int mouseX, int mouseY);
void (*mouseDownCallback)(int mouseX, int mouseY);
void (*mouseUpCallback)(int mouseX, int mouseY);
void (*resizeCallback)(int newWidth, int newHeight);
void (*keyDownCallback)(int key);
void (*keyUpCallback)(int key);
void (*mouseWheelCallback)(int wheelDelta);

bool win32Init()
{
	HINSTANCE hInstance = (HINSTANCE)&__ImageBase;
	WNDCLASSA wndClass = {};
	wndClass.lpszClassName = className;
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = WndProc;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	if(RegisterClassA(&wndClass) == 0)
	{
		DWORD error = GetLastError(); 
		LPSTR errorStr;
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | 
						FORMAT_MESSAGE_IGNORE_INSERTS |
						FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, 
						error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
						(LPTSTR)&errorStr, 0, NULL);
		fprintf(stderr, "Failed to register window class (Error %lu):\n%s", error, errorStr);
		LocalFree(errorStr);
		return false;
	}
	return true;
}

bool win32createWindow(win32Window *w)
{
	//https://docs.microsoft.com/pt-br/windows/win32/learnwin32/your-first-windows-program
	HINSTANCE hInstance = (HINSTANCE)&__ImageBase;
	w->hwnd = CreateWindowA(className, "EhDraw", WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
		NULL, NULL, hInstance, NULL);
	if(w->hwnd == NULL)
	{
		DWORD error = GetLastError(); 
		LPSTR errorStr;
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | 
						FORMAT_MESSAGE_IGNORE_INSERTS |
						FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, 
						error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
						(LPTSTR)&errorStr, 0, NULL);
		fprintf(stderr, "Failed to create window (Error %lu):\n%s", error, errorStr);
		LocalFree(errorStr);
		return false;	
	}
	//https://docs.microsoft.com/en-us/windows/win32/gdi/display-device-contexts
	//Using OWNDC
	w->hdc = GetDC(w->hwnd);
	return true;
}

bool win32SetupGlContext(win32Window *w)
{
	PIXELFORMATDESCRIPTOR pfd = {};
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;

	int pfdIndex = ChoosePixelFormat(w->hdc, &pfd);
	if(pfdIndex == 0)
	{
		DWORD error = GetLastError(); 
		LPSTR errorStr;
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | 
						FORMAT_MESSAGE_IGNORE_INSERTS |
						FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, 
						error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
						(LPTSTR)&errorStr, 0, NULL);
		fprintf(stderr, "Failed to choose pixel format (Error %lu):\n%s", error, errorStr);
		LocalFree(errorStr);
		return false;
	}
	if(SetPixelFormat(w->hdc, pfdIndex, &pfd) == FALSE)
	{
		DWORD error = GetLastError(); 
		LPSTR errorStr;
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | 
						FORMAT_MESSAGE_IGNORE_INSERTS |
						FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, 
						error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
						(LPTSTR)&errorStr, 0, NULL);
		fprintf(stderr, "Failed to choose pixel format (Error %lu):\n%s", error, errorStr);
		LocalFree(errorStr);
		return false;	
	}

	HGLRC glContext = wglCreateContext(w->hdc);
	if(glContext == NULL)
	{
		DWORD error = GetLastError(); 
		LPSTR errorStr;
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | 
						FORMAT_MESSAGE_IGNORE_INSERTS |
						FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, 
						error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
						(LPTSTR)&errorStr, 0, NULL);
		fprintf(stderr, "Failed to create GL context (Error %lu):\n%s", error, errorStr);
		LocalFree(errorStr);
		return false;	
	}

	if(wglMakeCurrent(w->hdc, glContext) == FALSE)
	{
		DWORD error = GetLastError(); 
		LPSTR errorStr;
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | 
						FORMAT_MESSAGE_IGNORE_INSERTS |
						FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, 
						error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
						(LPTSTR)&errorStr, 0, NULL);
		fprintf(stderr, "Failed to make current GL context (Error %lu):\n%s", error, errorStr);
		LocalFree(errorStr);
		return false;
	}

	return true;
}

void win32ShowWindow(win32Window *w)
{
	ShowWindow(w->hwnd, SW_SHOW);
}

void win32DestroyWindow(win32Window *w)
{
	if(DestroyWindow(w->hwnd) == 0)
	{
		DWORD error = GetLastError(); 
		LPSTR errorStr;
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | 
						FORMAT_MESSAGE_IGNORE_INSERTS |
						FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, 
						error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
						(LPTSTR)&errorStr, 0, NULL);
		fprintf(stderr, "Failed to destroy window (Error %lu):\n%s", error, errorStr);
		LocalFree(errorStr);	
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
			break;
		}
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		{
			int mouseX = GET_X_LPARAM(lParam);
			int mouseY = GET_Y_LPARAM(lParam);
			if(msg == WM_MOUSEMOVE && mouseMoveCallback != NULL)
				mouseMoveCallback(mouseX, mouseY);
			else if(msg == WM_LBUTTONDOWN && mouseDownCallback != NULL)
				mouseDownCallback(mouseX, mouseY);
			else if(msg == WM_LBUTTONUP && mouseUpCallback != NULL)
				mouseUpCallback(mouseX, mouseY);
			PostMessage(hwnd, WM_PAINT, 0, 0);
			break;
		}
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			if(msg == WM_KEYDOWN && keyDownCallback != NULL)
				keyDownCallback(wParam);
			if(msg == WM_KEYUP && keyUpCallback != NULL)
				keyUpCallback(wParam);
			break;
		}
		case WM_PAINT:
		{
			if(drawCallback != NULL)
				drawCallback();
			break;
		}
		case WM_SIZE:
		{
			int width = LOWORD(lParam);
			int height = HIWORD(lParam);
			if(resizeCallback != NULL)
				resizeCallback(width, height);
			break;
		}
		case WM_MOUSEWHEEL:
		{
			short delta = GET_WHEEL_DELTA_WPARAM(wParam);
			if(mouseWheelCallback != NULL)
				mouseWheelCallback((int)delta);
			break;
		}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
