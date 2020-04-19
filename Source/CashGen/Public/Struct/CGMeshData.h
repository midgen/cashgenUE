#pragma once

#include <ProceduralMeshComponent/Public/ProceduralMeshComponent.h>

#include "CGMeshData.generated.h"

/** Defines the data required for a single procedural mesh section */
USTRUCT(BlueprintType)
struct FCGMeshData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Data Struct")
	TArray<FVector> MyPositions;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Data Struct")
	TArray<FVector> MyNormals;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Data Struct")
	TArray<FProcMeshTangent> MyTangents;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Data Struct")
	TArray<FColor> MyColours;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Data Struct")
	TArray<FVector2D> MyUV0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Data Struct")
	TArray<int32> MyTriangles;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Data Struct")
	TArray<float> HeightMap;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Data Struct")
	TArray<FColor> myTextureData;
};