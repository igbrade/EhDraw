#pragma once
#include "vec2.hpp"

struct canvas
{
	vec2<int> pxDimension;
	unsigned int glTextureId;
	unsigned int numBytes;
	unsigned int rowSz;

	unsigned char *pixels;
};

void initCanvas(canvas *c, vec2<int> dimension);
void destroyCanvas(canvas *c);
void canvasPaint(canvas *c, vec2<float> textureCoord, int r, int g, int b);
void canvasPaintSquare(canvas *c, vec2<float> textureCoordCenter, int r, int g, int b, int sz);