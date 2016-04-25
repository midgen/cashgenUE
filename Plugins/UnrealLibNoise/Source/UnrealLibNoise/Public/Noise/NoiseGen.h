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

#include "Object.h"
#include "NoiseGen.generated.h"

/// @addtogroup UnrealLibNoise
/// @{

const int X_NOISE_GEN = 1619;
const int Y_NOISE_GEN = 31337;
const int Z_NOISE_GEN = 6971;
const int SEED_NOISE_GEN = 1013;
const int SHIFT_NOISE_GEN = 8;

/**
* This enum list the different levels of quality when generating noise. This directly relates to which interpolation is used
* to smooth the noise.
*/
UENUM(BlueprintType)
enum class FNoiseQuality : uint8
{
	/**
	* Generates coherent noise quickly.  When a coherent-noise function with
	* this quality setting is used to generate a bump-map image, there are
	* noticeable "creasing" artifacts in the resulting image.  This is
	* because the derivative of that function is discontinuous at integer
	* boundaries.
	*/
	QUALITY_FAST,

	/**
	* Generates standard-quality coherent noise.  When a coherent-noise
	* function with this quality setting is used to generate a bump-map
	* image, there are some minor "creasing" artifacts in the resulting
	* image.  This is because the second derivative of that function is
	* discontinuous at integer boundaries.
	*/
	QUALITY_STD,

	/** 
	* Generates the best-quality coherent noise.  When a coherent-noise
	* function with this quality setting is used to generate a bump-map
	* image, there are no "creasing" artifacts in the resulting image.  This
	* is because the first and second derivatives of that function are
	* continuous at integer boundaries.
	*/
	QUALITY_BEST
};

/**
*	This class contains various methods of generating noise; There are two types of noise generated: gradient and value.
*	Value noise is faster, but has less quality, where gradient provides a better range of noise but at the cost of speed.
*/
UCLASS(BlueprintType)
class UNREALLIBNOISE_API UNoiseGen : public UObject
{
	GENERATED_BODY()

public:	
	
	/**
	 *	This function generates a gradient coherent noise value from the coordinates of a three dimensional input value.
	 *
	 *	@param Coordinates - the input coordinates used to generate the noise.
	 *	@param Seed - The seed used for random number generation.
	 *	@param NoiseQuality - The quality setting that determines which method used to smooth the noise
	 * 
	 *	@return = Returns the generated noise value
	 */
	UFUNCTION(BlueprintCallable, Category = "Generation")
	static float GradientCoherentNoise3D(FVector Coordinates, int32 Seed = 0, FNoiseQuality NoiseQuality = FNoiseQuality::QUALITY_STD);
	
	/**
	*	This function generates a gradient noise value from the coordinates values of a three dimensional input and the
	*	integer coordinates of a nearby three dimensional value.
	*
	*	@param Coordinates - The input coordinates used to generate the noise.
	*	@param NearX - The x integer coordinate of a nearby value.
	*	@param NearY - The y integer coordinate of a nearby value.
	*	@param NearZ - The z integer coordinate of a nearby value.
	*	@param Seed - The seed used for random number generation.
	*
	*	@return Returns the generated noise value. It returns a value between -1.0 and 1.0.
	*
	*	@pre The difference between input coordinates and the integer coordinates must be less than or equal to one.
	*/
	UFUNCTION(BlueprintCallable, Category = "Generation")
	static float GradientNoise3D(FVector Coordinates, int32 NearX, int32 NearY, int32 NearZ, int32 Seed = 0);

	/**
	*	Generates an integer-noise value from the input values of a three-dimensional input.
	*
	*	@param X - The integer x coordinate of the input value.
	*	@param Y - The integer y coordinate of the input value.
	*	@param Z - The integer z coordinate of the input value.
	*	@param Seed - The seed used for random number generation.
	*
	*	@return Returns the generated noise value. The return value ranges from 0 to 2147483647.
	*/
	UFUNCTION(BlueprintCallable, Category = "Generation")
	static int32 IntValueNoise3D(int32 X, int32 Y, int32 Z, int32 Seed = 0);

	/**
	*	Modifies a floating point value so it can be stored in a int32 value.
	*
	*	@param Number - A floating-point number
	*
	*	@return Returns the modified floating point number
	*
	*	This function does not modify Number.
	*
	*	In libnoise, and so also in UnrealLibNoise, the noise generation algorithms are all intergerbased;
	*	they use variables of the type int32. Before calling a noise function, pass the x, y, and z coordinates
	*	to this function to ensure that these coordinates can be cast to int32.
	*/
	UFUNCTION(BlueprintCallable, Category = "Generation")
	static float MakeInt32Range(float Number);

	/*+
	+ Generates a value-coherent-noise value from the coordinates of a
	+ three-dimensional input value.
	*
	+ @param Coordinates The @a coordinates of the input value.
	+ @param Seed The random number seed.
	+ @param NoiseQuality The quality of the coherent-noise.
	+
	+ @returns The generated value-coherent-noise value.
	*/
	UFUNCTION(BlueprintCallable, Category = "Generation")
	static float ValueCoherentNoise3D(FVector Coordinates, int32 Seed = 0, FNoiseQuality noiseQuality = FNoiseQuality::QUALITY_STD);

	/**
	*	Generates a value-noise value from the coordinates of a
	*	three-dimensional input value.
	*
	*	@param X The @a x coordinate of the input value.
	*	@param Y The @a y coordinate of the input value.
	*	@param Z The @a z coordinate of the input value.
	*	@param Seed A random number seed.
	*
	*	@returns The generated value-noise value.
	*
	*	The return value ranges from -1.0 to +1.0.
	*
	*	A noise function differs from a random-number generator because it
	*	always returns the same output value if the same input value is passed
	*	to it.
	*/
	UFUNCTION(BlueprintCallable, Category = "Generation")
	static float ValueNoise3D(int32 X, int32 Y, int32 Z, int32 Seed = 0);
};
/// @}
