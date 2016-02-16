
#pragma once

#include "Engine.h"


class ZoneBlock
{
public:
	float Height;
	FColor Color;

	ZoneBlock* up;
	ZoneBlock* down;
	ZoneBlock* left;
	ZoneBlock* right;

	float GetUpHeight();
	float GetDownHeight();
	float GetLeftHeight();
	float GetRightHeight();

	ZoneBlock(float aHeight, FColor aColor);
	ZoneBlock();
	
};
