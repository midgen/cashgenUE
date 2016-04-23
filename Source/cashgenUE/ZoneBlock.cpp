
#include "cashgenUE.h"
#include "ZoneBlock.h"



ZoneBlock::ZoneBlock(float aHeight, FColor aColor, int aX, int aY)
{
	ZoneBlock();
	Height = aHeight;
	Color = aColor;
	MyX = aX;
	MyY = aY;
	Slope = 0.0f;
}



ZoneBlock::ZoneBlock()
{

}

void ZoneBlock::ProcessCorners(const float aMaxHeight, const float aUnitSize)
{
	topLeftCorner.CalcHeight();
	topRightCorner.CalcHeight();
	bottomLeftCorner.CalcHeight();
	bottomRightCorner.CalcHeight();

	float heightRatio = this->Height / aMaxHeight;

	Color.R = 255 * heightRatio;
	Color.G = 0;
	Color.B = 0;
	Color.A = 1;

	float minHeight = Height;
	float maxHeight = Height;

	if (topLeftCorner.height > maxHeight) { maxHeight = topLeftCorner.height; }
	if (topRightCorner.height > maxHeight) { maxHeight = topRightCorner.height; }
	if (bottomRightCorner.height > maxHeight) { maxHeight = bottomRightCorner.height; }
	if (bottomLeftCorner.height > maxHeight) { maxHeight = bottomLeftCorner.height; }

	if (topLeftCorner.height < minHeight) { minHeight = topLeftCorner.height; }
	if (topRightCorner.height < minHeight) { minHeight = topRightCorner.height; }
	if (bottomRightCorner.height < minHeight) { minHeight = bottomRightCorner.height; }
	if (bottomLeftCorner.height < minHeight) { minHeight = bottomLeftCorner.height; }

	Slope = (maxHeight - minHeight) / aUnitSize;

}

ZoneBlock::~ZoneBlock()
{

}