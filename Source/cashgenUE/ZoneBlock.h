
#pragma once
#include "cashgenUE.h"
#include "ZoneBlockCorner.h"

class ZoneBlock
{
public:
	float Height;
	FColor Color;
	int MyX;
	int MyY;

	ZoneBlockCorner topLeftCorner;
	ZoneBlockCorner topRightCorner;
	ZoneBlockCorner bottomLeftCorner;
	ZoneBlockCorner bottomRightCorner;

	void ProcessCorners(float aMaxHeight);

	ZoneBlock(float aHeight, FColor aColor, int aX, int aY);
	ZoneBlock();
	~ZoneBlock();
	
};
