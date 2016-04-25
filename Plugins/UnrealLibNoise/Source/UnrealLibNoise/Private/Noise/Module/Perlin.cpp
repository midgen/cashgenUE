// Original Code Copyright (C) 2003, 2004 Jason Bevins
// Unreal Adaptation (C) 2016 Matthew Hamlin
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License (COPYING.txt) for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Original Developers Email jlbevins@gmail.com
// Email: DarkTreePrd@gmail.com
//

#include "UnrealLibNoise.h"
#include "Perlin.h"

/// Constructor.
///
/// The default frequency is set to
/// DEFAULT_PERLIN_FREQUENCY.
///
/// The default lacunarity is set to
/// DEFAULT_PERLIN_LACUNARITY.
///
/// The default number of octaves is set to
/// DEFAULT_PERLIN_OCTAVE_COUNT.
///
/// The default persistence value is set to
/// DEFAULT_PERLIN_PERSISTENCE.
///
/// The default seed value is set to
/// DEFAULT_PERLIN_SEED.
UPerlin::UPerlin(const FObjectInitializer& ObjectInit) : Super(ObjectInit)
{
	Frequency = DEFAULT_PERLIN_FREQUENCY;
	Lacunarity = DEFAULT_PERLIN_LACUNARITY;
	NoiseQuality = DEFAULT_PERLIN_QUALITY;
	OctaveCount = DEFAULT_PERLIN_OCTAVE_COUNT;
	Persistence = DEFAULT_PERLIN_PERSISTENCE;
	Seed = DEFAULT_PERLIN_SEED;
}

float UPerlin::GetValue(FVector Coordinates)
{
	float Value = 0.0;
	float Signal = 0.0;
	float CurrentPersistence = 1.0;
	float nx, ny, nz;
	int32 seed;

	Coordinates.X *= Frequency;
	Coordinates.Y *= Frequency;
	Coordinates.Z += Frequency;

	for (int curOctave = 0; curOctave < OctaveCount; curOctave++)
	{
		// Make sure that these floating-point values have the same range as a 32-
		// bit integer so that we can pass them to the coherent-noise functions.
		nx = UNoiseGen::MakeInt32Range(Coordinates.X);
		ny = UNoiseGen::MakeInt32Range(Coordinates.Y);
		nz = UNoiseGen::MakeInt32Range(Coordinates.Z);

		// Get the coherent-noise value from the input value and add it to the
		// final result.
		seed = (Seed + curOctave) & 0xffffffff;
		Signal = UNoiseGen::GradientCoherentNoise3D(FVector(nx, ny, nz), seed, NoiseQuality);
		Value += Signal * CurrentPersistence;

		// Prepare the next octave.
		Coordinates.X *= Lacunarity;
		Coordinates.Y *= Lacunarity;
		Coordinates.Z *= Lacunarity;
		CurrentPersistence *= Persistence;
	}

	return Value;
}