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
#include "Cylinders.generated.h"

/// @addtogroup UnrealLibNoise
/// @{

/// @addtogroup modules
/// @{

/// @addtogroup generatormodules
/// @{

/// Default frequency value for the noise::module::Cylinders noise module.
const float DEFAULT_CYLINDERS_FREQUENCY = 1.0;

/// Noise module that outputs concentric cylinders.
///
///
/// This noise module outputs concentric cylinders centered on the origin.
/// These cylinders are oriented along the @a y axis similar to the
/// concentric rings of a tree.  Each cylinder extends infinitely along
/// the @a y axis.
///
/// The first cylinder has a radius of 1.0.  Each subsequent cylinder has
/// a radius that is 1.0 unit larger than the previous cylinder.
///
/// The output value from this noise module is determined by the distance
/// between the input value and the the nearest cylinder surface.  The
/// input values that are located on a cylinder surface are given the
/// output value 1.0 and the input values that are equidistant from two
/// cylinder surfaces are given the output value -1.0.
///
/// An application can change the frequency of the concentric cylinders.
/// Increasing the frequency reduces the distances between cylinders.  To
/// specify the frequency, call the SetFrequency() method.
///
/// This noise module, modified with some low-frequency, low-power
/// turbulence, is useful for generating wood-like textures.
///
/// This noise module does not require any source modules.
UCLASS(BlueprintType)
class UNREALLIBNOISE_API UCylinders : public UNoiseModule
{
	GENERATED_UCLASS_BODY()
	
public:
		
	/// Returns the frequency of the concentric cylinders.
	///
	/// @returns The frequency of the concentric cylinders.
	///
	/// Increasing the frequency increases the density of the concentric
	/// cylinders, reducing the distances between them.
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

	UFUNCTION(BlueprintCallable, Category = "Generation")
	virtual float GetValue(FVector Coordinates);

	/// Sets the frequency of the concentric cylinders.
	///
	/// @param frequency The frequency of the concentric cylinders.
	///
	/// Increasing the frequency increases the density of the concentric
	/// cylinders, reducing the distances between them.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetFrequency(float frequency)
	{
		Frequency = frequency;
	}

protected:

	/// Frequency of the concentric cylinders.
	float Frequency;

};

/// @}

/// @}

/// @}
