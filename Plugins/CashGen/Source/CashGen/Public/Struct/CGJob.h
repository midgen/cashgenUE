#pragma once

#include "CGJob.generated.h"

USTRUCT()
struct FCGJob
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACGTile* Tile;

	FCGMeshData* Data;

	TSharedPtr<TArray<FVector>, ESPMode::ThreadSafe>	Vertices;
	TSharedPtr<TArray<int32>, ESPMode::ThreadSafe>		Triangles;
	TSharedPtr<TArray<FVector>, ESPMode::ThreadSafe>	Normals;
	TSharedPtr<TArray<FVector2D>, ESPMode::ThreadSafe>	UV0;
	TSharedPtr<TArray<FColor>, ESPMode::ThreadSafe>		VertexColors;
	TSharedPtr<TArray<FRuntimeMeshTangent>, ESPMode::ThreadSafe> Tangents;
	TSharedPtr<TArray<FVector>, ESPMode::ThreadSafe> HeightMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
	uint8 LOD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
	bool IsInPlaceUpdate;
};