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
#include "Blend.generated.h"

/// @addtogroup UnrealLibNoise
/// @{

/// @addtogroup modules
/// @{

/// @defgroup selectormodules Selector Modules
/// @addtogroup selectormodules
/// @{

/// Noise module that outputs a weighted blend of the output values from
/// two source modules given the output value supplied by a control module.
///
///
/// Unlike most other noise modules, the index value assigned to a source
/// module determines its role in the blending operation:
/// - Source module 0 (upper left in the diagram) outputs one of the
///   values to blend.
/// - Source module 1 (lower left in the diagram) outputs one of the
///   values to blend.
/// - Source module 2 (bottom of the diagram) is known as the <i>control
///   module</i>.  The control module determines the weight of the
///   blending operation.  Negative values weigh the blend towards the
///   output value from the source module with an index value of 0.
///   Positive values weigh the blend towards the output value from the
///   source module with an index value of 1.
///
/// An application can pass the control module to the SetControlModule()
/// method instead of the SetSourceModule() method.  This may make the
/// application code easier to read.
///
/// This noise module uses linear interpolation to perform the blending
/// operation.
///
/// This noise module requires three source modules.
UCLASS(BlueprintType)
class UNREALLIBNOISE_API UBlend : public UNoiseModule
{
	GENERATED_UCLASS_BODY()
	
public:

	/// Returns the control module.
	///
	/// @returns A reference to the control module.
	///
	/// @pre A control module has been added to this noise module via a
	/// call to SetSourceModule() or SetControlModule().
	///
	/// @throw ExceptionNoModule See the preconditions for more
	/// information.
	///
	/// The control module determines the weight of the blending
	/// operation.  Negative values weigh the blend towards the output
	/// value from the source module with an index value of 0.  Positive
	/// values weigh the blend towards the output value from the source
	/// module with an index value of 1.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	UNoiseModule* GetControlModule()
	{
		if (GetSourceModule(0) == NULL || GetSourceModule(1) == NULL || GetSourceModule(2) == NULL) {
			throw ExceptionNoModule();
		}

		return GetSourceModule(2);
	}


	/// Sets the control module.
	///
	/// @param ControlModule The control module.
	///
	/// The control module determines the weight of the blending
	/// operation.  Negative values weigh the blend towards the output
	/// value from the source module with an index value of 0.  Positive
	/// values weigh the blend towards the output value from the source
	/// module with an index value of 1.
	///
	/// This method assigns the control module an index value of 2.
	/// Passing the control module to this method produces the same
	/// results as passing the control module to the SetSourceModule()
	/// method while assigning that noise module an index value of 2.
	///
	/// This control module must exist throughout the lifetime of this
	/// noise module unless another control module replaces that control
	/// module.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetControlModule(UNoiseModule* ControlModule)
	{
		if (GetSourceModule(0) == NULL || GetSourceModule(1) == NULL || GetSourceModule(2) == NULL || ControlModule == NULL) {
			throw ExceptionNoModule();
		}

		SetSourceModule(2, ControlModule);
	}
	
	UFUNCTION(BlueprintCallable, Category = "Generation")
	virtual int32 GetSourceModuleCount() const { return 3; }

	UFUNCTION(BlueprintCallable, Category = "Generation")
	virtual float GetValue(FVector Coordinates);

};


/// @}

/// @}

/// @}
