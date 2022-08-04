#pragma once
#include "vec2.hpp"
#include <vector>

struct pixelHistory
{
	int oldR, oldG, oldB;
	int newR, newG, newB;
	vec2<int> pos;
};

struct historyEntry
{
	pixelHistory *modifiedPixels;
	int nModifiedPixels;
};

struct canvas
{
	vec2<int> pxDimension;
	unsigned int glTextureId;
	unsigned int numBytes;
	unsigned int rowSz;

	unsigned char *pixels;

	std::vector<historyEntry> history;
};

void initCanvas(canvas *c, vec2<int> dimension);
void destroyCanvas(canvas *c);
void fillCanvas(canvas *c, int r, int g, int b);
void canvasPaint(canvas *c, vec2<float> textureCoord, int r, int g, int b);
void canvasPaintSquare(canvas *c, vec2<float> textureCoordCenter, int r, int g, int b, int sz);
void canvasUndo(canvas *c);