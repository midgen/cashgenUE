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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh Data Struct")
		TArray<float> DepositionMap;

};