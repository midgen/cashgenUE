#include "UnrealFastNoisePlugin.h"
#include "UFNNoiseGenerator.h"
#include "UFNWarpModule.h"

UUFNWarpModule::UUFNWarpModule(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

float UUFNWarpModule::GetNoise3D(float aX, float aY, float aZ)
{
	if (!(inputModule)) {
		return 0.0f;
	}

	FVector p = FVector(aX, aY, aZ);

	FVector q = FVector(warpModule->GetNoise3D(p.X, p.Y, p.Z), warpModule->GetNoise3D(p.X + Iteration1XOffset, p.Y + Iteration1YOffset, p.Z + Iteration1ZOffset), warpModule->GetNoise3D(p.X + Iteration1XOffset + 0.5f, p.Y + Iteration1YOffset + 0.5f, p.Z + Iteration1ZOffset + 2.4f));

	if (warpIterations == EWarpIterations::One)
	{
		return (inputModule->GetNoise3D(p.X + (multiplier * q.X), p.Y + (multiplier * q.Y), p.Z + (multiplier * q.Z)));
	}


	FVector r = FVector(warpModule->GetNoise3D((p.X + (multiplier * q).X) + Iteration2XOffset1, (p.Y + (multiplier * q).Y) + Iteration2YOffset1, (p.Z + (multiplier * q).Z) + Iteration2ZOffset1),
		warpModule->GetNoise3D((p.X + (multiplier * q).X) + Iteration2XOffset2, (p.Y + (multiplier * q).Y) + Iteration2YOffset2, (p.Z + (multiplier * q).Z) + Iteration2ZOffset2),
		warpModule->GetNoise3D((p.X + (multiplier * q).X + 3.4f) + Iteration2XOffset2, (p.Y + (multiplier * q).Y) + Iteration2YOffset2 + 4.6f, (p.Z + (multiplier * q).Z) + Iteration2ZOffset2) + 2.3f);

	return (inputModule->GetNoise3D(p.X + (multiplier * r.X), p.Y + (multiplier * r.Y), p.Z + (multiplier * r.Z)));
}

float UUFNWarpModule::GetNoise2D(float aX, float aY)
{
	if ((!(inputModule)) || !warpModule) {
		return 0.0f;
	}

	FVector2D p = FVector2D(aX, aY);

	FVector2D q = FVector2D(warpModule->GetNoise2D(p.X, p.Y), warpModule->GetNoise2D(p.X + Iteration1XOffset, p.Y + Iteration1YOffset));

	if (warpIterations == EWarpIterations::One)
	{
		return (inputModule->GetNoise2D(p.X + (multiplier * q.X), p.Y + (multiplier * q.Y)));
	}
	

	FVector2D r = FVector2D(warpModule->GetNoise2D((p.X + (multiplier * q).X) + Iteration2XOffset1, (p.Y + (multiplier * q).Y) + Iteration2YOffset1),
		warpModule->GetNoise2D((p.X + (multiplier * q).X) + Iteration2XOffset2, (p.Y + (multiplier * q).Y) + Iteration2YOffset2));

	return (inputModule->GetNoise2D(p.X + (multiplier * r.X), p.Y + (multiplier * r.Y)));

}

