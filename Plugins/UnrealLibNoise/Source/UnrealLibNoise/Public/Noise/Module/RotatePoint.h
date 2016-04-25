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
#include "RotatePoint.generated.h"

/// @addtogroup UnrealLibNoise
/// @{

/// @addtogroup modules
/// @{

/// @addtogroup transformermodules
/// @{

/// Default @a x rotation angle for the noise::module::RotatePoint noise
/// module.
const float DEFAULT_ROTATE_X = 0.0;

/// Default @a y rotation angle for the noise::module::RotatePoint noise
/// module.
const float DEFAULT_ROTATE_Y = 0.0;

/// Default @a z rotation angle for the noise::module::RotatePoint noise
/// module.
const float DEFAULT_ROTATE_Z = 0.0;

/// Noise module that rotates the input value around the origin before
/// returning the output value from a source module.
///
///
/// The GetValue() method rotates the coordinates of the input value
/// around the origin before returning the output value from the source
/// module.  To set the rotation angles, call the SetAngles() method.  To
/// set the rotation angle around the individual @a x, @a y, or @a z axes,
/// call the SetXAngle(), SetYAngle() or SetZAngle() methods,
/// respectively.
///
/// The coordinate system of the input value is assumed to be
/// "left-handed" (@a x increases to the right, @a y increases upward,
/// and @a z increases inward.)
///
/// This noise module requires one source module.
UCLASS(BlueprintType)
class UNREALLIBNOISE_API URotatePoint : public UNoiseModule
{
	GENERATED_UCLASS_BODY()
	
	
public:

	UFUNCTION(BlueprintCallable, Category = "Generation")
	virtual int32 GetSourceModuleCount() const
	{
		return 1;
	}

	UFUNCTION(BlueprintCallable, Category = "Generation")
	virtual float GetValue(FVector Coordinates);

	/// Returns the rotation angle around the @a x axis to apply to the
	/// input value.
	///
	/// @returns The rotation angle around the @a x axis, in degrees.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	float GetXAngle() const
	{
		return xAngle;
	}

	/// Returns the rotation angle around the @a y axis to apply to the
	/// input value.
	///
	/// @returns The rotation angle around the @a y axis, in degrees.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	float GetYAngle() const
	{
		return yAngle;
	}

	/// Returns the rotation angle around the @a z axis to apply to the
	/// input value.
	///
	/// @returns The rotation angle around the @a z axis, in degrees.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	float GetZAngle() const
	{
		return zAngle;
	}

	/// Sets the rotation angles around all three axes to apply to the
	/// input value.
	///
	/// @param xAngle The rotation angle around the @a x axis, in degrees.
	/// @param yAngle The rotation angle around the @a y axis, in degrees.
	/// @param zAngle The rotation angle around the @a z axis, in degrees.
	///
	/// The GetValue() method rotates the coordinates of the input value
	/// around the origin before returning the output value from the
	/// source module.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetAngles(float xAngle, float yAngle, float zAngle);

	/// Sets the rotation angle around the @a x axis to apply to the input
	/// value.
	///
	/// @param InxAngle The rotation angle around the @a x axis, in degrees.
	///
	/// The GetValue() method rotates the coordinates of the input value
	/// around the origin before returning the output value from the
	/// source module.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetXAngle(float InxAngle)
	{
		SetAngles(InxAngle, yAngle, zAngle);
	}

	/// Sets the rotation angle around the @a y axis to apply to the input
	/// value.
	///
	/// @param InyAngle The rotation angle around the @a y axis, in degrees.
	///
	/// The GetValue() method rotates the coordinates of the input value
	/// around the origin before returning the output value from the
	/// source module.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetYAngle(float InyAngle)
	{
		SetAngles(xAngle, InyAngle, zAngle);
	}

	/// Sets the rotation angle around the @a z axis to apply to the input
	/// value.
	///
	/// @param InzAngle The rotation angle around the @a z axis, in degrees.
	///
	/// The GetValue() method rotates the coordinates of the input value
	/// around the origin before returning the output value from the
	/// source module.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetZAngle(float InzAngle)
	{
		SetAngles(xAngle, yAngle, InzAngle);
	}

protected:

	/// An entry within the 3x3 rotation matrix used for rotating the
	/// input value.
	float x1Matrix;

	/// An entry within the 3x3 rotation matrix used for rotating the
	/// input value.
	float x2Matrix;

	/// An entry within the 3x3 rotation matrix used for rotating the
	/// input value.
	float x3Matrix;

	/// @a x rotation angle applied to the input value, in degrees.
	float xAngle;

	/// An entry within the 3x3 rotation matrix used for rotating the
	/// input value.
	float y1Matrix;

	/// An entry within the 3x3 rotation matrix used for rotating the
	/// input value.
	float y2Matrix;

	/// An entry within the 3x3 rotation matrix used for rotating the
	/// input value.
	float y3Matrix;

	/// @a y rotation angle applied to the input value, in degrees.
	float yAngle;

	/// An entry within the 3x3 rotation matrix used for rotating the
	/// input value.
	float z1Matrix;

	/// An entry within the 3x3 rotation matrix used for rotating the
	/// input value.
	float z2Matrix;

	/// An entry within the 3x3 rotation matrix used for rotating the
	/// input value.
	float z3Matrix;

	/// @a z rotation angle applied to the input value, in degrees.
	float zAngle;

};

/// @}

/// @}

/// @}
	
