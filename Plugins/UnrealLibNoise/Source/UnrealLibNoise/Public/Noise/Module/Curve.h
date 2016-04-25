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
#include "Curve.generated.h"

/// @addtogroup UnrealLibNoise
/// @{

/// This structure defines a control point.
///
/// Control points are used for defining splines.
USTRUCT(BlueprintType)
struct  FControlPoint
{
	GENERATED_BODY()

	/// The input value.
	double inputValue;

	/// The output value that is mapped from the input value.
	double outputValue;

};

/// @addtogroup modules
/// @{

/// @addtogroup modifiermodules
/// @{

/// Noise module that maps the output value from a source module onto an
/// arbitrary function curve.
///
///
/// This noise module maps the output value from the source module onto an
/// application-defined curve.  This curve is defined by a number of
/// <i>control points</i>; each control point has an <i>input value</i>
/// that maps to an <i>output value</i>.  Refer to the following
/// illustration:
///
///
/// To add the control points to this curve, call the AddControlPoint()
/// method.
///
/// Since this curve is a cubic spline, an application must add a minimum
/// of four control points to the curve.  If this is not done, the
/// GetValue() method fails.  Each control point can have any input and
/// output value, although no two control points can have the same input
/// value.  There is no limit to the number of control points that can be
/// added to the curve.  
///
/// This noise module requires one source module.
UCLASS()
class UNREALLIBNOISE_API UCurve : public UNoiseModule
{
	GENERATED_UCLASS_BODY()
	
public:

	/// Adds a control point to the curve.
	///
	/// @param inputValue The input value stored in the control point.
	/// @param outputValue The output value stored in the control point.
	///
	/// @pre No two control points have the same input value.
	///
	/// @throw noise::ExceptionInvalidParam An invalid parameter was
	/// specified; see the preconditions for more information.
	///
	/// It does not matter which order these points are added.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void AddControlPoint(float inputValue, float outputValue);

	/// Deletes all the control points on the curve.
	///
	/// @post All points on the curve are deleted.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void ClearAllControlPoints();

	/// Returns a pointer to the array of control points on the curve.
	///
	/// @returns A pointer to the array of control points.
	///
	/// Before calling this method, call GetControlPointCount() to
	/// determine the number of control points in this array.
	///
	/// It is recommended that an application does not store this pointer
	/// for later use since the pointer to the array may change if the
	/// application calls another method of this object.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	const TArray<FControlPoint> GetControlPointArray() const
	{
		return ControlPoints;
	}

	/// Returns the number of control points on the curve.
	///
	/// @returns The number of control points on the curve.
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

	UFUNCTION(BlueprintCallable, Category = "Generation")
	virtual float GetValue(FVector Coordinates);

protected:

	/// Determines the array index in which to insert the control point
	/// into the internal control point array.
	///
	/// @param inputValue The input value of the control point.
	///
	/// @returns The array index in which to insert the control point.
	///
	/// @pre No two control points have the same input value.
	///
	/// @throw noise::ExceptionInvalidParam An invalid parameter was
	/// specified; see the preconditions for more information.
	///
	/// By inserting the control point at the returned array index, this
	/// class ensures that the control point array is sorted by input
	/// value.  The code that maps a value onto the curve requires a
	/// sorted control point array.
	int32 FindInsertionPos(float inputValue);

	/// Inserts the control point at the specified position in the
	/// internal control point array.
	///
	/// @param insertionPos The zero-based array position in which to
	/// insert the control point.
	/// @param inputValue The input value stored in the control point.
	/// @param outputValue The output value stored in the control point.
	///
	/// To make room for this new control point, this method reallocates
	/// the control point array and shifts all control points occurring
	/// after the insertion position up by one.
	///
	/// Because the curve mapping algorithm used by this noise module
	/// requires that all control points in the array must be sorted by
	/// input value, the new control point should be inserted at the
	/// position in which the order is still preserved.
	void InsertAtPos(int32 insertionPos, float inputValue, float outputValue);

	/// Number of control points on the curve.
	int32 ControlPointCount;

	/// Array that stores the control points.
	TArray<FControlPoint> ControlPoints;
	
	
};

/// @}

/// @}

/// @}