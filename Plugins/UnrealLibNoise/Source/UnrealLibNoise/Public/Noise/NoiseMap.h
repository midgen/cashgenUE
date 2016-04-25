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
#include "Module/NoiseModule.h"
#include "NoiseMap.generated.h"

///This is the structure that sets a color to a specific value that will represent a position on a heightmap
USTRUCT(BlueprintType)
struct FGradientPoint
{
	GENERATED_BODY()

	///The position that correlates to a noise value
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	float Position;

	///The Color that will represent this position
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	FColor Color;

	FGradientPoint() {}
	FGradientPoint(float InPosition, FColor InColor) { Position = InPosition; Color = InColor; }
};

///Array of Gradient points that will determine what colors should be used to visually represent a noise value
USTRUCT(BlueprintType)
struct FGradient
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	TArray<FGradientPoint> GradientPoints;

	FColor GetColor(float Value)
	{
		if (GradientPoints.Num() >= 2)
		{
			int indexPos;
			for (indexPos = 0; indexPos < GradientPoints.Num(); indexPos++)
			{
				if (Value < GradientPoints[indexPos].Position) break;
			}

			int index0 = FMath::Clamp(indexPos - 1, 0, GradientPoints.Num() - 1);
			int index1 = FMath::Clamp(indexPos, 0, GradientPoints.Num() - 1);

			if (index0 == index1) return GradientPoints[index1].Color;

			float input0 = GradientPoints[index0].Position;
			float input1 = GradientPoints[index1].Position;
			float alpha = (Value - input0) / (input1 - input0);

			FLinearColor Color0 = FLinearColor(GradientPoints[index0].Color);
			FLinearColor Color1 = FLinearColor(GradientPoints[index1].Color);

			FLinearColor ReturnColor = FLinearColor::LerpUsingHSV(Color0, Color1, alpha);
			return ReturnColor.ToFColor(true);
		}

		return FColor::Black;
	}

	void AddGradientPoint(float Position, FColor Color)
	{
		GradientPoints.Add(FGradientPoint(Position, Color));
	}

	static FGradient BlackAndWhiteGradient()
	{
		FGradient temp;

		temp.GradientPoints.Empty();

		temp.AddGradientPoint(-1.0, FColor(0, 0, 0, 255));
		temp.AddGradientPoint(1, FColor(255, 255, 255, 255));

		return temp;
	}

	static FGradient TerrainGradient()
	{
		FGradient temp;

		temp.GradientPoints.Empty();

		temp.AddGradientPoint(-1.00, FColor(0, 0, 128, 255));
		temp.AddGradientPoint(-0.20, FColor(32, 64, 128, 255));
		temp.AddGradientPoint(-0.04, FColor(64, 96, 192, 255));
		temp.AddGradientPoint(-0.02, FColor(192, 192, 128, 255));
		temp.AddGradientPoint(0.00, FColor(0, 192, 0, 255));
		temp.AddGradientPoint(0.25, FColor(192, 192, 0, 255));
		temp.AddGradientPoint(0.50, FColor(160, 96, 64, 255));
		temp.AddGradientPoint(0.75, FColor(128, 255, 255, 255));
		temp.AddGradientPoint(1.00, FColor(255, 255, 255, 255));

		return temp;
	}
};


/**
 * 
 */
UCLASS(BlueprintType)
class UNREALLIBNOISE_API UNoiseMap : public UObject
{
	GENERATED_BODY()

public:
	UNoiseMap() {}

	UFUNCTION(BlueprintCallable, Category = "Default")
	void Clear() { NoiseArray.Empty(); }

	UFUNCTION(BlueprintCallable, Category = "Variables")
	float GetValue(int32 X, int32 Y) { return NoiseArray[Width * X + Y]; }
	UFUNCTION(BlueprintCallable, Category = "Variables")
	void SetValue(int32 X, int32 Y, float Value) { NoiseArray[Width * X + Y] = Value; }

	UFUNCTION(BlueprintCallable, Category = "Variables")
	int32 GetHeight() { return Height; }
	UFUNCTION(BlueprintCallable, Category = "Variables")
	int32 GetWidth() { return Width; }
	UFUNCTION(BlueprintCallable, Category = "Variables")
	void SetSize(int32 InHeight, int32 InWidth) { Height = InHeight; Width = InWidth; }
	UFUNCTION(BlueprintCallable, Category = "Variables")
	UNoiseModule* GetNoiseModule() { return NoiseModule; }
	UFUNCTION(BlueprintCallable, Category = "Variables")
	void SetNoiseModule(UNoiseModule* InModule) { NoiseModule = InModule; }
	TArray<FColor> GetColorArray() { return ColorArray; }

	UFUNCTION(BlueprintCallable, Category = "Generation")
	void GeneratePlane(float LowerXBounds, float LowerZBounds, float UpperXBounds, float UpperZBounds);
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void GenerateSphere(float NorthLatBound, float SouthLatBound, float WestLongBound, float EastLongBound);
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void GenerateCylinder(float LowerAngleBounds, float LowerHeightBounds, float UpperAngleBounds, float UpperHeight);
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void GenerateColorArray(FGradient Gradient);
	UFUNCTION(BlueprintCallable, Category = "Generation")
	UTexture2D* GenerateTexture();

private:
	TArray<float> NoiseArray;
	TArray<FColor> ColorArray;
	int32 Height;
	int32 Width;

	float BorderValue;

	UNoiseModule* NoiseModule;
	bool Seemless = true;

	FVector LatLongtoVector(float Latidtude, float Longitude);
	
};
