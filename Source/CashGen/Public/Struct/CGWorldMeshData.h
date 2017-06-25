#pragma once
#include "cashgen.h"
#include "RuntimeMeshComponent.h"
#include "CGWorldMeshdata.generated.h"

/** Defines the data required for a single procedural mesh section */
USTRUCT()
struct FCGWorldMeshData
{
	GENERATED_USTRUCT_BODY()
	TArray<FRuntimeMeshVertexSimple> Vertices;
	TArray<int32> Indices;

public:
	void Allocate(const uint16& aSubDivisions) 
	{ 
		Vertices.Empty();
		Indices.Empty();

		int32 numTris = (3)* (FMath::Pow(4, aSubDivisions));

		Vertices.Reserve(numTris);
		for (int i = 0; i < numTris; i++)
		{
			Vertices.Emplace();
		}
		
		Indices.Reserve(numTris);
		for (int i = 0; i < numTris; i++)
		{
			Indices.Add(0);
		}
	}
};