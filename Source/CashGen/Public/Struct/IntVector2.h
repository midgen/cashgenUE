#pragma once
#include "CashGen.h"
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

	FORCEINLINE bool operator==(const FIntVector2& Src) const
	{
		return (X == Src.X) && (Y == Src.Y);
	}

	FORCEINLINE bool operator!=(const FIntVector2& Src) const
	{
		return X != Src.X || Y != Src.Y;
	}

	FIntVector2 operator-(const FIntVector2& Src) const
	{
		return FIntVector2(X - Src.X, Y - Src.Y);
	}

	FString ToString()
	{
		return "[" + FString::FromInt(X) + ":" + FString::FromInt(Y) + "]";
	}

	friend FORCEINLINE uint32 GetTypeHash(const FIntVector2& point)
	{
		return FCrc::MemCrc32(&point, sizeof(FIntVector2));
	}

};
