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
#include "TranslatePoint.generated.h"

/// @addtogroup UnrealLibNoise
/// @{

/// @addtogroup modules
/// @{

/// @addtogroup transformermodules
/// @{

/// Default translation factor applied to the @a x coordinate for the
/// noise::module::TranslatePoint noise module.
const float DEFAULT_TRANSLATE_POINT_X = 0.0;

/// Default translation factor applied to the @a y coordinate for the
/// noise::module::TranslatePoint noise module.
const float DEFAULT_TRANSLATE_POINT_Y = 0.0;

/// Default translation factor applied to the @a z coordinate for the
/// noise::module::TranslatePoint noise module.
const float DEFAULT_TRANSLATE_POINT_Z = 0.0;

/// Noise module that moves the coordinates of the input value before
/// returning the output value from a source module.
///
///
/// The GetValue() method moves the ( @a x, @a y, @a z ) coordinates of
/// the input value by a translation amount before returning the output
/// value from the source module.  To set the translation amount, call
/// the SetTranslation() method.  To set the translation amount to
/// apply to the individual @a x, @a y, or @a z coordinates, call the
/// SetXTranslation(), SetYTranslation() or SetZTranslation() methods,
/// respectively.
///
/// This noise module requires one source module.
UCLASS(BlueprintType)
class UNREALLIBNOISE_API UTranslatePoint : public UNoiseModule
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

	/// Returns the translation amount to apply to the @a x coordinate of
	/// the input value.
	///
	/// @returns The translation amount to apply to the @a x coordinate.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	float GetXTranslation() const
	{
		return xTranslation;
	}

	/// Returns the translation amount to apply to the @a y coordinate of
	/// the input value.
	///
	/// @returns The translation amount to apply to the @a y coordinate.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	float GetYTranslation() const
	{
		return yTranslation;
	}

	/// Returns the translation amount to apply to the @a z coordinate of
	/// the input value.
	///
	/// @returns The translation amount to apply to the @a z coordinate.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	float GetZTranslation() const
	{
		return zTranslation;
	}

	/// Sets the translation amount to apply to the input value.
	///
	/// @param translation The translation amount to apply.
	///
	/// The GetValue() method moves the ( @a x, @a y, @a z ) coordinates
	/// of the input value by a translation amount before returning the
	/// output value from the source module
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetTranslation(float translation)
	{
		xTranslation = translation;
		yTranslation = translation;
		zTranslation = translation;
	}

	/// Sets the translation amounts to apply to the ( @a x, @a y, @a z )
	/// coordinates of the input value.
	///
	/// @param InxTranslation The translation amount to apply to the @a x
	/// coordinate.
	/// @param InyTranslation The translation amount to apply to the @a y
	/// coordinate.
	/// @param InzTranslation The translation amount to apply to the @a z
	/// coordinate.
	///
	/// The GetValue() method moves the ( @a x, @a y, @a z ) coordinates
	/// of the input value by a translation amount before returning the
	/// output value from the source module
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetTranslationXYZ(float InxTranslation, float InyTranslation,
		float InzTranslation)
	{
		xTranslation = InxTranslation;
		yTranslation = InyTranslation;
		zTranslation = InzTranslation;
	}

	/// Sets the translation amount to apply to the @a x coordinate of the
	/// input value.
	///
	/// @param InxTranslation The translation amount to apply to the @a x
	/// coordinate.
	///
	/// The GetValue() method moves the ( @a x, @a y, @a z ) coordinates
	/// of the input value by a translation amount before returning the
	/// output value from the source module
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetXTranslation(float InxTranslation)
	{
		xTranslation = InxTranslation;
	}

	/// Sets the translation amount to apply to the @a y coordinate of the
	/// input value.
	///
	/// @param InyTranslation The translation amount to apply to the @a y
	/// coordinate.
	///
	/// The GetValue() method moves the ( @a x, @a y, @a z ) coordinates
	/// of the input value by a translation amount before returning the
	/// output value from the source module
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetYTranslation(float InyTranslation)
	{
		yTranslation = InyTranslation;
	}

	/// Sets the translation amount to apply to the @a z coordinate of the
	/// input value.
	///
	/// @param InzTranslation The translation amount to apply to the @a z
	/// coordinate.
	///
	/// The GetValue() method moves the ( @a x, @a y, @a z ) coordinates
	/// of the input value by a translation amount before returning the
	/// output value from the source module
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetZTranslation(float InzTranslation)
	{
		zTranslation = InzTranslation;
	}

protected:

	/// Translation amount applied to the @a x coordinate of the input
	/// value.
	float xTranslation;

	/// Translation amount applied to the @a y coordinate of the input
	/// value.
	float yTranslation;

	/// Translation amount applied to the @a z coordinate of the input
	/// value.
	float zTranslation;

};

/// @}

/// @}

/// @}
