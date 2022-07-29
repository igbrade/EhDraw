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