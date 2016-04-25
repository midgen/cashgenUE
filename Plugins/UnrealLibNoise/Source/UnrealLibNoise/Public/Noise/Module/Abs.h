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
#include "Abs.generated.h"

/// @addtogroup UnrealLibNoise
/// @{

/// @addtogroup modules
/// @{

/// @defgroup modifiermodules Modifier Modules
/// @addtogroup modifiermodules
/// @{

/// Noise module that outputs the absolute value of the output value from
/// a source module.
///
///
/// This noise module requires one source module.

UCLASS(BlueprintType)
class UNREALLIBNOISE_API UAbs : public UNoiseModule
{
	GENERATED_UCLASS_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Generation")
	virtual int32 GetSourceModuleCount() const { return 1; }
	
	UFUNCTION(BlueprintCallable, Category = "Generation")
	virtual float GetValue(FVector Coordinates) override;
};

/// @}

/// @}

/// @}