#pragma once
#include "CashGen.h"
#include "CGIntVector2.generated.h"



/** Struct defines all applicable attributes for managing generation of a single zone */
USTRUCT()
struct FCGIntVector2
{
	GENERATED_USTRUCT_BODY()

	int32 X, Y;
	FCGIntVector2(int32 aX, int32 aY)
	{
		X = aX;
		Y = aY;
	}
	FCGIntVector2()
	{
		X = 0; Y = 0;
	}

	FORCEINLINE bool operator==(const FCGIntVector2& Src) const
	{
		return (X == Src.X) && (Y == Src.Y);
	}

	FORCEINLINE bool operator!=(const FCGIntVector2& Src) const
	{
		return X != Src.X || Y != Src.Y;
	}

	FCGIntVector2 operator-(const FCGIntVector2& Src) const
	{
		return FCGIntVector2(X - Src.X, Y - Src.Y);
	}

	FString ToString()
	{
		return "[" + FString::FromInt(X) + ":" + FString::FromInt(Y) + "]";
	}

	friend FORCEINLINE uint32 GetTypeHash(const FCGIntVector2& point)
	{
		return FCrc::MemCrc32(&point, sizeof(FCGIntVector2));
	}

};
