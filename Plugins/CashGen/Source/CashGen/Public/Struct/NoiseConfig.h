#pragma once
#include "cashgen.h"
#include "FastNoise.h"
#include "NoiseConfig.generated.h"


/** Defines the noise generator parameters */
USTRUCT()
struct FNoiseConfig
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Noise Config Struct")
	TEnumAsByte<ENoiseType> Noise_Type;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Noise Config Struct")
	TEnumAsByte<EFractalType> Fractal_Type;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Noise Config Struct")
	int32 Octaves;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Noise Config Struct")
	float Frequency;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Noise Config Struct")
	int32 Seed;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Noise Config Struct")
	float Lacunarity;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Noise Config Struct")
	float Gain;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Config Struct")
	float SampleFactor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Config Struct")
	bool Invert;

};
