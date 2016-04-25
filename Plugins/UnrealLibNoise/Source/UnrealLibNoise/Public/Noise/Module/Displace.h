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
#include "Displace.generated.h"

/// @addtogroup UnrealLibNoise
/// @{

/// @addtogroup modules
/// @{

/// @defgroup transformermodules Transformer Modules
/// @addtogroup transformermodules
/// @{

/// Noise module that uses three source modules to displace each
/// coordinate of the input value before returning the output value from
/// a source module.
///
///
/// Unlike most other noise modules, the index value assigned to a source
/// module determines its role in the displacement operation:
/// - Source module 0 (left in the diagram) outputs a value.
/// - Source module 1 (lower left in the diagram) specifies the offset to
///   apply to the @a x coordinate of the input value.
/// - Source module 2 (lower center in the diagram) specifies the
///   offset to apply to the @a y coordinate of the input value.
/// - Source module 3 (lower right in the diagram) specifies the offset
///   to apply to the @a z coordinate of the input value.
///
/// The GetValue() method modifies the ( @a x, @a y, @a z ) coordinates of
/// the input value using the output values from the three displacement
/// modules before retrieving the output value from the source module.
///
/// The noise::module::Turbulence noise module is a special case of the
/// displacement module; internally, there are three Perlin-noise modules
/// that perform the displacement operation.
///
/// This noise module requires four source modules.
UCLASS(BlueprintType)
class UNREALLIBNOISE_API UDisplace : public UNoiseModule
{
	GENERATED_UCLASS_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Generation")
	virtual int32 GetSourceModuleCount() const
	{
		return 4;
	}

	UFUNCTION(BlueprintCallable, Category = "Generation")
	virtual float GetValue(FVector Coordinates);

	/// Returns the @a x displacement module.
	///
	/// @returns A reference to the @a x displacement module.
	///
	/// @pre This displacement module has been added to this noise module
	/// via a call to SetSourceModule() or SetXDisplaceModule().
	///
	/// @throw noise::ExceptionNoModule See the preconditions for more
	/// information.
	///
	/// The GetValue() method displaces the input value by adding the output
	/// value from this displacement module to the @a x coordinate of the
	/// input value before returning the output value from the source
	/// module.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	 UNoiseModule* GetXDisplaceModule()
	{
		if (GetSourceModule(0) == NULL || GetSourceModule(1) == NULL) {
			throw ExceptionNoModule();
		}

		return GetSourceModule(1);
	}

	/// Returns the @a y displacement module.
	///
	/// @returns A reference to the @a y displacement module.
	///
	/// @pre This displacement module has been added to this noise module
	/// via a call to SetSourceModule() or SetYDisplaceModule().
	///
	/// @throw noise::ExceptionNoModule See the preconditions for more
	/// information.
	///
	/// The GetValue() method displaces the input value by adding the output
	/// value from this displacement module to the @a y coordinate of the
	/// input value before returning the output value from the source
	/// module.
	UNoiseModule* GetYDisplaceModule() 
	{
		if (GetSourceModule(0) == NULL || GetSourceModule(2) == NULL) {
			throw ExceptionNoModule();
		}

		return GetSourceModule(2);
	}

	/// Returns the @a z displacement module.
	///
	/// @returns A reference to the @a z displacement module.
	///
	/// @pre This displacement module has been added to this noise module
	/// via a call to SetSourceModule() or SetZDisplaceModule().
	///
	/// @throw noise::ExceptionNoModule See the preconditions for more
	/// information.
	///
	/// The GetValue() method displaces the input value by adding the output
	/// value from this displacement module to the @a z coordinate of the
	/// input value before returning the output value from the source
	/// module.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	UNoiseModule* GetZDisplaceModule() 
	{
		if (GetSourceModule(0) == NULL || GetSourceModule(3) == NULL) {
			throw ExceptionNoModule();
		}
		return GetSourceModule(3);
	}

	/// Sets the @a x, @a y, and @a z displacement modules.
	///
	/// @param xDisplaceModule Displacement module that displaces the @a x
	/// coordinate of the input value.
	/// @param yDisplaceModule Displacement module that displaces the @a y
	/// coordinate of the input value.
	/// @param zDisplaceModule Displacement module that displaces the @a z
	/// coordinate of the input value.
	///
	/// The GetValue() method displaces the input value by adding the output
	/// value from each of the displacement modules to the corresponding
	/// coordinates of the input value before returning the output value
	/// from the source module.
	///
	/// This method assigns an index value of 1 to the @a x displacement
	/// module, an index value of 2 to the @a y displacement module, and an
	/// index value of 3 to the @a z displacement module.
	///
	/// These displacement modules must exist throughout the lifetime of
	/// this noise module unless another displacement module replaces it.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetDisplaceModules(UNoiseModule* xDisplaceModule,
		 UNoiseModule* yDisplaceModule, UNoiseModule* zDisplaceModule)
	{
		SetXDisplaceModule(xDisplaceModule);
		SetYDisplaceModule(yDisplaceModule);
		SetZDisplaceModule(zDisplaceModule);
	}

	/// Sets the @a x displacement module.
	///
	/// @param xDisplaceModule Displacement module that displaces the @a x
	/// coordinate.
	///
	/// The GetValue() method displaces the input value by adding the output
	/// value from this displacement module to the @a x coordinate of the
	/// input value before returning the output value from the source
	/// module.
	///
	/// This method assigns an index value of 1 to the @a x displacement
	/// module.  Passing this displacement module to this method produces
	/// the same results as passing this displacement module to the
	/// SetSourceModule() method while assigning it an index value of 1.
	///
	/// This displacement module must exist throughout the lifetime of this
	/// noise module unless another displacement module replaces it.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetXDisplaceModule(UNoiseModule* xDisplaceModule)
	{
		if (GetSourceModule(0) == NULL) throw ExceptionNoModule();
		SetSourceModule(1, xDisplaceModule);
	}

	/// Sets the @a y displacement module.
	///
	/// @param yDisplaceModule Displacement module that displaces the @a y
	/// coordinate.
	///
	/// The GetValue() method displaces the input value by adding the output
	/// value from this displacement module to the @a y coordinate of the
	/// input value before returning the output value from the source
	/// module.
	///
	/// This method assigns an index value of 2 to the @a y displacement
	/// module.  Passing this displacement module to this method produces
	/// the same results as passing this displacement module to the
	/// SetSourceModule() method while assigning it an index value of 2.
	///
	/// This displacement module must exist throughout the lifetime of this
	/// noise module unless another displacement module replaces it.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetYDisplaceModule(UNoiseModule* yDisplaceModule)
	{
		if (GetSourceModule(0) == NULL) throw ExceptionNoModule();
		SetSourceModule(2, yDisplaceModule);
	}

	/// Sets the @a z displacement module.
	///
	/// @param zDisplaceModule Displacement module that displaces the @a z
	/// coordinate.
	///
	/// The GetValue() method displaces the input value by adding the output
	/// value from this displacement module to the @a z coordinate of the
	/// input value before returning the output value from the source
	/// module.
	///
	/// This method assigns an index value of 3 to the @a z displacement
	/// module.  Passing this displacement module to this method produces
	/// the same results as passing this displacement module to the
	/// SetSourceModule() method while assigning it an index value of 3.
	///
	/// This displacement module must exist throughout the lifetime of this
	/// noise module unless another displacement module replaces it.
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void SetZDisplaceModule(UNoiseModule* zDisplaceModule)
	{
		if (GetSourceModule(0) == NULL) throw ExceptionNoModule();
		SetSourceModule(3, zDisplaceModule);
	}

};

/// @}

/// @}

/// @}
