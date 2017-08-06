#pragma once
#include "cashgen.h"
#include "IntVector2.generated.h"



/** Struct defines all applicable attributes for managing generation of a single zone */
USTRUCT()
struct FIntVector2
{
	GENERATED_USTRUCT_BODY()

	int32 X, Y;
	FIntVector2(int32 aX, int32 aY)
	{
		X = aX;
		Y = aY;
	}
	FIntVector2()
	{
		X = 0; Y = 0;
	}

	bool operator==(const FIntVector2& Src) const
	{
		return (X == Src.X) && (Y == Src.Y);
	}

	FIntVector2 operator-(const FIntVector2& Src) const
	{
		return FIntVector2(X - Src.X, Y - Src.Y);
	}

	bool operator!=(const FIntVector2& Src) const
	{
		return (X != Src.X) || (Y != Src.Y);
	}

	friend FORCEINLINE uint32 GetTypeHash(const FIntVector2& point)
	{
		return FCrc::MemCrc_DEPRECATED(&point, sizeof(FIntVector2));
	}

};