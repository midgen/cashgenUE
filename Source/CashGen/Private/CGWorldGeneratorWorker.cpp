#include "cashgen.h"
#include "UFNNoiseGenerator.h"
#include "CGWorldFace.h"
#include "CGWorldGeneratorWorker.h"
#include "CGWorldFaceJob.h"
#include <chrono>

using namespace std::chrono;

void FCGWorldGeneratorWorker::SubDivideGeometry(const FRuntimeMeshVertexSimple &v1, const FRuntimeMeshVertexSimple &v2, const FRuntimeMeshVertexSimple &v3, const int32 aDepth, const float aScale)
{
	if (!pWorldConfig->NoiseGenerator->IsValidLowLevel())
	{
		return;
	}

	if (aDepth == 0) {
		float v1n = pWorldConfig->NoiseGenerator->GetNoise3D(v1.Position.X, v1.Position.Y, v1.Position.Z);
		float v2n = pWorldConfig->NoiseGenerator->GetNoise3D(v2.Position.X, v2.Position.Y, v2.Position.Z);
		float v3n = pWorldConfig->NoiseGenerator->GetNoise3D(v3.Position.X, v3.Position.Y, v3.Position.Z);

		FVector v1t = v1.Position + ((v1.Position.GetSafeNormal() * v1n) * pWorldConfig->Amplitude);
		FVector v2t = v2.Position + ((v2.Position.GetSafeNormal() * v2n) * pWorldConfig->Amplitude);
		FVector v3t = v3.Position + ((v3.Position.GetSafeNormal() * v3n) * pWorldConfig->Amplitude);

		(*pVertices)[vertexIndex++] = FRuntimeMeshVertexSimple(v1t);
		(*pVertices)[vertexIndex++] = FRuntimeMeshVertexSimple(v2t);
		(*pVertices)[vertexIndex++] = FRuntimeMeshVertexSimple(v3t);


		(*pIndices)[triangleIndex++] = vertexIndex - 3;
		(*pIndices)[triangleIndex++] = vertexIndex - 2;
		(*pIndices)[triangleIndex++] = vertexIndex - 1;
		return;
	}
	const FRuntimeMeshVertexSimple v12 = FRuntimeMeshVertexSimple(((v1.Position + v2.Position).GetSafeNormal()) * aScale);
	const FRuntimeMeshVertexSimple v23 = FRuntimeMeshVertexSimple(((v2.Position + v3.Position).GetSafeNormal()) * aScale);
	const FRuntimeMeshVertexSimple v31 = FRuntimeMeshVertexSimple(((v3.Position + v1.Position).GetSafeNormal()) * aScale);
	SubDivideGeometry(v1, v12, v31, aDepth - 1, aScale);
	SubDivideGeometry(v2, v23, v12, aDepth - 1, aScale);
	SubDivideGeometry(v3, v31, v23, aDepth - 1, aScale);
	SubDivideGeometry(v12, v23, v31, aDepth - 1, aScale);
}

FCGWorldGeneratorWorker::FCGWorldGeneratorWorker(ACGWorld* aWorld, FCGWorldConfig* aWorldConfig, TQueue<FCGWorldFaceJob, EQueueMode::Spsc>* anInputQueue)
{
	pWorld = aWorld;
	pWorldConfig = aWorldConfig;
	inputQueue = anInputQueue;
}

FCGWorldGeneratorWorker::~FCGWorldGeneratorWorker()
{

}

bool FCGWorldGeneratorWorker::Init()
{
	IsThreadFinished = false;
	return true;
}

uint32 FCGWorldGeneratorWorker::Run()
{
	// Here's the loop
	while (!IsThreadFinished)
	{
		// Return complete if something is wrong.
		if (!pWorldConfig->NoiseGenerator)
		{
			return 1;
		}
		if (inputQueue->Dequeue(workJob))
		{
			pVertices = &workJob.pMeshData->Vertices;
			pIndices = &workJob.pMeshData->Indices;
			vertexIndex = 0;
			triangleIndex = 0;


			SubDivideGeometry(workJob.v1, workJob.v2, workJob.v3, workJob.SubDivisions, workJob.Radius);

			pWorld->UpdateJobs.Enqueue(workJob);
		}
		// Otherwise, take a nap
		else
		{
			FPlatformProcess::Sleep(0.01f);
		}
	}

	return 1;
}

void FCGWorldGeneratorWorker::Stop()
{
	IsThreadFinished = true;
}

void FCGWorldGeneratorWorker::Exit()
{

}
