#pragma once
#include "RuntimeMeshComponent.h"
#include "CGWorldFaceJob.generated.h"

class ACGWorldFace;
struct FCGWorldMeshData;

USTRUCT()
struct FCGWorldFaceJob
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACGWorldFace* pFace;

	FCGWorldMeshData* pMeshData;

	FRuntimeMeshVertexSimple v1;
	FRuntimeMeshVertexSimple v2;
	FRuntimeMeshVertexSimple v3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
	uint8 SubDivisions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
	float Radius;

};