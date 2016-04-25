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
#include "Exponent.generated.h"

/// @addtogroup UnrealLibNoise
/// @{

/// @addtogroup modules
/// @{

/// @addtogroup modifiermodules
/// @{

/// Default exponent for the noise::module::Exponent noise module.
const float DEFAULT_EXPONENT = 1.0;

/// Noise module that maps the output value from a source module onto an
/// exponential curve.
///
///
/// Because most noise modules will output values that range from -1.0 to
/// +1.0, this noise module first normalizes this output value (the range
/// becomes 0.0 to 1.0), maps that value onto an exponential curve, then
/// rescales that value back to the original range.
///
/// This noise module requires one source module.
UCLASS(BlueprintType)
class UNREALLIBNOISE_API UExponent : public UNoiseModule
{
	GENERATED_UCLASS_BODY()
	
public:



	/// Returns the exponent value to apply to the output value from the
	/// source module.
	///
	/// @returns The exponent value.
	///
	/// Because most noise modules will output values that range from -1.0
	/// to +1.0, this noise module first normalizes this output value (the
	/// range becomes 0.0 to 1.0), maps that value onto an exponential
	/// curve, then rescales that value back to the original range.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	float GetExponent() const
	{
		return Exponent;
	}

	UFUNCTION(BlueprintCallable, Category = "Generation")
	virtual int32 GetSourceModuleCount() const
	{
		return 1;
	}

	UFUNCTION(BlueprintCallable, Category = "Generation")
	virtual float GetValue(FVector Coordinates);

	/// Sets the exponent value to apply to the output value from the
	/// source module.
	///
	/// @param exponent The exponent value.
	///
	/// Because most noise modules will output values that range from -1.0
	/// to +1.0, this noise module first normalizes this output value (the
	/// range becomes 0.0 to 1.0), maps that value onto an exponential
	/// curve, then rescales that value back to the original range.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetExponent(float exponent)
	{
		Exponent = exponent;
	}

protected:

	/// Exponent to apply to the output value from the source module.
	float Exponent;
	
	
};


/// @}

/// @}

/// @}