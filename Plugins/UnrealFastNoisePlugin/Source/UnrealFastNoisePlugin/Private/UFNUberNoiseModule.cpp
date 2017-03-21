
#include "UnrealFastNoisePlugin.h"
#include "UFNNoiseGenerator.h"
#include "UFNScaleBiasModule.h"

UUFNUberNoiseModule::UUFNUberNoiseModule(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

float UUFNUberNoiseModule::GetNoise3D(float aX, float aY, float aZ)
{
	if (!(inputModule)) {
		return 0.0f;
	}

	return (inputModule->GetNoise3D(aX, aY, aZ));
}

float UUFNUberNoiseModule::GetNoise2D(float aX, float aY)
{
	if (!(inputModule)) {
		return 0.0f;
	}

	//FVector noise = GetNoise2DDerivative(aX, aY);

	FVector2D p = FVector2D(aX, aY);

	float sum = 0.5f;
	float freq = 1.0f;
	float amp = 1.0f;
	FVector2D  dsum = FVector2D(0.0f, 0.0f);
	for (int i = 0; i < iterations; i++)
	{
		FVector n = GetNoise2DDerivative(p.X, p.Y);
		dsum += FVector2D(n.Y, n.Z);
		sum += amp*n.X / (1.0f + FVector2D::DotProduct(dsum, dsum));
		freq *= 1.8f;
		amp += 0.5f;
	}

	return sum;

}

FVector UUFNUberNoiseModule::GetNoise2DDerivative(float aX, float aY)
{
	FVector2D p = FVector2D(aX, aY);
	p.X = FMath::FloorToFloat(p.X);
	p.Y = FMath::FloorToFloat(p.Y);
	FVector2D f = FVector2D(aX, aY) - p;

	FVector2D u = f*f*f;

	float a = inputModule->GetNoise2D(p.X, p.Y);
	float b = inputModule->GetNoise2D(p.X + sampleRange, p.Y);
	float c = inputModule->GetNoise2D(p.X, p.Y + sampleRange);
	float d = inputModule->GetNoise2D(p.X + sampleRange, p.Y + sampleRange);

	FVector result;

	const FVector2D Intermediate = 6.0f*f*(f)*(FVector2D(b - a, c - a) + (a - b - c + d)*(FVector2D(u.Y, u.X)));

	result = FVector(a + (b - a)*u.X + (c - a)*u.Y + (a - b - c + d)*u.X*u.Y,
		Intermediate.X, Intermediate.Y);

	//result = FVector(a + (b - a)*u.X + (c - a)*u.Y + (a - b - c + d)*u.X*u.Y,
	//6.0*f*(1.0 - f)*(FVector2D(b - a, c - a) + (a - b - c + d)*(FVector2D(u.Y, u.X))));

	return result;
}

