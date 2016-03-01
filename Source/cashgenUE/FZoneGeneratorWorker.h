#pragma once
#include "cashgenUE.h"
#include "ZoneManager.h"
#include "WorldGenerator.h"
#include "Point.h"

class FZoneGeneratorWorker : public FRunnable
{
	AZoneManager* pCallingZoneManager;

	TArray<GridRow>*	pZoneData;
	ZoneConfig*			pZoneConfig;
	Point*				pOffset;

	TArray<FVector>*	pVertices;
	TArray<int32>*		pTriangles;
	TArray<FVector>*	pNormals;
	TArray<FVector2D>*	pUV0;
	TArray<FColor>*		pVertexColors;
	
	void ProcessTerrainMap();
	void ProcessGeometry();
	void UpdateOneBlockGeometry(ZoneBlock* aBlock, int32 aTriangleCounter);
	FVector CalcSurfaceNormalForTriangle(FVector v1, FVector v2, FVector v3);

public:
	FZoneGeneratorWorker(AZoneManager*		apZoneManager,
		ZoneConfig*			aZoneConfig,
		Point*				aOffSet,
		TArray<GridRow>*	aZoneData,
		TArray<FVector>*	aVertices,
		TArray<int32>*		aTriangles,
		TArray<FVector>*	aNormals,
		TArray<FVector2D>*	aUV0,
		TArray<FColor>*		aVertexColors);

	virtual ~FZoneGeneratorWorker();

	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	virtual void Exit();


};