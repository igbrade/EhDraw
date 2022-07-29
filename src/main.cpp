#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <wingdi.h>
#include <windowsx.h>
#include <GL/gl.h>
#include <iostream>
#include <algorithm>

#include "canvas.hpp"
#include "win32window.hpp"
#include "vec2.hpp"

int windowWidth;
int windowHeight;

float canvasXX;
float canvasYY = 0.75;

win32Window window;
canvas cnv;

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

bool mouseDown = false;
bool eraser = false;

vec2<int> lastMouse;

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

    vec2<int> pixelCoord = vec2<int>(cnv.pxDimension.x * textureCoord.x, cnv.pxDimension.y * textureCoord.y);

    for(int xx = std::max(0, pixelCoord.x - radius + 1); xx <= std::min((int)cnv.pxDimension.x, pixelCoord.x + radius - 1); ++xx)
    {
    	for(int yy = std::max(0, pixelCoord.y - radius + 1); yy <= std::min((int)cnv.pxDimension.y, pixelCoord.y + radius - 1); ++yy)
    	{
    		int pixelPos = cnv.rowSz * yy + xx * 3;
    		cnv.pixels[pixelPos] = r;
    		cnv.pixels[pixelPos + 1] = g;
    		cnv.pixels[pixelPos + 2] = b;
    	}
	}
}

void canvasPaintLine(vec2<int> screenA, vec2<int> screenB, int r, int g, int b)
{
    canvasPaint(screenA.x, screenA.y, r, g, b);

    int dX = abs(screenA.x - screenB.x);
    int dY = abs(screenA.y - screenB.y);
    int x = screenB.x, y = screenB.y;
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


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cnv.pxDimension.x, cnv.pxDimension.y, 0, GL_RGB, GL_UNSIGNED_BYTE, cnv.pixels);
    // printf("Clicked on: %f %f\n", textureCoordX, textureCoordY);

    PostMessage(window.hwnd, WM_PAINT, 0, 0);
    if(swapped)
        std::swap(screenA.x, screenA.y);
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
    vec2<int> mouseCoords(mouseX, mouseY);
    if(mouseDown)
    {
        vec2<float> normalized = screenToNormalized(mouseCoords);
        if(-canvasXX <= normalized.x && normalized.x <= canvasXX)
        {
            if(-canvasYY <= normalized.y && normalized.y <= canvasYY)
            {
                if(eraser)
                    canvasPaintLine(mouseCoords, lastMouse, 0xff, 0xff, 0xff);
                else 
                    canvasPaintLine(mouseCoords, lastMouse, 0, 0, 0);
                lastMouse = mouseCoords;
            }
        }
    }
}

void onMouseDown(int mouseX, int mouseY)
{
	mouseDown = true;
    vec2<int> mousePos(mouseX, mouseY);
    vec2<float> normalized = screenToNormalized(mousePos);
    lastMouse = mousePos;

    if(canvasXX <= normalized.x && normalized.x <= canvasXX){
        if(canvasYY <= normalized.y && normalized.y <= canvasYY)
        {
        	if(eraser)
        		canvasPaint(mouseX, mouseY, 0xff, 0xff, 0xff);
        	else	
            	canvasPaint(mouseX, mouseY, 0, 0, 0);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cnv.pxDimension.x, cnv.pxDimension.y, 0, GL_RGB, GL_UNSIGNED_BYTE, cnv.pixels);

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

    initCanvas(&cnv, vec2<int>(128, 128));    

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    destroyCanvas(&cnv);
}