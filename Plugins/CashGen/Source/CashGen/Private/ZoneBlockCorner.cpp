#include "cashgen.h"
#include "ZoneBlockCorner.h"
#include "ZoneBlock.h"

void ZoneBlockCorner::CalcHeight()
{
	float tlHeight = 0;
	float trHeight = 0;
	float blHeight = 0;
	float brHeight = 0;

	int numToAverage = 0;

	if (topLeftBlock != nullptr)
	{
		tlHeight = topLeftBlock->Height;
		++numToAverage;
	}
	if (bottomLeftBlock != nullptr)
	{
		blHeight = bottomLeftBlock->Height;
		++numToAverage;
	}
	if (topRightBlock != nullptr)
	{
		trHeight = topRightBlock->Height;
		++numToAverage;
	}
	if (bottomRightBlock != nullptr)
	{
		brHeight = bottomRightBlock->Height;
		++numToAverage;
	}

	this->height = (tlHeight + blHeight + trHeight + brHeight) / numToAverage;
}