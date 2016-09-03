#pragma once
#include "cashgen.h"
struct CGPoint
{
	int32 x, y;
	CGPoint(int32 aX, int32 aY)
	{
		x = aX;
		y = aY;
	}
	CGPoint()
	{
		x = 0; y = 0;
	}

	bool operator==(const CGPoint& Src) const
	{
		return (x == Src.x) && (y == Src.y);
	}

	friend FORCEINLINE uint32 GetTypeHash(const CGPoint& point)
	{
		return FCrc::MemCrc_DEPRECATED(&point, sizeof(CGPoint));
	}

};