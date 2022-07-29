#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <wingdi.h>
#include <windowsx.h>
#include <GL/gl.h>
#include <iostream>
#include <algorithm>

#include "win32window.hpp"
#include "vec2.hpp"

int windowWidth;
int windowHeight;

float canvasXX;
float canvasYY = 0.75;

win32Window window;
void draw()
{
    glClearColor(0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    float ratio = windowHeight / (float)windowWidth;
    canvasXX = ratio * canvasYY;

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-canvasXX, -canvasYY);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(canvasXX, -canvasYY);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(canvasXX, canvasYY);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(-canvasXX, canvasYY);
    glEnd();

    glFlush();
}

unsigned int texture;
unsigned int canvasWidth = 128;
unsigned int canvasHeight = 128;
unsigned int canvasRowSz = 4 * ((canvasWidth * 3 + 3) / 4);
unsigned int canvasBytes = canvasRowSz * canvasHeight;
unsigned char *pixels;


bool mouseDown = false;
bool eraser = false;

int lastX, lastY;

vec2<float> screenToNormalized(vec2<int> screenPos)
{
    return vec2<float>(2.0f * screenPos.x / windowWidth, 2.0f * screenPos.y / windowHeight) - vec2<float>::one();
}

void canvasPaint(int screenX, int screenY, int r, int g, int b, int radius = 1){
    vec2<float> normalized = screenToNormalized(vec2<int>(screenX, screenY));
    vec2<float> textureCoord = normalized - vec2<float>(-canvasXX, -canvasYY);
    textureCoord.x /= (2 * canvasXX);
    textureCoord.y /= (2 * canvasYY);
    textureCoord.y = 1 - textureCoord.y;

    vec2<int> pixelCoord = vec2<int>(canvasWidth * textureCoord.x, canvasHeight * textureCoord.y);

    for(int xx = std::max(0, pixelCoord.x - radius + 1); xx <= std::min((int)canvasWidth, pixelCoord.x + radius - 1); ++xx)
    {
    	for(int yy = std::max(0, pixelCoord.y - radius + 1); yy <= std::min((int)canvasHeight, pixelCoord.y + radius - 1); ++yy)
    	{
    		int pixelPos = canvasRowSz * yy + xx * 3;
    		pixels[pixelPos] = r;
    		pixels[pixelPos + 1] = g;
    		pixels[pixelPos + 2] = b;
    	}
	}
}

void canvasPaintLine(vec2<int> screenA, vec2<int> screenB, int r, int g, int b)
{
    canvasPaint(screenA.x, screenA.y, r, g, b);

    int dX = abs(screenA.x - lastX);
    int dY = abs(screenA.y - lastY);
    int x = lastX, y = lastY;
    bool swapped = 0;
    if(dX < dY)
    {
        swapped = 1, std::swap(x, y), std::swap(screenA.x, screenA.y), std::swap(dX, dY);
    }
    
    while(x != screenA.x || y != screenA.y)
    {
        int progressionRate = (dY == 0 ? 0x3f3f3f3f : dX / dY);
        while(progressionRate-- && x != screenA.x)
        {
            if(x < screenA.x) x++;
            else x--;

            if(swapped)
            {
                canvasPaint(y, x, r, g, b);
            }
            else
            {
                canvasPaint(x, y, r, g, b);
            }
        }
        if(y != screenA.y)
        {
            if(y < screenA.y) y++;
            else y--;
        }

        if(swapped)
        {
            canvasPaint(y, x, r, g, b);
        }
        else
        {
            canvasPaint(x, y, r, g, b);
        }
    }


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, canvasWidth, canvasHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    // printf("Clicked on: %f %f\n", textureCoordX, textureCoordY);

    PostMessage(window.hwnd, WM_PAINT, 0, 0);
    if(swapped)
        std::swap(screenA.x, screenA.y);
    lastX = screenA.x;
    lastY = screenA.y;
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
        vec2<float> normalized = screenToNormalized(vec2<int>(mouseX, mouseY));
        if(-canvasXX <= normalized.x && normalized.x <= canvasXX)
        {
            if(-canvasYY <= normalized.y && normalized.y <= canvasYY)
            {
                if(eraser)
                    canvasPaintLine(vec2<int>(mouseX, mouseY), vec2<int>(lastX, lastY), 0xff, 0xff, 0xff);
                else 
                    canvasPaintLine(vec2<int>(mouseX, mouseY), vec2<int>(lastX, lastY), 0, 0, 0);
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

    if(canvasXX <= normalizedX && normalizedX <= canvasXX){
        if(canvasYY <= normalizedY && normalizedY <= canvasYY)
        {
        	if(eraser)
        		canvasPaint(mouseX, mouseY, 0xff, 0xff, 0xff);
        	else	
            	canvasPaint(mouseX, mouseY, 0, 0, 0);

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