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
#include "ScaleBias.generated.h"

/// @addtogroup UnrealLibNoise
/// @{

/// @addtogroup modules
/// @{

/// @addtogroup modifiermodules
/// @{

/// Default bias for the noise::module::ScaleBias noise module.
const float DEFAULT_BIAS = 0.0;

/// Default scale for the noise::module::ScaleBias noise module.
const float DEFAULT_SCALE = 1.0;

/// Noise module that applies a scaling factor and a bias to the output
/// value from a source module.
///
///
/// The GetValue() method retrieves the output value from the source
/// module, multiplies it with a scaling factor, adds a bias to it, then
/// outputs the value.
///
/// This noise module requires one source module.
UCLASS(BlueprintType)
class UNREALLIBNOISE_API UScaleBias : public UNoiseModule
{
	GENERATED_UCLASS_BODY()
	
public:



	/// Returns the bias to apply to the scaled output value from the
	/// source module.
	///
	/// @returns The bias to apply.
	///
	/// The GetValue() method retrieves the output value from the source
	/// module, multiplies it with the scaling factor, adds the bias to
	/// it, then outputs the value.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	float GetBias() const
	{
		return Bias;
	}

	/// Returns the scaling factor to apply to the output value from the
	/// source module.
	///
	/// @returns The scaling factor to apply.
	///
	/// The GetValue() method retrieves the output value from the source
	/// module, multiplies it with the scaling factor, adds the bias to
	/// it, then outputs the value.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	float GetScale() const
	{
		return Scale;
	}

	UFUNCTION(BlueprintCallable, Category = "Generation")
	virtual int32 GetSourceModuleCount() const
	{
		return 1;
	}

	UFUNCTION(BlueprintCallable, Category = "Generation")
	virtual float GetValue(FVector Coordinates);

	/// Sets the bias to apply to the scaled output value from the source
	/// module.
	///
	/// @param bias The bias to apply.
	///
	/// The GetValue() method retrieves the output value from the source
	/// module, multiplies it with the scaling factor, adds the bias to
	/// it, then outputs the value.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetBias(float bias)
	{
		Bias = bias;
	}

	/// Sets the scaling factor to apply to the output value from the
	/// source module.
	///
	/// @param scale The scaling factor to apply.
	///
	/// The GetValue() method retrieves the output value from the source
	/// module, multiplies it with the scaling factor, adds the bias to
	/// it, then outputs the value.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetScale(float scale)
	{
		Scale = scale;
	}

protected:

	/// Bias to apply to the scaled output value from the source module.
	float Bias;

	/// Scaling factor to apply to the output value from the source
	/// module.
	float Scale;

};

/// @}

/// @}

/// @}