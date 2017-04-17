#pragma once
#include "cashgen.h"
struct CGPoint
{
	int32 X, Y;
	CGPoint(int32 aX, int32 aY)
	{
		X = aX;
		Y = aY;
	}
	CGPoint()
	{
		X = 0; Y = 0;
	}

	bool operator==(const CGPoint& Src) const
	{
		return (X == Src.X) && (Y == Src.Y);
	}

	CGPoint operator-(const CGPoint& Src) const
	{
		return CGPoint(X - Src.X, Y - Src.Y);
	}

	bool operator!=(const CGPoint& Src) const
	{
		return (X != Src.X) || (Y != Src.Y);
	}

	friend FORCEINLINE uint32 GetTypeHash(const CGPoint& point)
	{
		return FCrc::MemCrc_DEPRECATED(&point, sizeof(CGPoint));
	}

};