#pragma once

#include "CGJob.generated.h"

USTRUCT()
struct FCGJob
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACGTile* Tile;

	FCGMeshData* Data;

	TArray<FVector>*	Vertices;
	TArray<int32>*		Triangles;
	TArray<FVector>*	Normals;
	TArray<FVector2D>*	UV0;
	TArray<FColor>*	VertexColors;
	TArray<FRuntimeMeshTangent>* Tangents;
	TArray<FVector>* HeightMap;
	TArray<float>* DespositionMap;

	int32 HeightmapGenerationDuration;
	int32 ErosionGenerationDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
	uint8 LOD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
	bool IsInPlaceUpdate;
};