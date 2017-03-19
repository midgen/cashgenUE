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

	return (inputModule->GetNoise3D(aX, aY, aZ));
}

float UUFNWarpModule::GetNoise2D(float aX, float aY)
{
	if (!(inputModule)) {
		return 0.0f;
	}

	FVector2D p = FVector2D(aX, aY);

	FVector2D q = FVector2D(inputModule->GetNoise2D(p.X, p.Y), inputModule->GetNoise2D(p.X + Iteration1XOffset, p.Y + Iteration1YOffset));

	//q *= unitSize;

	FVector2D r = FVector2D(inputModule->GetNoise2D((p.X + (multiplier * q).X) + Iteration2XOffset1, (p.Y + (multiplier * q).Y) + Iteration2YOffset1),
		inputModule->GetNoise2D((p.X + (multiplier * q).X) + Iteration2XOffset2, (p.Y + (multiplier * q).Y) + Iteration2YOffset2));

	return (inputModule->GetNoise2D(p.X + (multiplier * r.X), p.Y + (multiplier * r.Y)));

	//return (inputModule->GetNoise2D(p.X + (multiplier * q.X), p.Y + (multiplier * q.Y)));
}

