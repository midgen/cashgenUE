#pragma once
#include "cashgen.h"
#include "RuntimeMeshComponent.h"
#include "BiomeWeights.h"
#include "Meshdata.generated.h"

/** Defines the data required for a single procedural mesh section */
USTRUCT()
struct FMeshData
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Data Struct")
	TArray<FVector> MyVertices;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Data Struct")
	TArray<int32> MyTriangles;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Data Struct")
	TArray<FVector> MyNormals;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Data Struct")
	TArray<FVector2D> MyUV0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Data Struct")
	TArray<FColor> MyVertexColors;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Data Struct")
	TArray<FRuntimeMeshTangent> MyTangents;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Data Struct")
	TArray<FVector> MyHeightMap;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = "Mesh Data Struct")
	TArray<FBiomeWeights> BiomeWeightMap;
};




