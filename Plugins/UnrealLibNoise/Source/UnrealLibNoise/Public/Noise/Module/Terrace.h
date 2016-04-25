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
#include "Terrace.generated.h"

/// @addtogroup UnrealLibNoise
/// @{

/// @addtogroup modules
/// @{

/// @addtogroup modifiermodules
/// @{

/// Noise module that maps the output value from a source module onto a
/// terrace-forming curve.
///
///
/// This noise module maps the output value from the source module onto a
/// terrace-forming curve.  The start of this curve has a slope of zero;
/// its slope then smoothly increases.  This curve also contains
/// <i>control points</i> which resets the slope to zero at that point,
/// producing a "terracing" effect.  Refer to the following illustration:
///
///
/// To add a control point to this noise module, call the
/// AddControlPoint() method.
///
/// An application must add a minimum of two control points to the curve.
/// If this is not done, the GetValue() method fails.  The control points
/// can have any value, although no two control points can have the same
/// value.  There is no limit to the number of control points that can be
/// added to the curve.
///
/// This noise module clamps the output value from the source module if
/// that value is less than the value of the lowest control point or
/// greater than the value of the highest control point.
///
/// This noise module is often used to generate terrain features such as
/// your stereotypical desert canyon.
///
/// This noise module requires one source module.
UCLASS(BlueprintType)
class UNREALLIBNOISE_API UTerrace : public UNoiseModule
{
	GENERATED_UCLASS_BODY()

public:

	/// Adds a control point to the terrace-forming curve.
	///
	/// @param value The value of the control point to add.
	///
	/// @pre No two control points have the same value.
	///
	/// @throw noise::ExceptionInvalidParam An invalid parameter was
	/// specified; see the preconditions for more information.
	///
	/// Two or more control points define the terrace-forming curve.  The
	/// start of this curve has a slope of zero; its slope then smoothly
	/// increases.  At the control points, its slope resets to zero.
	///
	/// It does not matter which order these points are added.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void AddControlPoint(float value);

	/// Deletes all the control points on the terrace-forming curve.
	///
	/// @post All control points on the terrace-forming curve are deleted.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void ClearAllControlPoints();

	/// Returns a pointer to the array of control points on the
	/// terrace-forming curve.
	///
	/// @returns A pointer to the array of control points in this noise
	/// module.
	///
	/// Two or more control points define the terrace-forming curve.  The
	/// start of this curve has a slope of zero; its slope then smoothly
	/// increases.  At the control points, its slope resets to zero.
	///
	/// Before calling this method, call GetControlPointCount() to
	/// determine the number of control points in this array.
	///
	/// It is recommended that an application does not store this pointer
	/// for later use since the pointer to the array may change if the
	/// application calls another method of this object.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	const TArray<float> GetControlPointArray() const
	{
		return ControlPoints;
	}

	/// Returns the number of control points on the terrace-forming curve.
	///
	/// @returns The number of control points on the terrace-forming
	/// curve.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	int32 GetControlPointCount() const
	{
		return ControlPointCount;
	}

	UFUNCTION(BlueprintCallable, Category = "Generation")
	virtual int32 GetSourceModuleCount() const
	{
		return 1;
	}

	/// Enables or disables the inversion of the terrace-forming curve
	/// between the control points.
	///
	/// @param invert Specifies whether to invert the curve between the
	/// control points.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void InvertTerraces(bool invert = true)
	{
		ShouldInvertTerraces = invert;
	}

	/// Determines if the terrace-forming curve between the control
	/// points is inverted.
	///
	/// @returns
	/// - @a true if the curve between the control points is inverted.
	/// - @a false if the curve between the control points is not
	///   inverted.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	bool IsTerracesInverted() const
	{
		return ShouldInvertTerraces;
	}

	UFUNCTION(BlueprintCallable, Category = "Generation")
	virtual float GetValue(FVector Coordinates);

	/// Creates a number of equally-spaced control points that range from
	/// -1 to +1.
	///
	/// @param controlPointCount The number of control points to generate.
	///
	/// @pre The number of control points must be greater than or equal to
	/// 2.
	///
	/// @post The previous control points on the terrace-forming curve are
	/// deleted.
	///
	/// @throw noise::ExceptionInvalidParam An invalid parameter was
	/// specified; see the preconditions for more information.
	///
	/// Two or more control points define the terrace-forming curve.  The
	/// start of this curve has a slope of zero; its slope then smoothly
	/// increases.  At the control points, its slope resets to zero.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void MakeControlPoints(int32 controlPointCount);

protected:

	/// Determines the array index in which to insert the control point
	/// into the internal control point array.
	///
	/// @param value The value of the control point.
	///
	/// @returns The array index in which to insert the control point.
	///
	/// @pre No two control points have the same value.
	///
	/// @throw noise::ExceptionInvalidParam An invalid parameter was
	/// specified; see the preconditions for more information.
	///
	/// By inserting the control point at the returned array index, this
	/// class ensures that the control point array is sorted by value.
	/// The code that maps a value onto the curve requires a sorted
	/// control point array.
	int FindInsertionPos(float value);

	/// Inserts the control point at the specified position in the
	/// internal control point array.
	///
	/// @param insertionPos The zero-based array position in which to
	/// insert the control point.
	/// @param value The value of the control point.
	///
	/// To make room for this new control point, this method reallocates
	/// the control point array and shifts all control points occurring
	/// after the insertion position up by one.
	///
	/// Because the curve mapping algorithm in this noise module requires
	/// that all control points in the array be sorted by value, the new
	/// control point should be inserted at the position in which the
	/// order is still preserved.
	void InsertAtPos(int32 insertionPos, float value);

	/// Number of control points stored in this noise module.
	int32 ControlPointCount;

	/// Determines if the terrace-forming curve between all control points
	/// is inverted.
	bool ShouldInvertTerraces;

	/// Array that stores the control points.
	TArray<float> ControlPoints;

};

/// @}

/// @}

/// @}