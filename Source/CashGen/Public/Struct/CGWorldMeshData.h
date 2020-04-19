//#pragma once
//
//#include <ProceduralMeshComponent/Public/ProceduralMeshComponent.h>
//
//#include "CGWorldMeshData.generated.h"
//
///** Defines the data required for a single procedural mesh section */
//USTRUCT()
//struct FCGWorldMeshData
//{
//	GENERATED_BODY()
//
//	TArray<FVector> myPositions;
//	TArray<FVector> myNormals;
//	TArray<FProcMeshTangent> myTangents;
//	UPROPERTY()
//	TArray<int32> myIndices;
//
//public:
//	void Allocate(const uint16& aSubDivisions) 
//	{ 
//		myPositions.Empty();
//		myNormals.Empty();
//		myTangents.Empty();
//
//		myIndices.Empty();
//
//		int32 numTris = (3)* (FMath::Pow(4, aSubDivisions));
//
//		myPositions.Reserve(numTris);
//		myNormals.Reserve(numTris);
//		myTangents.Reserve(numTris);
//		
//		myPositions.AddDefaulted(numTris);
//		myNormals.AddDefaulted(numTris);
//		myTangents.AddDefaulted(numTris);
//		myIndices.AddDefaulted(numTris);
//	}
//};