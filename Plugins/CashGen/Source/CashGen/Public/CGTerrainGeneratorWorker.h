#pragma once
#include "cashgen.h"
#include "CGTerrainManager.h"
#include "CGTerrainConfig.h"



class FCGTerrainGeneratorWorker : public FRunnable
{

	ACGTerrainManager* pTerrainManager;
	FCGTerrainConfig* pTerrainConfig;
	CGPoint* pOffset;

	FCGMeshData* pMeshData;
	uint8 workLOD;

	bool IsThreadFinished;

	void ProcessTerrainMap();
	void ProcessPerBlockGeometry();
	void ProcessPerVertexTasks();
	void ProcessBiomeWeightMap();
	FVector GetNormalFromHeightMapForVertex(const int32 vertexX, const int32 vertexY);
	FRuntimeMeshTangent GetTangentFromNormal(const FVector aNormal);
	void UpdateOneBlockGeometry(const int aX, const int aY, int32& aVertCounter, int32& triCounter);

	int32 GetNumberOfNoiseSamplePoints();

public:

	FCGTerrainGeneratorWorker(ACGTerrainManager* aTerrainManager,
		FCGTerrainConfig* aTerrainConfig);

	virtual ~FCGTerrainGeneratorWorker();

	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	virtual void Exit();

};


