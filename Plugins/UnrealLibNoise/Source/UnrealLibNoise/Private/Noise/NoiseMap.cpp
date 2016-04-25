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

#include "UnrealLibNoise.h"
#include "NoiseMap.h"
#include "Runtime/Engine/Public/ImageUtils.h"

void UNoiseMap::GeneratePlane(float LowerXBounds, float LowerZBounds, float UpperXBounds, float UpperZBounds)
{
	if (UpperXBounds <= LowerXBounds ||UpperZBounds <= LowerZBounds || Height <= 0 || Width <= 0)
	{
		Msg("Error: Plane out of bounds");
	}
	else
	{
		NoiseArray.Empty();

		float xExtent = UpperXBounds - LowerXBounds;
		float zExtent = UpperZBounds - LowerZBounds;
		float xDelta = (xExtent / Width);
		float zDelta = (zExtent / Height);
		float CurrentX = LowerXBounds;
		float CurrentZ = LowerZBounds;

		for (int z = 0; z < Height; z++)
		{
			CurrentX = LowerXBounds;

			for (int x = 0; x < Width; x++)
			{
				float finalValue;

				if (!Seemless)
				{
					finalValue =NoiseModule->GetValue(FVector(CurrentX, 0, CurrentZ));
				}
				else
				{
					float swValue, seValue, nwValue, neValue;
					swValue =NoiseModule->GetValue(FVector(CurrentX, 0, CurrentZ));
					seValue =NoiseModule->GetValue(FVector(CurrentX + xExtent, 0, CurrentZ));
					nwValue =NoiseModule->GetValue(FVector(CurrentX, 0, CurrentZ + xExtent));
					neValue =NoiseModule->GetValue(FVector(CurrentX + xExtent, 0, CurrentX + xExtent));

					float xBlend = 1.0 - ((CurrentX - LowerXBounds) / xExtent);
					float zBlend = 1.0 - ((CurrentZ - LowerZBounds) / zExtent);
					float z0 = FMath::Lerp(swValue, seValue, xBlend);
					float z1 = FMath::Lerp(nwValue, neValue, xBlend);

					finalValue = FMath::Lerp(z0, z1, zBlend);
				}

				NoiseArray.Add(finalValue);
				CurrentX += xDelta;
			}

			CurrentZ += zDelta;
		}
	}
}

void UNoiseMap::GenerateCylinder(float LowerAngleBounds, float LowerHeightBounds, float UpperAngleBounds, float UpperHeightBounds)
{
	if (UpperAngleBounds <= LowerAngleBounds || UpperHeightBounds <= LowerHeightBounds || Height <= 0 || Width <= 0)
	{
		Msg("Error: Cylinder out of bounds");
	}
	else
	{
		NoiseArray.Empty();
		NoiseArray.SetNum(Width * Height);

		float AngleExtent = UpperAngleBounds - LowerAngleBounds;
		float HeightExtent = UpperHeightBounds - LowerHeightBounds;
		float xDelta = AngleExtent / Width;
		float yDelta = AngleExtent / Height;
		float CurrentAngle = LowerAngleBounds;
		float CurrentHeight = LowerHeightBounds;

		for (int y = 0; y < Height; y++)
		{
			CurrentAngle = LowerAngleBounds;

			for (int x = 0; x < Width; x++)
			{
				float X, Y, Z;

				X = FMath::Cos(FMath::DegreesToRadians(CurrentAngle));
				Y = CurrentHeight;
				Z = FMath::Sin(FMath::DegreesToRadians(CurrentAngle));

				float CurrentValue =NoiseModule->GetValue(FVector(X, Y, Z));

				NoiseArray.Add(CurrentValue);

				CurrentAngle += xDelta;
			}

			CurrentHeight += yDelta;
		}
	}
}

void UNoiseMap::GenerateSphere(float NorthLatBound, float SouthLatBound, float WestLongBound, float EastLongBound)
{
	if (EastLongBound <= WestLongBound || NorthLatBound <= SouthLatBound || Height <= 0 || Width <= 0)
	{
		Msg("Error: Sphere out of bounds");
	}
	else
	{
		NoiseArray.Empty();
		NoiseArray.SetNum(Width * Height);

		float LongitudeExetent = EastLongBound - WestLongBound;
		float LatitudeExtent = NorthLatBound - SouthLatBound;
		float xDelta = LongitudeExetent / Width;
		float yDelta = LatitudeExtent / Height;
		float CurrentLongitude = WestLongBound;
		float CurrentLatitude = SouthLatBound;

		for (int y = 0; y < Height; y++)
		{
			CurrentLongitude = WestLongBound;

			for (int x = 0; x < Width; x++)
			{
				FVector ValueVector = LatLongtoVector(CurrentLatitude, CurrentLongitude);
				float CurrentValue =NoiseModule->GetValue(FVector(ValueVector.X, ValueVector.Y, ValueVector.Z));

				CurrentLongitude += xDelta;
			}

			CurrentLatitude += yDelta;
		}
	}
}

FVector UNoiseMap::LatLongtoVector(float Latidtude, float Longitude)
{
	float r = FMath::Cos(FMath::DegreesToRadians(Latidtude));
	FVector tempVector;
	tempVector.X = r * FMath::Cos(FMath::DegreesToRadians(Longitude));
	tempVector.Y = FMath::Sin(FMath::DegreesToRadians(Latidtude));
	tempVector.Z = FMath::Sin(FMath::DegreesToRadians(Longitude));

	return tempVector;
}

void UNoiseMap::GenerateColorArray(FGradient Gradient)
{
	if (Height <= 0 || Width <= 0 || NoiseArray.Num() <= 0)
	{
		Msg("Error: While generating color array, either noise array was empty or height/width is less than or equal to 0.");
	}
	else
	{
		ColorArray.Empty();
 
		for (int32 y = 0; y < Height; y++)
		{
			for (int32 x = 0; x < Width; x++)
			{
				FColor tempColor = Gradient.GetColor(GetValue(x, y));
				ColorArray.Add(tempColor);
			}
		}
	}
}

UTexture2D* UNoiseMap::GenerateTexture()
{
	UTexture2D* temp = nullptr;

	ColorArray.Empty();
	GenerateColorArray(FGradient::BlackAndWhiteGradient());
	if (ColorArray.Num() > 0)
	{
		temp = UTexture2D::CreateTransient(Width, Height, PF_R8G8B8A8);


		uint8* MipData = static_cast<uint8*>(temp->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

		//Create base mip
		uint8* DestPtr = NULL;
		const FColor* SrcPtr = NULL;

		for (int y = 0; y < Height; y++)
		{
			DestPtr = &MipData[(Height - 1 - y) * Width * sizeof(FColor)];
			SrcPtr = const_cast<FColor*>(&ColorArray[(Height - 1 - y) * Width]);

			for (int x = 0; x < Width; x++)
			{
				*DestPtr++ = SrcPtr->B;
				*DestPtr++ = SrcPtr->G;
				*DestPtr++ = SrcPtr->R;
				*DestPtr++ = SrcPtr->A;

				SrcPtr++;
			}
		}

		temp->PlatformData->Mips[0].BulkData.Unlock();
		temp->UpdateResource();

	}
	return temp;
}


