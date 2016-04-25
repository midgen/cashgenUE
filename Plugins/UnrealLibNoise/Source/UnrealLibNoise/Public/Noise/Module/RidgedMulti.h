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

#pragma once

#include "Noise/Module/NoiseModule.h"
#include "../Noise.h"
#include "RidgedMulti.generated.h"

/// @addtogroup UnrealLibNoise
/// @{

/// @addtogroup modules
/// @{

/// @addtogroup generatormodules
/// @{

/// Default frequency for the RidgedMulti noise module.
const float DEFAULT_RIDGED_FREQUENCY = 1.0;

/// Default lacunarity for the RidgedMulti noise module.
const float DEFAULT_RIDGED_LACUNARITY = 2.0;

/// Default number of octaves for the RidgedMulti noise
/// module.
const int32 DEFAULT_RIDGED_OCTAVE_COUNT = 6;

/// Default noise quality for the RidgedMulti noise
/// module.
const FNoiseQuality DEFAULT_RIDGED_QUALITY = FNoiseQuality::QUALITY_STD;

/// Default noise seed for the RidgedMulti noise module.
const int32 DEFAULT_RIDGED_SEED = 0;

/// Maximum number of octaves for the RidgedMulti noise
/// module.
const int32 RIDGED_MAX_OCTAVE = 30;

/// Noise module that outputs 3-dimensional ridged-multifractal noise.
///
///
/// This noise module, heavily based on the Perlin-noise module, generates
/// ridged-multifractal noise.  Ridged-multifractal noise is generated in
/// much of the same way as Perlin noise, except the output of each octave
/// is modified by an absolute-value function.  Modifying the octave
/// values in this way produces ridge-like formations.
///
/// Ridged-multifractal noise does not use a persistence value.  This is
/// because the persistence values of the octaves are based on the values
/// generated from from previous octaves, creating a feedback loop (or
/// that's what it looks like after reading the code.)
///
/// This noise module outputs ridged-multifractal-noise values that
/// usually range from -1.0 to +1.0, but there are no guarantees that all
/// output values will exist within that range.
///
/// @note For ridged-multifractal noise generated with only one octave,
/// the output value ranges from -1.0 to 0.0.
///
/// Ridged-multifractal noise is often used to generate craggy mountainous
/// terrain or marble-like textures.
///
/// This noise module does not require any source modules.
///
/// <b>Octaves</b>
///
/// The number of octaves control the <i>amount of detail</i> of the
/// ridged-multifractal noise.  Adding more octaves increases the detail
/// of the ridged-multifractal noise, but with the drawback of increasing
/// the calculation time.
///
/// An application may specify the number of octaves that generate
/// ridged-multifractal noise by calling the SetOctaveCount() method.
///
/// <b>Frequency</b>
///
/// An application may specify the frequency of the first octave by
/// calling the SetFrequency() method.
///
/// <b>Lacunarity</b>
///
/// The lacunarity specifies the frequency multipler between successive
/// octaves.
///
/// The effect of modifying the lacunarity is subtle; you may need to play
/// with the lacunarity value to determine the effects.  For best results,
/// set the lacunarity to a number between 1.5 and 3.5.
///
/// <b>References &amp; Acknowledgments</b>
///
/// <a href=http://www.texturingandmodeling.com/Musgrave.html>F.
/// Kenton "Doc Mojo" Musgrave's texturing page</a> - This page contains
/// links to source code that generates ridged-multfractal noise, among
/// other types of noise.  The source file <a
/// href=http://www.texturingandmodeling.com/CODE/MUSGRAVE/CLOUD/fractal.c>
/// fractal.c</a> contains the code I used in my ridged-multifractal class
/// (see the @a RidgedMultifractal() function.)  This code was written by F.
/// Kenton Musgrave, the person who created
/// <a href=http://www.pandromeda.com/>MojoWorld</a>.  He is also one of
/// the authors in <i>Texturing and Modeling: A Procedural Approach</i>
/// (Morgan Kaufmann, 2002. ISBN 1-55860-848-6.)
UCLASS(BlueprintType)
class UNREALLIBNOISE_API URidgedMulti : public UNoiseModule
{
	GENERATED_UCLASS_BODY()
	
public:

	/// Returns the frequency of the first octave.
	///
	/// @returns The frequency of the first octave.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	float GetFrequency()
	{
		return Frequency;
	}

	/// Returns the lacunarity of the ridged-multifractal noise.
	///
	/// @returns The lacunarity of the ridged-multifractal noise.
	/// 
	/// The lacunarity is the frequency multiplier between successive
	/// octaves.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	float GetLacunarity()
	{
		return Lacunarity;
	}

	/// Returns the quality of the ridged-multifractal noise.
	///
	/// @returns The quality of the ridged-multifractal noise.
	///
	/// FNoiseQuality for definitions of the various
	/// coherent-noise qualities.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	FNoiseQuality GetNoiseQuality()
	{
		return NoiseQuality;
	}

	/// Returns the number of octaves that generate the
	/// ridged-multifractal noise.
	///
	/// @returns The number of octaves that generate the
	/// ridged-multifractal noise.
	///
	/// The number of octaves controls the amount of detail in the
	/// ridged-multifractal noise.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	int32 GetOctaveCount()
	{
		return OctaveCount;
	}

	/// Returns the seed value used by the ridged-multifractal-noise
	/// function.
	///
	/// @returns The seed value.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	int32 GetSeed()
	{
		return Seed;
	}

	UFUNCTION(BlueprintCallable, Category = "Generation")
	virtual int32 GetSourceModuleCount() const
	{
		return 0;
	}

	UFUNCTION(BlueprintCallable, Category = "Generation")
	virtual float GetValue(FVector Coordinates);

	/// Sets the frequency of the first octave.
	///
	/// @param frequency The frequency of the first octave.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetFrequency(float frequency)
	{
		Frequency = frequency;
	}

	/// Sets the lacunarity of the ridged-multifractal noise.
	///
	/// @param lacunarity The lacunarity of the ridged-multifractal noise.
	/// 
	/// The lacunarity is the frequency multiplier between successive
	/// octaves.
	///
	/// For best results, set the lacunarity to a number between 1.5 and
	/// 3.5.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetLacunarity(float lacunarity)
	{
		Lacunarity = lacunarity;
		CalcSpectralWeights();
	}

	/// Sets the quality of the ridged-multifractal noise.
	///
	/// @param noiseQuality The quality of the ridged-multifractal noise.
	///
	/// See noise::NoiseQuality for definitions of the various
	/// coherent-noise qualities.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetNoiseQuality(FNoiseQuality noiseQuality)
	{
		NoiseQuality = noiseQuality;
	}

	/// Sets the number of octaves that generate the ridged-multifractal
	/// noise.
	///
	/// @param octaveCount The number of octaves that generate the
	/// ridged-multifractal noise.
	///
	/// @pre The number of octaves ranges from 1 to
	/// RIDGED_MAX_OCTAVE.
	///
	/// @throw ExceptionInvalidParam An invalid parameter was
	/// specified; see the preconditions for more information.
	///
	/// The number of octaves controls the amount of detail in the
	/// ridged-multifractal noise.
	///
	/// The larger the number of octaves, the more time required to
	/// calculate the ridged-multifractal-noise value.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetOctaveCount(int32 octaveCount)
	{
		if (octaveCount > RIDGED_MAX_OCTAVE) {
			throw ExceptionInvalidParam();
		}
		OctaveCount = octaveCount;
	}

	/// Sets the seed value used by the ridged-multifractal-noise
	/// function.
	///
	/// @param seed The seed value.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetSeed(int32 seed)
	{
		Seed = seed;
	}

protected:

	/// Calculates the spectral weights for each octave.
	///
	/// This method is called when the lacunarity changes.
	void CalcSpectralWeights();

	/// Frequency of the first octave.
	float Frequency;

	/// Frequency multiplier between successive octaves.
	float Lacunarity;

	/// Quality of the ridged-multifractal noise.
	FNoiseQuality NoiseQuality;

	/// Total number of octaves that generate the ridged-multifractal
	/// noise.
	int32 OctaveCount;

	/// Contains the spectral weights for each octave.
	float SpectralWeights[RIDGED_MAX_OCTAVE];

	/// Seed value used by the ridged-multfractal-noise function.
	int32 Seed;
	
};

/// @}

/// @}

/// @}