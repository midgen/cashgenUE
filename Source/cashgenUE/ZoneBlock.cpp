
#include "cashgenUE.h"
#include "ZoneBlock.h"



ZoneBlock::ZoneBlock(float aHeight, FColor aColor, int aX, int aY)
{
	ZoneBlock();
	Height = aHeight;
	Color = aColor;
	MyX = aX;
	MyY = aY;
}



ZoneBlock::ZoneBlock()
{

}

void ZoneBlock::ProcessCorners(float aMaxHeight)
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
}

ZoneBlock::~ZoneBlock()
{

}