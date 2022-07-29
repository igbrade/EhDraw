#include "canvas.hpp"
#include <GL/gl.h>
#include <algorithm>

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


void canvasPaint(canvas *c, vec2<float> textureCoord, int r, int g, int b)
{
    vec2<int> pixelCoord = vec2<int>(c->pxDimension.x * textureCoord.x, c->pxDimension.y * textureCoord.y);

	int pixelPos = c->rowSz * pixelCoord.y + pixelCoord.x * 3;
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
