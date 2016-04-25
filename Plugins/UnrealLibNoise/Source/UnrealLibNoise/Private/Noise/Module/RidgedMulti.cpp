

#include "UnrealLibNoise.h"
#include "RidgedMulti.h"

/// Constructor.
///
/// The default number of octaves is set to
/// DEFAULT_RIDGED_OCTAVE_COUNT.
///
/// The default frequency is set to
/// DEFAULT_RIDGED_FREQUENCY.
///
/// The default lacunarity is set to
/// DEFAULT_RIDGED_LACUNARITY.
///
/// The default seed value is set to
/// DEFAULT_RIDGED_SEED.
URidgedMulti::URidgedMulti(const FObjectInitializer& ObjectInit) : Super(ObjectInit),
	Frequency(DEFAULT_RIDGED_FREQUENCY),
	Lacunarity(DEFAULT_RIDGED_LACUNARITY),
	NoiseQuality(DEFAULT_RIDGED_QUALITY),
	OctaveCount(DEFAULT_RIDGED_OCTAVE_COUNT),
	Seed(DEFAULT_RIDGED_SEED)
{
	CalcSpectralWeights();
}

// Calculates the spectral weights for each octave.
void URidgedMulti::CalcSpectralWeights()
{
	// This exponent parameter should be user-defined; it may be exposed in a
	// future version of libnoise.
	float h = 1.0;

	float frequency = 1.0;
	for (int i = 0; i < RIDGED_MAX_OCTAVE; i++) {
		// Compute weight for each frequency.
		SpectralWeights[i] = FMath::Pow(frequency, -h);
		frequency *= Lacunarity;
	}
}

// Multifractal code originally written by F. Kenton "Doc Mojo" Musgrave,
// 1998.  Modified by jas for use with libnoise. Modified by Matt Hamlin for Unreal
float URidgedMulti::GetValue(FVector Coordinates)
{
	Coordinates.X *= Frequency;
	Coordinates.Y *= Frequency;
	Coordinates.Z *= Frequency;

	float signal = 0.0;
	float value = 0.0;
	float weight = 1.0;

	// These parameters should be user-defined; they may be exposed in a
	// future version of libnoise.
	float offset = 1.0;
	float gain = 2.0;

	for (int curOctave = 0; curOctave < OctaveCount; curOctave++) {

		// Make sure that these floating-point values have the same range as a 32-
		// bit integer so that we can pass them to the coherent-noise functions.
		float nx, ny, nz;
		nx = UNoiseGen::MakeInt32Range(Coordinates.X);
		ny = UNoiseGen::MakeInt32Range(Coordinates.Y);
		nz = UNoiseGen::MakeInt32Range(Coordinates.Z);

		// Get the coherent-noise value.
		int seed = (Seed + curOctave) & 0x7fffffff;
		signal = UNoiseGen::GradientCoherentNoise3D(FVector(nx, ny, nz), seed, NoiseQuality);

		// Make the ridges.
		signal = FMath::Abs(signal);
		signal = offset - signal;

		// Square the signal to increase the sharpness of the ridges.
		signal *= signal;

		// The weighting from the previous octave is applied to the signal.
		// Larger values have higher weights, producing sharp points along the
		// ridges.
		signal *= weight;

		// Weight successive contributions by the previous signal.
		weight = signal * gain;
		if (weight > 1.0) {
			weight = 1.0;
		}
		if (weight < 0.0) {
			weight = 0.0;
		}

		// Add the signal to the output value.
		value += (signal * SpectralWeights[curOctave]);

		// Go to the next octave.
		Coordinates.X *= Lacunarity;
		Coordinates.Y *= Lacunarity;
		Coordinates.Z *= Lacunarity;
	}

	return (value * 1.25) - 1.0;
}


