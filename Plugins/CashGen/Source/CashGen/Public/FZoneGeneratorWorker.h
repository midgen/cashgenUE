#pragma once
#include "cashgen.h"
#include "ZoneManager.h"
#include "ZoneJob.h"
#include "Point.h"
#include "ZoneConfig.h"

class FZoneGeneratorWorker : public FRunnable
{
	AZoneManager* pCallingZoneManager;
	FZoneConfig*			pZoneConfig;
	Point*				pOffset;
	TMap<uint8, eLODStatus>* pZoneJobData;
	uint8 MyLOD;

	float MyMaxHeight;

	TArray<FVector>*	pVertices;
	TArray<int32>*		pTriangles;
	TArray<FVector>*	pNormals;
	TArray<FVector2D>*	pUV0;
	TArray<FColor>*		pVertexColors;
	TArray<FProcMeshTangent>* pTangents;
	TArray<FVector>* pHeightMap;

	void ProcessTerrainMap();
	void ProcessGeometry();
	FVector GetNormalFromHeightMapForVertex(const int32 vertexX, const int32 vertexY);
	FProcMeshTangent GetTangentFromNormal(const FVector aNormal);
	void UpdateOneBlockGeometry(const int aX, const int aY, int32& aVertCounter, int32& triCounter);
	
public:
	FZoneGeneratorWorker(AZoneManager*		apZoneManager,
		FZoneConfig*			aZoneConfig,
		Point*				aOffSet,
		TMap<uint8, eLODStatus>* pZoneJobData,
		uint8 MyLOD,
		TArray<FVector>*	aVertices,
		TArray<int32>*		aTriangles,
		TArray<FVector>*	aNormals,
		TArray<FVector2D>*	aUV0,
		TArray<FColor>*		aVertexColors,
		TArray<FProcMeshTangent>* aTangents,
		TArray<FVector>* aHeightMap);

	virtual ~FZoneGeneratorWorker();

	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	virtual void Exit();


};