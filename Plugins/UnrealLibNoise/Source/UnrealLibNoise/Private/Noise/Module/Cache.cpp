

#include "UnrealLibNoise.h"
#include "Cache.h"

/// Constructor.
UCache::UCache(const FObjectInitializer& ObjectInit) : Super(ObjectInit)
{
	IsCached = false;
}

float UCache::GetValue(FVector Coordinates)
{	
	if (!(IsCached && Coordinates.X == xCache && Coordinates.Y == yCache && Coordinates.Z == zCache)) 
	{
		CachedValue = GetSourceModule(0)->GetValue(Coordinates);
		xCache = Coordinates.X;
		yCache = Coordinates.Y;
		zCache = Coordinates.Z;
	}

	IsCached = true;

	return CachedValue;
}


