#include "canvas.hpp"
#include <GL/gl.h>

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
            // pixels[pos] = i / 2;
            // pixels[pos + 1] = j / 2;
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