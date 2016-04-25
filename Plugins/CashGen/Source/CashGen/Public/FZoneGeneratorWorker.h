#pragma once
#include "cashgen.h"
#include "ZoneManager.h"
#include "ZoneJob.h"
#include "Point.h"
#include "ZoneConfig.h"

class FZoneGeneratorWorker : public FRunnable
{
	AZoneManager* pCallingZoneManager;

	TArray<GridRow>*	pZoneData;
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

	void ProcessTerrainMap();
	void ProcessGeometry();
	void ProcessChildMeshSpawns();
	void UpdateOneBlockGeometry(ZoneBlock* aBlock, int32& aVertCounter, int32& triCounter);
	FVector CalcSurfaceNormalForTriangle(FVector v1, FVector v2, FVector v3);

public:
	FZoneGeneratorWorker(AZoneManager*		apZoneManager,
		FZoneConfig*			aZoneConfig,
		Point*				aOffSet,
		TMap<uint8, eLODStatus>* pZoneJobData,
		uint8 MyLOD,
		TArray<GridRow>*	aZoneData,
		TArray<FVector>*	aVertices,
		TArray<int32>*		aTriangles,
		TArray<FVector>*	aNormals,
		TArray<FVector2D>*	aUV0,
		TArray<FColor>*		aVertexColors,
		TArray<FProcMeshTangent>* aTangents);

	virtual ~FZoneGeneratorWorker();

	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	virtual void Exit();


};