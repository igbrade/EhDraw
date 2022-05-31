#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <wingdi.h>
#include <windowsx.h>
#include <GL/gl.h>
#include <iostream>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

void draw()
{
	glClearColor(0, 0, 0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-0.5, -0.5);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(0.5, -0.5);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(0.5, 0.5);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(-0.5, 0.5);
	glEnd();

	glFlush();
}

unsigned int texture;
unsigned int canvasWidth = 128;
unsigned int canvasHeight = 128;
unsigned int canvasRowSz = 4 * ((canvasWidth * 3 + 3) / 4);
unsigned int canvasBytes = canvasRowSz * canvasHeight;
unsigned char *pixels;

int windowWidth;
int windowHeight;

bool mouseDown = false;

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
		case WM_SIZE:
		{
			int width = LOWORD(lParam);
			int height = HIWORD(lParam);
			// printf("Window size: (%4d, %4d)\n", width, height);
			glViewport(0, 0, width, height);
			windowWidth = width;
			windowHeight = height;
			break;
		}
		case WM_MOUSEMOVE:
		{
			int mouseX = GET_X_LPARAM(lParam);
			int mouseY = GET_Y_LPARAM(lParam);
			// printf("Mouse pos: (%4d, %4d)\r", mouseX, mouseY);
			if(mouseDown)
			{
				float normalizedX = 2.0 * mouseX / windowWidth - 1.0;
				float normalizedY = 2.0 * mouseY / windowHeight - 1.0;

			if(-0.5 <= normalizedX && normalizedX <= 0.5)
				if(-0.5 <= normalizedY && normalizedY <= 0.5)
				{
					float textureCoordX = normalizedX - (-0.5); 
					float textureCoordY = normalizedY - (-0.5);

					textureCoordY = 1 - textureCoordY;

					int pixelX = canvasWidth * textureCoordX;
					int pixelY = canvasHeight * textureCoordY;

					int pixelPos = canvasRowSz * pixelY + pixelX * 3;
					pixels[pixelPos] = 0;
					pixels[pixelPos + 1] = 0;
					pixels[pixelPos + 2] = 0;

					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, canvasWidth, canvasHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
					printf("Clicked on: %f %f\n", textureCoordX, textureCoordY);

					PostMessage(hwnd, WM_PAINT, 0, 0);
				}
			}
			break;
		}
		case WM_LBUTTONDOWN:
		{
			mouseDown = true;
			int mouseX = GET_X_LPARAM(lParam);
			int mouseY = GET_Y_LPARAM(lParam);
			float normalizedX = 2.0 * mouseX / windowWidth - 1.0;
			float normalizedY = 2.0 * mouseY / windowHeight - 1.0;

			if(-0.5 <= normalizedX && normalizedX <= 0.5)
				if(-0.5 <= normalizedY && normalizedY <= 0.5)
				{
					float textureCoordX = normalizedX - (-0.5); 
					float textureCoordY = normalizedY - (-0.5);

					textureCoordY = 1 - textureCoordY;

					int pixelX = canvasWidth * textureCoordX;
					int pixelY = canvasHeight * textureCoordY;

					int pixelPos = canvasRowSz * pixelY + pixelX * 3;
					pixels[pixelPos] = 0;
					pixels[pixelPos + 1] = 0;
					pixels[pixelPos + 2] = 0;

					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, canvasWidth, canvasHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
					printf("Clicked on: %f %f\n", textureCoordX, textureCoordY);

					PostMessage(hwnd, WM_PAINT, 0, 0);
				}


			break;
		}
		case WM_LBUTTONUP:
		{
			mouseDown = false;
			break;
		}
		case WM_PAINT:
		{
			draw();
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

	//https://docs.microsoft.com/en-us/windows/win32/gdi/display-device-contexts
	HDC hdc = GetDC(hwnd);

	PIXELFORMATDESCRIPTOR pfd = {};
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;

	int pfdIndex = ChoosePixelFormat(hdc, &pfd);
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
		return -1;
	}
	if(SetPixelFormat(hdc, pfdIndex, &pfd) == FALSE)
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
		return -1;	
	}

	HGLRC glContext = wglCreateContext(hdc);
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
		return -1;	
	}

	if(wglMakeCurrent(hdc, glContext) == FALSE)
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
		return -1;
	}

	ShowWindow(hwnd, SW_SHOW);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	pixels = new unsigned char[canvasBytes];
	for(int i = 0; i < canvasHeight; ++i)
		for(int j = 0; j < canvasWidth; ++j)
		{
			int pos = i * canvasRowSz + j * 3;
			// pixels[pos] = i / 2;
			// pixels[pos + 1] = j / 2;
			pixels[pos] = 0xff;
			pixels[pos + 1] = 0xff;
			pixels[pos + 2] = 0xff;
		}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, canvasWidth, canvasHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	glEnable(GL_TEXTURE_2D);

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	delete[] pixels;

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