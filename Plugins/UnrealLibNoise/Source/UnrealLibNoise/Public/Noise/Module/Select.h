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
#include "Select.generated.h"

/// @addtogroup UnrealLibNoise
/// @{

/// @addtogroup modules
/// @{

/// @addtogroup selectormodules
/// @{

/// Default edge-falloff value for the noise::module::Select noise module.
const float DEFAULT_SELECT_EDGE_FALLOFF = 0.0;

/// Default lower bound of the selection range for the
/// noise::module::Select noise module.
const float DEFAULT_SELECT_LOWER_BOUND = -1.0;

/// Default upper bound of the selection range for the
/// noise::module::Select noise module.
const float DEFAULT_SELECT_UPPER_BOUND = 1.0;

/// Noise module that outputs the value selected from one of two source
/// modules chosen by the output value from a control module.
///
///
/// Unlike most other noise modules, the index value assigned to a source
/// module determines its role in the selection operation:
/// - Source module 0 (upper left in the diagram) outputs a value.
/// - Source module 1 (lower left in the diagram) outputs a value.
/// - Source module 2 (bottom of the diagram) is known as the <i>control
///   module</i>.  The control module determines the value to select.  If
///   the output value from the control module is within a range of values
///   known as the <i>selection range</i>, this noise module outputs the
///   value from the source module with an index value of 1.  Otherwise,
///   this noise module outputs the value from the source module with an
///   index value of 0.
///
/// To specify the bounds of the selection range, call the SetBounds()
/// method.
///
/// An application can pass the control module to the SetControlModule()
/// method instead of the SetSourceModule() method.  This may make the
/// application code easier to read.
///
/// By default, there is an abrupt transition between the output values
/// from the two source modules at the selection-range boundary.  To
/// smooth the transition, pass a non-zero value to the SetEdgeFalloff()
/// method.  Higher values result in a smoother transition.
///
/// This noise module requires three source modules.
UCLASS(BlueprintType)
class UNREALLIBNOISE_API USelect : public UNoiseModule
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
	/// @throw noise::ExceptionNoModule See the preconditions for more
	/// information.
	///
	/// The control module determines the output value to select.  If the
	/// output value from the control module is within a range of values
	/// known as the <i>selection range</i>, the GetValue() method outputs
	/// the value from the source module with an index value of 1.
	/// Otherwise, this method outputs the value from the source module
	/// with an index value of 0.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	UNoiseModule* GetControlModule()
	{
		if (IsValid(GetSourceModule(0)) || IsValid(GetSourceModule(2))) {
			throw ExceptionNoModule();
		}
		return GetSourceModule(2);
	}

	/// Returns the falloff value at the edge transition.
	///
	/// @returns The falloff value at the edge transition.
	///
	/// The falloff value is the width of the edge transition at either
	/// edge of the selection range.
	///
	/// By default, there is an abrupt transition between the output
	/// values from the two source modules at the selection-range
	/// boundary.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	float GetEdgeFalloff() const
	{
		return EdgeFalloff;
	}

	/// Returns the lower bound of the selection range.
	///
	/// @returns The lower bound of the selection range.
	///
	/// If the output value from the control module is within the
	/// selection range, the GetValue() method outputs the value from the
	/// source module with an index value of 1.  Otherwise, this method
	/// outputs the value from the source module with an index value of 0.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	float GetLowerBound()
	{
		return LowerBound;
	}

	UFUNCTION(BlueprintCallable, Category = "Generation")
	virtual int32 GetSourceModuleCount() const
	{
		return 3;
	}

	/// Returns the upper bound of the selection range.
	///
	/// @returns The upper bound of the selection range.
	///
	/// If the output value from the control module is within the
	/// selection range, the GetValue() method outputs the value from the
	/// source module with an index value of 1.  Otherwise, this method
	/// outputs the value from the source module with an index value of 0.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	float GetUpperBound() const
	{
		return UpperBound;
	}

	UFUNCTION(BlueprintCallable, Category = "Generation")
	virtual float GetValue(FVector Coordinates);

	/// Sets the lower and upper bounds of the selection range.
	///
	/// @param lowerBound The lower bound.
	/// @param upperBound The upper bound.
	///
	/// @pre The lower bound must be less than or equal to the upper
	/// bound.
	///
	/// @throw noise::ExceptionInvalidParam An invalid parameter was
	/// specified; see the preconditions for more information.
	///
	/// If the output value from the control module is within the
	/// selection range, the GetValue() method outputs the value from the
	/// source module with an index value of 1.  Otherwise, this method
	/// outputs the value from the source module with an index value of 0.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetBounds(float lowerBound, float upperBound);

	/// Sets the control module.
	///
	/// @param controlModule The control module.
	///
	/// The control module determines the output value to select.  If the
	/// output value from the control module is within a range of values
	/// known as the <i>selection range</i>, the GetValue() method outputs
	/// the value from the source module with an index value of 1.
	/// Otherwise, this method outputs the value from the source module
	/// with an index value of 0.
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
	void SetControlModule(UNoiseModule* controlModule)
	{
		//if (IsValid(GetSourceModule(0))) throw ExceptionNoModule();
		SetSourceModule(2, controlModule);
	}

	/// Sets the falloff value at the edge transition.
	///
	/// @param edgeFalloff The falloff value at the edge transition.
	///
	/// The falloff value is the width of the edge transition at either
	/// edge of the selection range.
	///
	/// By default, there is an abrupt transition between the values from
	/// the two source modules at the boundaries of the selection range.
	///
	/// For example, if the selection range is 0.5 to 0.8, and the edge
	/// falloff value is 0.1, then the GetValue() method outputs:
	/// - the output value from the source module with an index value of 0
	///   if the output value from the control module is less than 0.4
	///   ( = 0.5 - 0.1).
	/// - a linear blend between the two output values from the two source
	///   modules if the output value from the control module is between
	///   0.4 ( = 0.5 - 0.1) and 0.6 ( = 0.5 + 0.1).
	/// - the output value from the source module with an index value of 1
	///   if the output value from the control module is between 0.6
	///   ( = 0.5 + 0.1) and 0.7 ( = 0.8 - 0.1).
	/// - a linear blend between the output values from the two source
	///   modules if the output value from the control module is between
	///   0.7 ( = 0.8 - 0.1 ) and 0.9 ( = 0.8 + 0.1).
	/// - the output value from the source module with an index value of 0
	///   if the output value from the control module is greater than 0.9
	///   ( = 0.8 + 0.1).
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetEdgeFalloff(float edgeFalloff);

protected:

	/// Edge-falloff value.
	double EdgeFalloff;

	/// Lower bound of the selection range.
	double LowerBound;

	/// Upper bound of the selection range.
	double UpperBound;

};

/// @}

/// @}

/// @}
