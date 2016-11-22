#pragma once
#include "cashgen.h"
#include "CGTerrainManager.h"
#include "CGTerrainConfig.h"



class FCGTerrainGeneratorWorker : public FRunnable
{

	ACGTerrainManager* pTerrainManager;
	FCGTerrainConfig* pTerrainConfig;
	//TSharedPtr<FCGMeshData> pMeshData;
	FCGJob workJob;
	uint8 workLOD;

	TSharedPtr<TArray<FVector>, ESPMode::ThreadSafe>	pVertices;
	TSharedPtr<TArray<int32>, ESPMode::ThreadSafe>		pTriangles;
	TSharedPtr<TArray<FVector>, ESPMode::ThreadSafe>	pNormals;
	TSharedPtr<TArray<FVector2D>, ESPMode::ThreadSafe>	pUV0;
	TSharedPtr<TArray<FColor>, ESPMode::ThreadSafe>		pVertexColors;
	TSharedPtr<TArray<FRuntimeMeshTangent>, ESPMode::ThreadSafe> pTangents;
	TSharedPtr<TArray<FVector>, ESPMode::ThreadSafe> pHeightMap;

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


