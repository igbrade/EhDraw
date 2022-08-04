#include "canvas.hpp"
#include <GL/gl.h>
#include <algorithm>
#include <cstdio>

void initCanvas(canvas *c, vec2<int> dimension)
{
	c->pxDimension = dimension;
	c->rowSz = 4 * ((dimension.x * 3 + 3) / 4);
	c->numBytes = c->rowSz * dimension.y;
	glGenTextures(1, &c->glTextureId);
    glBindTexture(GL_TEXTURE_2D, c->glTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    c->pixels = new unsigned char[c->numBytes];
    for(int i = 0; i < dimension.y; ++i)
        for(int j = 0; j < dimension.x; ++j)
        {
            int pos = i * c->rowSz + j * 3;
            //TODO: Add dark mode 
            //Default canvas is white
            c->pixels[pos] = 0xff;
            c->pixels[pos + 1] = 0xff;
            c->pixels[pos + 2] = 0xff;
        }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dimension.x, dimension.y, 0, GL_RGB, GL_UNSIGNED_BYTE, c->pixels);

    glEnable(GL_TEXTURE_2D);
}

void destroyCanvas(canvas *c)
{
	delete[] c->pixels;
}

void fillCanvas(canvas *c, int r, int g, int b)
{
	 for(int i = 0; i < c->pxDimension.y; ++i)
        for(int j = 0; j < c->pxDimension.x; ++j)
        {
            int pos = i * c->rowSz + j * 3;
            c->pixels[pos] = r;
            c->pixels[pos + 1] = g;
            c->pixels[pos + 2] = b;
        }
}

void canvasPaint(canvas *c, vec2<float> textureCoord, int r, int g, int b, bool saveHistory)
{
    vec2<int> pixelCoord = vec2<int>(c->pxDimension.x * textureCoord.x, c->pxDimension.y * textureCoord.y);

	int pixelPos = c->rowSz * pixelCoord.y + pixelCoord.x * 3;
    if(saveHistory)
    {
        historyEntry e;
        e.modifiedPixels = new pixelHistory[1];
        e.nModifiedPixels = 1;
        e.modifiedPixels[0].pos = pixelCoord;
        c->history.push_back(e);
        e.modifiedPixels[0].oldR = c->pixels[pixelPos];
        e.modifiedPixels[0].oldG = c->pixels[pixelPos + 1];
        e.modifiedPixels[0].oldB = c->pixels[pixelPos + 2];

        e.modifiedPixels[0].newR = r;
        e.modifiedPixels[0].newG = g;
        e.modifiedPixels[0].newB = b;
    }

	c->pixels[pixelPos] = r;
	c->pixels[pixelPos + 1] = g;
	c->pixels[pixelPos + 2] = b;

}

void canvasPaintSquare(canvas *c, vec2<float> textureCoordCenter, int r, int g, int b, int sz)
{
    vec2<int> pixelCoord = vec2<int>(c->pxDimension.x * textureCoordCenter.x, c->pxDimension.y * textureCoordCenter.y);

    for(int yy = std::max(0, pixelCoord.y - sz + 1); yy <= std::min((int)c->pxDimension.y, pixelCoord.y + sz - 1); ++yy)
    {
    	for(int xx = std::max(0, pixelCoord.x - sz + 1); xx <= std::min((int)c->pxDimension.x, pixelCoord.x + sz - 1); ++xx)
    	{
    		int pixelPos = c->rowSz * yy + xx * 3;
    		c->pixels[pixelPos] = r;
    		c->pixels[pixelPos + 1] = g;
    		c->pixels[pixelPos + 2] = b;
    	}
	}
}

void canvasUndo(canvas *c)
{
    if(c->history.empty())
    {
        printf("Nothing to undo.\n");
        return;
    }
    historyEntry &e = c->history.back();
    for(int i = 0; i < e.nModifiedPixels; ++i)
    {
        int pixelPos = c->rowSz * e.modifiedPixels[i].pos.y + e.modifiedPixels[i].pos.x * 3;
        c->pixels[pixelPos] = e.modifiedPixels[i].oldR;
        c->pixels[pixelPos + 1] = e.modifiedPixels[i].oldG;
        c->pixels[pixelPos + 2] = e.modifiedPixels[i].oldB;
    }  
    delete[] e.modifiedPixels;
    c->history.pop_back();
}

void canvasPaintLine(canvas *c, vec2<float> textureCoordA, vec2<float> textureCoordB, int r, int g, int b, bool saveHistory)
{
    vec2<int> pixelCoordA(textureCoordA.x * c->pxDimension.x, textureCoordA.y * c->pxDimension.y);
    vec2<int> pixelCoordB(textureCoordB.x * c->pxDimension.x, textureCoordB.y * c->pxDimension.y);
    vec2<int> diff = pixelCoordB - pixelCoordA;

    int maxDiff = std::max(abs(diff.x), abs(diff.y));
    maxDiff = std::max(maxDiff, 1);

    if(saveHistory)
    {
        historyEntry e;
        e.modifiedPixels = new pixelHistory[maxDiff + 1];
        e.nModifiedPixels = maxDiff + 1;
        for(int i = 0; i <= maxDiff; ++i)
        {
            vec2<int> pos = pixelCoordA + diff * i / maxDiff;   
            int pixelIndex = c->rowSz * pos.y + pos.x * 3;
            e.modifiedPixels[i].oldR = c->pixels[pixelIndex];
            e.modifiedPixels[i].oldG = c->pixels[pixelIndex + 1];
            e.modifiedPixels[i].oldB = c->pixels[pixelIndex + 2];

            e.modifiedPixels[i].newR = r;
            e.modifiedPixels[i].newG = g;
            e.modifiedPixels[i].newB = b;

            e.modifiedPixels[i].pos = pos;
        }
        c->history.push_back(e);
    }

    for(int i = 0; i <= maxDiff; ++i)
    {
        vec2<int> pos = pixelCoordA + diff * i / maxDiff;

        int pixelIndex = c->rowSz * pos.y + pos.x * 3;
        c->pixels[pixelIndex] = r;
        c->pixels[pixelIndex + 1] = g;
        c->pixels[pixelIndex + 2] = b;
    }
}