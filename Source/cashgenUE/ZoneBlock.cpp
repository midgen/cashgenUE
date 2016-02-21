
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

void ZoneBlock::ProcessCorners()
{
	topLeftCorner.CalcHeight();
	topRightCorner.CalcHeight();
	bottomLeftCorner.CalcHeight();
	bottomRightCorner.CalcHeight();
}

ZoneBlock::~ZoneBlock()
{

}