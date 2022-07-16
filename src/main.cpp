#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <wingdi.h>
#include <windowsx.h>
#include <GL/gl.h>
#include <iostream>
#include <utility>
#include <algorithm>

#include "win32window.hpp"

win32Window window;
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
bool eraser = false;

int lastX, lastY;

float normalizeX(int X){
    return 2.0 * X / windowWidth - 1.0;
}

float normalizeY(int Y){
    return 2.0 * Y / windowHeight - 1.0;
}

std::pair<float, float> normalize(int X, int Y){
    return std::make_pair(normalizeX(X), normalizeY(Y));
}

void toRender(int X, int Y, int r, int g, int b){
    // printf("%d %d\n", X, Y);
    auto [normalizedX, normalizedY] = normalize(X, Y);

    float textureCoordX = normalizedX - (-0.5);
    float textureCoordY = normalizedY - (-0.5);

    textureCoordY = 1 - textureCoordY;

    int pixelX = canvasWidth * textureCoordX;
    int pixelY = canvasHeight * textureCoordY;

    int pixelPos = canvasRowSz * pixelY + pixelX * 3;
    pixels[pixelPos] = r;
    pixels[pixelPos + 1] = g;
    pixels[pixelPos + 2] = b;
}
  
extern void (*drawCallback)();
extern void (*mouseMoveCallback)(int mouseX, int mouseY);
extern void (*mouseDownCallback)(int mouseX, int mouseY);
extern void (*mouseUpCallback)(int mouseX, int mouseY);
extern void (*resizeCallback)(int newWidth, int newHeight);
extern void (*keyDownCallback)(int key);
extern void (*keyUpCallback)(int key);

void onMouseMove(int mouseX, int mouseY)
{
	// printf("Mouse pos: (%4d, %4d)\r", mouseX, mouseY);
    if(mouseDown)
    {

        float normalizedX = 2.0 * mouseX / windowWidth - 1.0;
        float normalizedY = 2.0 * mouseY / windowHeight - 1.0;

        if(-0.5 <= normalizedX && normalizedX <= 0.5){
            if(-0.5 <= normalizedY && normalizedY <= 0.5)
            {
            	if(eraser)
            		toRender(mouseX, mouseY, 0xff, 0xff, 0xff);
            	else
                	toRender(mouseX, mouseY, 0, 0, 0);
                // printf("%d %d %d %d\n", mouseX, lastX, mouseY, lastY);
                int dX = abs(mouseX - lastX);
                int dY = abs(mouseY - lastY);
                int x = lastX, y = lastY;
                bool swapped = 0;
                if(dX < dY){
                    swapped = 1, std::swap(x, y), std::swap(mouseX, mouseY), std::swap(dX, dY);
                }
                
                while(x != mouseX || y != mouseY){
                    int progressionRate = (dY == 0 ? 0x3f3f3f3f : dX / dY);
                    while(progressionRate-- && x != mouseX){
                        if(x < mouseX) x++;
                        else x--;

                        if(swapped)
                        {
                        	if(eraser)
                        		toRender(y, x, 0xff, 0xff, 0xff);
                        	else
                        		toRender(y, x, 0, 0, 0);
                        }
                        else
                        {
                        	if(eraser)
	                        	toRender(x, y, 0xff, 0xff, 0xff);
                        	else
                        		toRender(x, y, 0, 0, 0);
                        }
                    }
                    if(y != mouseY){
                        if(y < mouseY) y++;
                        else y--;
                    }

                    if(swapped)
                    {
                    	if(eraser)
                    		toRender(y, x, 0xff, 0xff, 0xff);
                    	else
                    		toRender(y, x, 0, 0, 0);
                    }
                    else
                    {
                    	if(eraser)
                    		toRender(x, y, 0xff, 0xff, 0xff);
                    	else
                   			toRender(x, y, 0, 0, 0);
                    }
                }
            

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, canvasWidth, canvasHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
                // printf("Clicked on: %f %f\n", textureCoordX, textureCoordY);

                PostMessage(window.hwnd, WM_PAINT, 0, 0);
                if(swapped)
                    std::swap(mouseX, mouseY);
                lastX = mouseX;
                lastY = mouseY;
            }
        }
    }
}

void onMouseDown(int mouseX, int mouseY)
{
	mouseDown = true;
	float normalizedX = 2.0 * mouseX / windowWidth - 1.0;
    float normalizedY = 2.0 * mouseY / windowHeight - 1.0;
    lastX = mouseX;
    lastY = mouseY;

    if(-0.5 <= normalizedX && normalizedX <= 0.5){
        if(-0.5 <= normalizedY && normalizedY <= 0.5)
        {
        	if(eraser)
        		toRender(mouseX, mouseY, 0xff, 0xff, 0xff);
        	else	
            	toRender(mouseX, mouseY, 0, 0, 0);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, canvasWidth, canvasHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
            // printf("Clicked on: %f %f\n", textureCoordX, textureCoordY);

            PostMessage(window.hwnd, WM_PAINT, 0, 0);
        }
    }
}

void onMouseUp(int mouseX, int mouseY)
{
	mouseDown = false;
}

void onResize(int newWidth, int newHeight)
{
	glViewport(0, 0, newWidth, newHeight);
	windowWidth = newWidth;
	windowHeight = newHeight;
}

void onKeyDown(int key)
{
	if(key == 'E')
		eraser = !eraser;
}

int main()
{
	if(!win32Init())
		return -1;
	if(!win32createWindow(&window))
		return -1;
	if(!win32SetupGlContext(&window))
		return -1;

	drawCallback = draw;
	mouseUpCallback = onMouseUp;
	mouseDownCallback = onMouseDown;
	mouseMoveCallback = onMouseMove;
	resizeCallback = onResize;
	keyDownCallback = onKeyDown;

	win32ShowWindow(&window);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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
}