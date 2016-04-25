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
#include "Const.generated.h"

/// @addtogroup UnrealLibNoise
/// @{

/// @addtogroup modules
/// @{

/// @defgroup generatormodules Generator Modules
/// @addtogroup generatormodules
/// @{

/// Default constant value for the noise::module::Const noise module.
const float DEFAULT_CONST_VALUE = 0.0;

/// Noise module that outputs a constant value.
///
///
/// To specify the constant value, call the SetConstValue() method.
///
/// This noise module is not useful by itself, but it is often used as a
/// source module for other noise modules.
///
/// This noise module does not require any source modules.
UCLASS(BlueprintType)
class UNREALLIBNOISE_API UConst : public UNoiseModule
{
	GENERATED_UCLASS_BODY()
	
public:

	/// Returns the constant output value for this noise module.
	///
	/// @returns The constant output value for this noise module.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	float GetConstValue() const
	{
		return ConstValue;
	}

	UFUNCTION(BlueprintCallable, Category = "Generation")
	virtual int32 GetSourceModuleCount() const
	{
		return 0;
	}

	UFUNCTION(BlueprintCallable, Category = "Generation")
	virtual float GetValue(FVector Coordinates)
	{
		return ConstValue;
	}

	/// Sets the constant output value for this noise module.
	///
	/// @param constValue The constant output value for this noise module.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetConstValue(float constValue)
	{
		ConstValue = constValue;
	}

protected:

	/// Constant value.
	float ConstValue;
	
	
};

/// @}

/// @}

/// @}
