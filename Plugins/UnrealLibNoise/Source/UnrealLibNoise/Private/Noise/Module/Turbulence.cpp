

#include "UnrealLibNoise.h"
#include "Turbulence.h"

UTurbulence::UTurbulence(const FObjectInitializer& ObjectInit) : Super(ObjectInit)
{
	xDistortModule = ObjectInit.CreateDefaultSubobject<UPerlin>(this, TEXT("xDistort"));
	yDistortModule = ObjectInit.CreateDefaultSubobject<UPerlin>(this, TEXT("yDistort"));
	zDistortModule = ObjectInit.CreateDefaultSubobject<UPerlin>(this, TEXT("zDistort"));

	Power = DEFAULT_TURBULENCE_POWER;
	SetSeed(DEFAULT_TURBULENCE_SEED);
	SetFrequency(DEFAULT_TURBULENCE_FREQUENCY);
	SetRoughness(DEFAULT_TURBULENCE_ROUGHNESS);
}

float UTurbulence::GetFrequency()
{
	// Since each noise::module::Perlin noise module has the same frequency, it
	// does not matter which module we use to retrieve the frequency.
	return xDistortModule->GetFrequency();
}

int UTurbulence::GetSeed() 
{
	return xDistortModule->GetSeed();
}

float UTurbulence::GetValue(FVector Coordinates)
{
	if (IsValid(GetSourceModule(0))) throw ExceptionNoModule();

	// Get the values from the three noise::module::Perlin noise modules and
	// add each value to each coordinate of the input value.  There are also
	// some offsets added to the coordinates of the input values.  This prevents
	// the distortion modules from returning zero if the (Coordinates.X, Coordinates.Y, Coordinates.Z) coordinates,
	// when multiplied by the frequency, are near an integer boundary.  This is
	// due to a property of gradient coherent noise, which returns zero at
	// integer boundaries.
	float x0, y0, z0;
	float x1, y1, z1;
	float x2, y2, z2;
	x0 = Coordinates.X + (12414.0 / 65536.0);
	y0 = Coordinates.Y + (65124.0 / 65536.0);
	z0 = Coordinates.Z + (31337.0 / 65536.0);
	x1 = Coordinates.X + (26519.0 / 65536.0);
	y1 = Coordinates.Y + (18128.0 / 65536.0);
	z1 = Coordinates.Z + (60493.0 / 65536.0);
	x2 = Coordinates.X + (53820.0 / 65536.0);
	y2 = Coordinates.Y + (11213.0 / 65536.0);
	z2 = Coordinates.Z + (44845.0 / 65536.0);
	float xDistort = Coordinates.X + (xDistortModule->GetValue(FVector(x0, y0, z0)) * Power);
	float yDistort = Coordinates.Y + (yDistortModule->GetValue(FVector(x1, y1, z1)) * Power);
	float zDistort = Coordinates.Z + (zDistortModule->GetValue(FVector(x2, y2, z2)) * Power);

	// Retrieve the output value at the offsetted input value instead of the
	// original input value.
	return GetSourceModule(0)->GetValue(FVector(xDistort, yDistort, zDistort));
}

void UTurbulence::SetSeed(int seed)
{
	// Set the seed of each noise::module::Perlin noise modules.  To prevent any
	// sort of weird artifacting, use a slightly different seed for each noise
	// module.
	xDistortModule->SetSeed(seed);
	yDistortModule->SetSeed(seed + 1);
	zDistortModule->SetSeed(seed + 2);
}



