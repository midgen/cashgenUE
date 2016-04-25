

#include "UnrealLibNoise.h"
#include "Billow.h"

/// Constructor
///
/// The default frequency is set to
/// DEFAULT_BILLOW_FREQUENCY.
///
/// The default lacunarity is set to
/// DEFAULT_BILLOW_LACUNARITY.
///
/// The default number of octaves is set to
/// nDEFAULT_BILLOW_OCTAVE_COUNT.
///
/// The default persistence value is set to
/// DEFAULT_BILLOW_PERSISTENCE.
///
/// The default seed value is set to
/// DEFAULT_BILLOW_SEED.
UBillow::UBillow(const FObjectInitializer& ObjectInit) : Super(ObjectInit)
{
	Frequency = DEFAULT_BILLOW_FREQUENCY;
	Lacunarity = DEFAULT_BILLOW_LACUNARITY;
	OctaveCount = DEFAULT_BILLOW_OCTAVE_COUNT;
	Persistence = DEFAULT_BILLOW_PERSISTENCE;
	NoiseQuality = DEFAULT_BILLOW_QUALITY;
	Seed = DEFAULT_BILLOW_SEED;
}

float UBillow::GetValue(FVector Corrdinates)
{
	float value = 0.0;
	float signal = 0.0;
	float curPersistence = 1.0;
	float nx, ny, nz;
	int seed;

	Corrdinates.X *= Frequency;
	Corrdinates.Y *= Frequency;
	Corrdinates.Z *= Frequency;

	for (int curOctave = 0; curOctave < OctaveCount; curOctave++) {

		// Make sure that these floating-point values have the same range as a 32-
		// bit integer so that we can pass them to the coherent-noise functions.
		nx = UNoiseGen::MakeInt32Range(Corrdinates.X);
		ny = UNoiseGen::MakeInt32Range(Corrdinates.Y);
		nz = UNoiseGen::MakeInt32Range(Corrdinates.Z);

		// Get the coherent-noise value from the input value and add it to the
		// final result.
		seed = (Seed + curOctave) & 0xffffffff;
		signal = UNoiseGen::GradientCoherentNoise3D(FVector(nx, ny, nz), seed, NoiseQuality);
		signal = 2.0 * fabs(signal) - 1.0;
		value += signal * curPersistence;

		// Prepare the next octave.
		Corrdinates.X *= Lacunarity;
		Corrdinates.Y *= Lacunarity;
		Corrdinates.Z *= Lacunarity;
		curPersistence *= Persistence;
	}
	value += 0.5;

	return value;
}