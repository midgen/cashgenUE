#pragma once
#include "CashGen/Public/CGTerrainManager.h"
#include "CashGen/Public/Struct/CGMeshData.h"
#include "CashGen/Public/Struct/CGTerrainConfig.h"

struct FCGJob;

class CASHGEN_API FCGTerrainGeneratorWorker : public FRunnable
{
public:
	FCGTerrainGeneratorWorker(ACGTerrainManager& aTerrainManager,
		FCGTerrainConfig& aTerrainConfig, TArray<TCGObjectPool<FCGMeshData>>& meshDataPoolPerLOD);

	virtual ~FCGTerrainGeneratorWorker();

	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	virtual void Exit();

private:
	ACGTerrainManager& pTerrainManager;
	FCGTerrainConfig& pTerrainConfig;
	TArray<TCGObjectPool<FCGMeshData>>& pMeshDataPoolsPerLOD;
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
	TCGBorrowedObject<FCGMeshData> BorrowMeshData();

	void erodeHeightMapAtIndex(int32 aX, int32 aY, float aAmount);
	void GetNormalFromHeightMapForVertex(const int32& vertexX, const int32& vertexY, FVector& aOutNormal); // , FVector& aOutTangent);

	void UpdateOneBlockGeometry(const int32& aX, const int32& aY, int32& aVertCounter, int32& triCounter);

	int32 GetNumberOfNoiseSamplePoints();
};
