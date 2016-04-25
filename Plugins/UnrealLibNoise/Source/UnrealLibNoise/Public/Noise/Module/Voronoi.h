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
#include "Voronoi.generated.h"

/// @addtogroup UnrealLibNoise
/// @{

/// @addtogroup modules
/// @{

/// @addtogroup generatormodules
/// @{

/// Default displacement to apply to each cell for the
/// noise::module::Voronoi noise module.
const float DEFAULT_VORONOI_DISPLACEMENT = 1.0;

/// Default frequency of the seed points for the noise::module::Voronoi
/// noise module.
const float DEFAULT_VORONOI_FREQUENCY = 1.0;

/// Default seed of the noise function for the noise::module::Voronoi
/// noise module.
const int32 DEFAULT_VORONOI_SEED = 0;

/// Noise module that outputs Voronoi cells.
///
///
/// In mathematics, a <i>Voronoi cell</i> is a region containing all the
/// points that are closer to a specific <i>seed point</i> than to any
/// other seed point.  These cells mesh with one another, producing
/// polygon-like formations.
///
/// By default, this noise module randomly places a seed point within
/// each unit cube.  By modifying the <i>frequency</i> of the seed points,
/// an application can change the distance between seed points.  The
/// higher the frequency, the closer together this noise module places
/// the seed points, which reduces the size of the cells.  To specify the
/// frequency of the cells, call the SetFrequency() method.
///
/// This noise module assigns each Voronoi cell with a random constant
/// value from a coherent-noise function.  The <i>displacement value</i>
/// controls the range of random values to assign to each cell.  The
/// range of random values is +/- the displacement value.  Call the
/// SetDisplacement() method to specify the displacement value.
///
/// To modify the random positions of the seed points, call the SetSeed()
/// method.
///
/// This noise module can optionally add the distance from the nearest
/// seed to the output value.  To enable this feature, call the
/// EnableDistance() method.  This causes the points in the Voronoi cells
/// to increase in value the further away that point is from the nearest
/// seed point.
///
/// Voronoi cells are often used to generate cracked-mud terrain
/// formations or crystal-like textures
///
/// This noise module requires no source modules.
UCLASS(BlueprintType)
class UNREALLIBNOISE_API UVoronoi : public UNoiseModule
{
	GENERATED_UCLASS_BODY()
	
public:

	/// Enables or disables applying the distance from the nearest seed
	/// point to the output value.
	///
	/// @param enable Specifies whether to apply the distance to the
	/// output value or not.
	///
	/// Applying the distance from the nearest seed point to the output
	/// value causes the points in the Voronoi cells to increase in value
	/// the further away that point is from the nearest seed point.
	/// Setting this value to @a true (and setting the displacement to a
	/// near-zero value) causes this noise module to generate cracked mud
	/// formations.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void EnableDistance(bool enable = true)
	{
		ShouldEnableDistance = enable;
	}

	/// Returns the displacement value of the Voronoi cells.
	///
	/// @returns The displacement value of the Voronoi cells.
	///
	/// This noise module assigns each Voronoi cell with a random constant
	/// value from a coherent-noise function.  The <i>displacement
	/// value</i> controls the range of random values to assign to each
	/// cell.  The range of random values is +/- the displacement value.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	float GetDisplacement() const
	{
		return Displacement;
	}

	/// Returns the frequency of the seed points.
	///
	/// @returns The frequency of the seed points.
	///
	/// The frequency determines the size of the Voronoi cells and the
	/// distance between these cells.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	float GetFrequency() const
	{
		return Frequency;
	}

	UFUNCTION(BlueprintCallable, Category = "Generation")
	virtual int32 GetSourceModuleCount() const
	{
		return 0;
	}

	/// Returns the seed value used by the Voronoi cells
	///
	/// @returns The seed value.
	///
	/// The positions of the seed values are calculated by a
	/// coherent-noise function.  By modifying the seed value, the output
	/// of that function changes.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	int32 GetSeed() const
	{
		return Seed;
	}

	/// Determines if the distance from the nearest seed point is applied
	/// to the output value.
	///
	/// @returns
	/// - @a true if the distance is applied to the output value.
	/// - @a false if not.
	///
	/// Applying the distance from the nearest seed point to the output
	/// value causes the points in the Voronoi cells to increase in value
	/// the further away that point is from the nearest seed point.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	bool IsDistanceEnabled() const
	{
		return ShouldEnableDistance;
	}

	UFUNCTION(BlueprintCallable, Category = "Generation")
	virtual float GetValue(FVector Coordinates);

	/// Sets the displacement value of the Voronoi cells.
	///
	/// @param displacement The displacement value of the Voronoi cells.
	///
	/// This noise module assigns each Voronoi cell with a random constant
	/// value from a coherent-noise function.  The <i>displacement
	/// value</i> controls the range of random values to assign to each
	/// cell.  The range of random values is +/- the displacement value.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetDisplacement(float displacement)
	{
		Displacement = displacement;
	}

	/// Sets the frequency of the seed points.
	///
	/// @param frequency The frequency of the seed points.
	///
	/// The frequency determines the size of the Voronoi cells and the
	/// distance between these cells.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetFrequency(float frequency)
	{
		Frequency = frequency;
	}

	/// Sets the seed value used by the Voronoi cells
	///
	/// @param seed The seed value.
	///
	/// The positions of the seed values are calculated by a
	/// coherent-noise function.  By modifying the seed value, the output
	/// of that function changes.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetSeed(int32 seed)
	{
		Seed = seed;
	}

protected:

	/// Scale of the random displacement to apply to each Voronoi cell.
	double Displacement;

	/// Determines if the distance from the nearest seed point is applied to
	/// the output value.
	bool ShouldEnableDistance;

	/// Frequency of the seed points.
	double Frequency;

	/// Seed value used by the coherent-noise function to determine the
	/// positions of the seed points.
	int Seed;

};

/// @}

/// @}

/// @}