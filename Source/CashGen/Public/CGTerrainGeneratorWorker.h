#pragma once
#include "CoreMinimal.h"
#include "CGTerrainManager.h"
#include "CGTerrainConfig.h"
#include "CGMeshData.h"

struct FCGJob;

class CASHGEN_API FCGTerrainGeneratorWorker : public FRunnable
{

	ACGTerrainManager* pTerrainManager;
	FCGTerrainConfig* pTerrainConfig;
	TQueue<FCGJob, EQueueMode::Spsc>* inputQueue;
	FCGJob workJob;
	uint8 workLOD;

	FCGMeshData* pMeshData;

	bool IsThreadFinished;

	void prepMaps();
	void ProcessTerrainMap();
	void AddDepositionToHeightMap();
	void ProcessSingleDropletErosion();
	void ProcessPerBlockGeometry();
	void ProcessPerVertexTasks();
	void ProcessSkirtGeometry();

	void erodeHeightMapAtIndex(int32 aX, int32 aY, float aAmount);
	void GetNormalFromHeightMapForVertex(const int32& vertexX, const int32& vertexY, FVector& aOutNormal);// , FVector& aOutTangent);

	void UpdateOneBlockGeometry(const int32& aX, const int32& aY, int32& aVertCounter, int32& triCounter);

	int32 GetNumberOfNoiseSamplePoints();

public:

	FCGTerrainGeneratorWorker(ACGTerrainManager* aTerrainManager,
		FCGTerrainConfig* aTerrainConfig,
		TQueue<FCGJob, EQueueMode::Spsc>* anInputQueue);

	virtual ~FCGTerrainGeneratorWorker();

	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	virtual void Exit();

};


