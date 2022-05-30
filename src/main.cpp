#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <windowsx.h>
#include <iostream>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

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
		{
			int mouseX = GET_X_LPARAM(lParam);
			int mouseY = GET_Y_LPARAM(lParam);
			printf("Mouse pos: (%4d, %4d)\r", mouseX, mouseY);
			break;
		}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

int main()
{
	//https://docs.microsoft.com/pt-br/windows/win32/learnwin32/your-first-windows-program
	HINSTANCE hInstance = (HINSTANCE)&__ImageBase;
	WNDCLASSA wndClass = {};
	const char *className = "EhDraw Main Window Class";
	wndClass.lpszClassName = className;
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = WndProc;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);

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
		return -1;
	}

	HWND hwnd = CreateWindowA(className, "EhDraw", WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
		NULL, NULL, hInstance, NULL);
	if(hwnd == NULL)
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
		return -1;	
	}

	ShowWindow(hwnd, SW_SHOW);

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// if(DestroyWindow(hwnd) == 0)
	// {
	// 	DWORD error = GetLastError(); 
	// 	LPSTR errorStr;
	// 	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | 
	// 					FORMAT_MESSAGE_IGNORE_INSERTS |
	// 					FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, 
	// 					error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
	// 					(LPTSTR)&errorStr, 0, NULL);
	// 	fprintf(stderr, "Failed to destroy window (Error %lu):\n%s", error, errorStr);
	// 	LocalFree(errorStr);
	// 	return -1;	
	// }
}