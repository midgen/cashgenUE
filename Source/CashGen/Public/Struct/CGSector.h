#pragma once
#include "Struct/IntVector2.h"
#include "CGSector.generated.h"


USTRUCT(BlueprintType)
struct FCGSector
{
	GENERATED_USTRUCT_BODY()

	FIntVector2 mySector;
	uint8 myLOD;

	FCGSector(const int32 aX, const int32 aY, const uint8 aLOD)
	{
		mySector.X = aX;
		mySector.Y = aY;
		myLOD = aLOD;
	}

	FCGSector(const FIntVector2 aIntVector, const uint8 aLOD = 0)
	{
		mySector.X = aIntVector.X;
		mySector.Y = aIntVector.Y;
		myLOD = aLOD;
	}


	FCGSector()
	{
		mySector.X = 0;
		mySector.Y = 0;
		myLOD = 0;
	}


	FORCEINLINE bool operator==(const FCGSector& Src) const
	{
		return (mySector.X == Src.mySector.X) && (mySector.Y == Src.mySector.Y);
	}

	FORCEINLINE bool operator!=(const FCGSector& Src) const
	{
		return mySector.X != Src.mySector.X || mySector.Y != Src.mySector.Y;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FCGSector& point)
	{
		return FCrc::MemCrc32(&point.mySector, sizeof(FIntVector2));
	}

};