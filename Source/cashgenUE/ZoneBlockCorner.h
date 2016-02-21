#pragma once
#include "cashgenUE.h"

class ZoneBlock;

class ZoneBlockCorner
{
public:
	ZoneBlock* topLeftBlock;
	ZoneBlock* topRightBlock;
	ZoneBlock* bottomLeftBlock;
	ZoneBlock* bottomRightBlock;
	float height;
	void CalcHeight();

};