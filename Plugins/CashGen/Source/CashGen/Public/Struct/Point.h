#pragma once
#include "cashgen.h"
struct Point
{
	int32 x, y;
	Point(int32 aX, int32 aY)
	{
		x = aX;
		y = aY;
	}
	Point()
	{
		x = 0; y = 0;
	}

	bool operator==(const Point& Src) const
	{
		return (x == Src.x) && (y == Src.y);
	}

	friend FORCEINLINE uint32 GetTypeHash(const Point& point)
	{
		return FCrc::MemCrc_DEPRECATED(&point, sizeof(Point));
	}

};