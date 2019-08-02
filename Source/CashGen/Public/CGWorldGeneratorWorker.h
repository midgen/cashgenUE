#pragma once
#include "CoreMinimal.h"
#include "CGTerrainManager.h"
#include "CGTerrainConfig.h"
#include "CGWorldFaceJob.h"

struct FCGWorldConfig;
class ACGWorld;

class CASHGEN_API FCGWorldGeneratorWorker : public FRunnable
{
	ACGWorld* pWorld;
	TQueue<FCGWorldFaceJob, EQueueMode::Spsc>* inputQueue;
	FCGWorldFaceJob workJob;

	FCGWorldConfig* pWorldConfig;

	TArray<FRuntimeMeshVertexSimple>*	pVertices;
	TArray<int32>*		pIndices;

	int32 vertexIndex = 0;
	int32 triangleIndex = 0;

	bool IsThreadFinished;
	void SubDivideGeometry(const FRuntimeMeshVertexSimple &v1, const FRuntimeMeshVertexSimple &v2, const FRuntimeMeshVertexSimple &v3, const int32 aDepth, const float aScale);

public:

	FCGWorldGeneratorWorker(ACGWorld* aWorld,
		FCGWorldConfig* aWorldConfig,
		TQueue<FCGWorldFaceJob, EQueueMode::Spsc>* anInputQueue);

	virtual ~FCGWorldGeneratorWorker();
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	virtual void Exit();
};


