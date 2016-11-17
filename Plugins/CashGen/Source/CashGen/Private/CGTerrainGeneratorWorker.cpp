#include "cashgen.h"
#include "UFNNoiseGenerator.h"
#include "CGTerrainGeneratorWorker.h"




FCGTerrainGeneratorWorker::FCGTerrainGeneratorWorker(ACGTerrainManager* aTerrainManager, FCGTerrainConfig* aTerrainConfig, CGPoint* aOffset, FCGMeshData* aMeshData)
{

}

FCGTerrainGeneratorWorker::~FCGTerrainGeneratorWorker()
{

}

bool FCGTerrainGeneratorWorker::Init()
{
	IsThreadFinished = false;
	return true;
}

uint32 FCGTerrainGeneratorWorker::Run()
{
	FCGJob job;
	// Here's the loop
	while (!IsThreadFinished)
	{
		// If there's a job, process it!
		if (pTerrainManager->GeometryJobs.Dequeue(job))
		{
			pMeshData = job.Data;

			ProcessTerrainMap();
			ProcessPerBlockGeometry();
			ProcessPerVertexTasks();
		}
		// Otherwise, take a nap
		else
		{
			Sleep(30);
		}
	}


	return 1;
}

void FCGTerrainGeneratorWorker::Stop()
{

}

void FCGTerrainGeneratorWorker::Exit()
{

}

void FCGTerrainGeneratorWorker::ProcessTerrainMap()
{

}

void FCGTerrainGeneratorWorker::ProcessPerBlockGeometry()
{

}

void FCGTerrainGeneratorWorker::ProcessPerVertexTasks()
{

}

void FCGTerrainGeneratorWorker::ProcessBiomeWeightMap()
{

}

FVector FCGTerrainGeneratorWorker::GetNormalFromHeightMapForVertex(const int32 vertexX, const int32 vertexY)
{
	FVector result;

	return result;
}

FRuntimeMeshTangent FCGTerrainGeneratorWorker::GetTangentFromNormal(const FVector aNormal)
{
	FRuntimeMeshTangent result;

	return result;
}

void FCGTerrainGeneratorWorker::UpdateOneBlockGeometry(const int aX, const int aY, int32& aVertCounter, int32& triCounter)
{

}

int32 FCGTerrainGeneratorWorker::GetNumberOfNoiseSamplePoints()
{
	return 0;
}
