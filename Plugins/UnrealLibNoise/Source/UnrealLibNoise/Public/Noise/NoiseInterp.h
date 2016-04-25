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

#include "Object.h"
#include "NoiseInterp.generated.h"

/// @addtogroup UnrealLibNoise
/// @{


///////
 /// 
 ////
UCLASS(BlueprintType)
class UNREALLIBNOISE_API UNoiseInterp : public UObject
{
	GENERATED_BODY()

public:

	/// Performs cubic interpolation between two values bound between two other
	/// values.
	///
	/// @param n0 The value before the first value.
	/// @param n1 The first value.
	/// @param n2 The second value.
	/// @param n3 The value after the second value.
	/// @param a The alpha value.
	///
	/// @returns The interpolated value.
	///
	/// The alpha value should range from 0.0 to 1.0.  If the alpha value is
	/// 0.0, this function returns @a n1.  If the alpha value is 1.0, this
	/// function returns @a n2.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	static float CubicInterp(float n0, float n1, float n2, float n3, float a);
		
	/// Performs linear interpolation between two values.
	///
	/// @param n0 The first value.
	/// @param n1 The second value.
	/// @param a The alpha value.
	///
	/// @returns The interpolated value.
	///
	/// The alpha value should range from 0.0 to 1.0.  If the alpha value is
	/// 0.0, this function returns @a n0.  If the alpha value is 1.0, this
	/// function returns @a n1.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	static float LinearInterp(float n0, float n1, float a);
	
	/// Maps a value onto a cubic S-curve.
	///
	/// @param a The value to map onto a cubic S-curve.
	///
	/// @returns The mapped value.
	///
	/// @a a should range from 0.0 to 1.0.
	///
	/// The derivative of a cubic S-curve is zero at @a a = 0.0 and @a a =
	/// 1.0
	UFUNCTION(BlueprintCallable, Category = "Generation")
	static float SCurve3(float a);

	/// Maps a value onto a quintic S-curve.
	///
	/// @param a The value to map onto a quintic S-curve.
	///
	///+ @returns The mapped value.
	///
	///+ @a a should range from 0.0 to 1.0.
	///
	/// The first derivative of a quintic S-curve is zero at @a a = 0.0 and
	/// @a a = 1.0
	///
	/// The second derivative of a quintic S-curve is zero at @a a = 0.0 and
	/// @a a = 1.0
	UFUNCTION(BlueprintCallable, Category = "Generation")
	static float SCurve5(float a);
};

/// @}
