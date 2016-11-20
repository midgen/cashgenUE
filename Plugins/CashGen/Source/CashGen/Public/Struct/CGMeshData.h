#pragma once
#include "cashgen.h"
#include "RuntimeMeshComponent.h"
#include "CGTerrainConfig.h"
#include "CGMeshdata.generated.h"

/** Defines the data required for a single procedural mesh section */
USTRUCT()
struct FCGMeshData
{
	GENERATED_USTRUCT_BODY()
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Data Struct")
		TArray<FVector> Vertices;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Data Struct")
		TArray<int32> Triangles;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Data Struct")
		TArray<FVector> Normals;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Data Struct")
		TArray<FVector2D> UV0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Data Struct")
		TArray<FColor> VertexColors;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Data Struct")
		TArray<FRuntimeMeshTangent> Tangents;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Data Struct")
		TArray<FVector> HeightMap;

	bool AllocateDataStructuresForLOD(const FCGTerrainConfig* aConfig, const uint8 aLOD)
	{
		int32 numXVerts = aLOD == 0 ? aConfig->XUnits + 1 : (aConfig->XUnits / (FMath::Pow(2, aLOD))) + 1;
		int32 numYVerts = aLOD == 0 ? aConfig->YUnits + 1 : (aConfig->YUnits / (FMath::Pow(2, aLOD))) + 1;

		Vertices.Reserve(numXVerts * numYVerts);
		Normals.Reserve(numXVerts * numYVerts);
		UV0.Reserve(numXVerts * numYVerts);
		VertexColors.Reserve(numXVerts * numYVerts);
		Tangents.Reserve(numXVerts * numYVerts);

		// Generate the per vertex data sets
		for (int32 i = 0; i < (numXVerts * numYVerts); ++i)
		{
			Vertices.Emplace(0.0f);
			Normals.Emplace(0.0f, 0.0f, 1.0f);
			UV0.Emplace(0.0f, 0.0f);
			VertexColors.Emplace(FColor::Black);
			Tangents.Emplace(0.0f, 0.0f, 0.0f);
		}

		// Heightmap needs to be larger than the mesh
		// Using vectors here is a bit wasteful, but it does make normal/tangent or any other
		// Geometric calculations based on the heightmap a bit easier. Easy enough to change to floats
		HeightMap.Reserve((numXVerts + 2) * (numYVerts + 2));
		for (int32 i = 0; i < (numXVerts + 2) * (numYVerts + 2); ++i)
		{
			HeightMap.Emplace(0.0f);
		}

		// Triangle indexes
		Triangles.Reserve((numXVerts - 1) * (numYVerts - 1) * 6);
		for (int32 i = 0; i < (numXVerts - 1) * (numYVerts - 1) * 6; ++i)
		{
			Triangles.Add(i);
		}
		 
		// Now calculate triangles and UVs
		int32 triCounter = 0;
		int32 thisX, thisY;
		int32 rowLength;

		rowLength = aLOD == 0 ? aConfig->XUnits + 1 : (aConfig->XUnits / (FMath::Pow(2, aLOD)) + 1);
		float maxUV = aLOD == 0 ? 1.0f : 1.0f / aLOD;

		int32 exX = aLOD == 0 ? aConfig->XUnits : (aConfig->XUnits / (FMath::Pow(2, aLOD)));
		int32 exY = aLOD == 0 ? aConfig->YUnits : (aConfig->YUnits / (FMath::Pow(2, aLOD)));

		for (int32 y = 0; y < exY; ++y)
		{
			for (int32 x = 0; x < exX; ++x)
			{

				thisX = x;
				thisY = y;
				//TR
				Triangles[triCounter] = thisX + ((thisY + 1) * (rowLength));
				triCounter++;
				//BL
				Triangles[triCounter] = (thisX + 1) + (thisY * (rowLength));
				triCounter++;
				//BR
				Triangles[triCounter] = thisX + (thisY * (rowLength));
				triCounter++;

				//BL
				Triangles[triCounter] = (thisX + 1) + (thisY * (rowLength));
				triCounter++;
				//TR
				Triangles[triCounter] = thisX + ((thisY + 1) * (rowLength));
				triCounter++;
				// TL
				Triangles[triCounter] = (thisX + 1) + ((thisY + 1) * (rowLength));
				triCounter++;

				//TR
				UV0[thisX + ((thisY + 1) * (rowLength))] = FVector2D(thisX * maxUV, (thisY + 1.0f) * maxUV);
				//BR
				UV0[thisX + (thisY * (rowLength))] = FVector2D(thisX * maxUV, thisY * maxUV);
				//BL
				UV0[(thisX + 1) + (thisY * (rowLength))] = FVector2D((thisX + 1.0f) * maxUV, thisY * maxUV);
				//TL
				UV0[(thisX + 1) + ((thisY + 1) * (rowLength))] = FVector2D((thisX + 1.0f)* maxUV, (thisY + 1.0f) * maxUV);

			}
		}

		return true;

	}
};