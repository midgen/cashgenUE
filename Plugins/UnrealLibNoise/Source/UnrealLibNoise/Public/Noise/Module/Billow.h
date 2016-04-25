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

#include "NoiseModule.h"
#include "../NoiseGen.h"
#include "../NoiseInterp.h"
#include "Billow.generated.h"

/// @addtogroup UnrealLibNoise
/// @{

/// @addtogroup modules
/// @{

/// @addtogroup generatormodules
/// @{

/// Default frequency for the Billow noise module.
const float DEFAULT_BILLOW_FREQUENCY = 1.0;

/// Default lacunarity for the the Billow noise module.
const float DEFAULT_BILLOW_LACUNARITY = 2.0;

/// Default number of octaves for the the Billow noise
/// module.
const int32 DEFAULT_BILLOW_OCTAVE_COUNT = 6;

/// Default persistence value for the the Billow noise
/// module.
const float DEFAULT_BILLOW_PERSISTENCE = 0.5;

/// Default noise quality for the the Billow noise module.
const FNoiseQuality DEFAULT_BILLOW_QUALITY = FNoiseQuality::QUALITY_STD;

/// Default noise seed for the the Billow noise module.
const int32 DEFAULT_BILLOW_SEED = 0;

/// Maximum number of octaves for the the Billow noise
/// module.
const int32 BILLOW_MAX_OCTAVE = 30;

/// Noise module that outputs three-dimensional "billowy" noise.
///
/// This noise module generates "billowy" noise suitable for clouds and
/// rocks.
///
/// This noise module is nearly identical to Perlin except
/// this noise module modifies each octave with an absolute-value
/// function.  See the documentation Perlin for more
/// information.
UCLASS(BlueprintType)
class UNREALLIBNOISE_API UBillow : public UNoiseModule
{
	GENERATED_UCLASS_BODY()

public:
	
	/// Returns the frequency of the first octave.
	///
	/// @returns The frequency of the first octave.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	float GetFrequency() const
	{
		return Frequency;
	}

	/// Returns the lacunarity of the billowy noise.
	///
	/// @returns The lacunarity of the billowy noise.
	/// 
	/// The lacunarity is the frequency multiplier between successive
	/// octaves.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	float GetLacunarity() const
	{
		return Lacunarity;
	}

	/// Returns the quality of the billowy noise.
	///
	/// @returns The quality of the billowy noise.
	///
	/// See noise::NoiseQuality for definitions of the various
	/// coherent-noise qualities.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	FNoiseQuality GetNoiseQuality() const
	{
		return NoiseQuality;
	}

	/// Returns the number of octaves that generate the billowy noise.
	///
	/// @returns The number of octaves that generate the billowy noise.
	///
	/// The number of octaves controls the amount of detail in the billowy
	/// noise.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	int32 GetOctaveCount() const
	{
		return OctaveCount;
	}

	/// Returns the persistence value of the billowy noise.
	///
	/// @returns The persistence value of the billowy noise.
	///
	/// The persistence value controls the roughness of the billowy noise.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	float GetPersistence() const
	{
		return Persistence;
	}

	/// Returns the seed value used by the billowy-noise function.
	///
	/// @returns The seed value.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	int32 GetSeed() const
	{
		return Seed;
	}

	UFUNCTION(BlueprintCallable, Category = "Generation")
	virtual int32 GetSourceModuleCount() const
	{
		return 0;
	}

	UFUNCTION(BlueprintCallable, Category = "Generation")
	virtual float GetValue(FVector Corrdinates) override;

	/// Sets the frequency of the first octave.
	///
	/// @param frequency The frequency of the first octave.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetFrequency(float frequency)
	{
		Frequency = frequency;
	}

	/// Sets the lacunarity of the billowy noise.
	///
	/// @param lacunarity The lacunarity of the billowy noise.
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
	}

	/// Sets the quality of the billowy noise.
	///
	/// @param noiseQuality The quality of the billowy noise.
	///
	/// See noise::NoiseQuality for definitions of the various
	/// coherent-noise qualities.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetNoiseQuality(FNoiseQuality noiseQuality)
	{
		NoiseQuality = noiseQuality;
	}

	/// Sets the number of octaves that generate the billowy noise.
	///
	/// @param octaveCount The number of octaves that generate the billowy
	/// noise.
	///
	/// @pre The number of octaves ranges from 1 to
	/// noise::module::BILLOW_MAX_OCTAVE.
	///
	/// @throw noise::ExceptionInvalidParam An invalid parameter was
	/// specified; see the preconditions for more information.
	///
	/// The number of octaves controls the amount of detail in the billowy
	/// noise.
	///
	/// The larger the number of octaves, the more time required to
	/// calculate the billowy-noise value.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetOctaveCount(int32 octaveCount)
	{
		if (octaveCount < 1 || octaveCount > BILLOW_MAX_OCTAVE) {
			throw ExceptionInvalidParam();
		}
		OctaveCount = octaveCount;
	}

	/// Sets the persistence value of the billowy noise.
	///
	/// @param persistence The persistence value of the billowy noise.
	///
	/// The persistence value controls the roughness of the billowy noise.
	///
	/// For best results, set the persistence value to a number between
	/// 0.0 and 1.0.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetPersistence(float persistence)
	{
		Persistence = persistence;
	}

	/// Sets the seed value used by the billowy-noise function.
	///
	/// @param seed The seed value.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetSeed(int32 seed)
	{
		Seed = seed;
	}

protected:

	/// Frequency of the first octave.
	float Frequency;

	/// Frequency multiplier between successive octaves.
	float Lacunarity;

	/// Quality of the billowy noise.
	FNoiseQuality NoiseQuality;

	/// Total number of octaves that generate the billowy noise.
	int32 OctaveCount;

	/// Persistence value of the billowy noise.
	float Persistence;

	/// Seed value used 
	int32 Seed;

};

/// @}

/// @}

/// @}