#include "cashgen.h"
#include "UFNNoiseGenerator.h"
#include "CGTerrainGeneratorWorker.h"




FCGTerrainGeneratorWorker::FCGTerrainGeneratorWorker(ACGTerrainManager* aTerrainManager, FCGTerrainConfig* aTerrainConfig)
{
	pTerrainManager = aTerrainManager;
	pTerrainConfig = aTerrainConfig;
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
	// Here's the loop
	while (!IsThreadFinished)
	{
		if (pTerrainManager->GeometryJobs.Dequeue(workJob))
		{
			//pMeshData = workJob.Data;

			pVertices = workJob.Vertices;
			pTriangles = workJob.Triangles;
			pNormals = workJob.Normals;
			pUV0 = workJob.UV0;
			pVertexColors = workJob.VertexColors;
			pTangents = workJob.Tangents;
			pHeightMap = workJob.HeightMap;
			pDepositionMap = workJob.DespositionMap;

			workLOD = workJob.LOD;

			ProcessTerrainMap();
			if (workLOD == 0)
			{
				for (int32 i = 0; i < pTerrainConfig->ThermalErosionIterations; ++i)
				{
					ProcessThermalErosion();
					AddDepositionToHeightMap();
				}
				for (int32 i = 0; i < pTerrainConfig->DropletAmount; ++i)
				{
					ProcessSingleDropletErosion();
				}
			}

			ProcessPerBlockGeometry();
			ProcessPerVertexTasks();


			pTerrainManager->UpdateJobs.Enqueue(workJob);
		}
		// Otherwise, take a nap
		else
		{
			Sleep(10);
		}
	}

	return 1;
}

void FCGTerrainGeneratorWorker::Stop()
{
	IsThreadFinished = true;
}

void FCGTerrainGeneratorWorker::Exit()
{

}

void FCGTerrainGeneratorWorker::ProcessTerrainMap()
{
	// Size of the noise sampling (larger than the actual mesh so we can have seamless normals)
	int32 exX = GetNumberOfNoiseSamplePoints();
	int32 exY = exX;

	int32 exUnitSize = workLOD == 0 ? pTerrainConfig->UnitSize : pTerrainConfig->UnitSize * pTerrainConfig->LODs[workLOD].ResolutionDivisor;

	// Calculate the new noisemap
	for (int x = 0; x < exX; ++x)
	{
		for (int y = 0; y < exY; ++y)
		{
			int32 worldX = (((workJob.Tile->Offset.X * (exX - 3) + x)) * exUnitSize) - exUnitSize;
			int32 worldY = (((workJob.Tile->Offset.Y * (exY - 3) + y)) * exUnitSize) - exUnitSize;

			(*pHeightMap)[x + (exX*y)] = FVector(x* exUnitSize, y*exUnitSize, pTerrainConfig->NoiseGenerator->GetNoise2D(worldX, worldY) * pTerrainConfig->Amplitude);
		}
	}
}

void FCGTerrainGeneratorWorker::ProcessThermalErosion()
{
	int32 XUnits = GetNumberOfNoiseSamplePoints();
	int32 YUnits = XUnits;

	const float talus = pTerrainConfig->ThermalErosionThreshold;

	for (auto& depo : (*pDepositionMap))
	{
		depo = 0.0f;
	}

	for (int32 x = 1; x < XUnits - 1; ++x)
	{
		for (int32 y = 1; y < YUnits - 1; ++y)
		{
			float* h = &(*pHeightMap)[x + (XUnits * y)].Z;
			float* hu = &(*pHeightMap)[x + (XUnits * (y + 1))].Z;
			float* hd = &(*pHeightMap)[x + (XUnits * (y - 1))].Z;
			float* hl = &(*pHeightMap)[x + 1 + (XUnits * y)].Z;
			float* hr = &(*pHeightMap)[x - 1 + (XUnits * y)].Z;
			float* hul = &(*pHeightMap)[x + 1 +(XUnits * (y + 1))].Z;
			float* hdl = &(*pHeightMap)[x + 1 +(XUnits * (y - 1))].Z;
			float* hur = &(*pHeightMap)[x - 1 + (XUnits * (y+1))].Z;
			float* hdr = &(*pHeightMap)[x - 1 + (XUnits * (y-1))].Z;

			float nhu  = 0.0f;
			float nhd  = 0.0f;
			float nhr  = 0.0f;
			float nhl  = 0.0f;
			float nhul = 0.0f;
			float nhdl = 0.0f;
			float nhur = 0.0f;
			float nhdr = 0.0f;

			float dl = *h - *hl; 
			if (dl > talus) {
				nhl += (pTerrainConfig->ThermalErosionDepositionAmount * (dl - talus));
			}

			float dr = *h - *hr;
			if (dr > talus) {
				nhr += (pTerrainConfig->ThermalErosionDepositionAmount * (dr - talus));
			}

			float du = *h - *hu;
			if (du > talus) {
				nhu += (pTerrainConfig->ThermalErosionDepositionAmount * (du - talus));
			}

			float dd = *h - *hd;
			if (dd > talus) {
				nhd += (pTerrainConfig->ThermalErosionDepositionAmount * (dd - talus));
			}

			float dul = *h - *hul;
			if (dul > talus) {
				nhul += (pTerrainConfig->ThermalErosionDepositionAmount * (dul - talus));
			}

			float dur = *h - *hur;
			if (dur > talus) {
				nhur += (pTerrainConfig->ThermalErosionDepositionAmount * (dur - talus));
			}

			float ddl = *h - *hdl;
			if (ddl > talus) {
				nhdl += (pTerrainConfig->ThermalErosionDepositionAmount * (ddl - talus));
			}

			float ddr = *h - *hdr;
			if (ddr > talus) {
				nhdr += (pTerrainConfig->ThermalErosionDepositionAmount * (ddr - talus));
			}

			(*pDepositionMap)[x + (XUnits * (y + 1))]    += nhu;
			(*pDepositionMap)[x + (XUnits * (y - 1))]   += nhd;
			(*pDepositionMap)[x + 1 + (XUnits * y)]     += nhl;
			(*pDepositionMap)[x - 1 + (XUnits * y)]     += nhr;
			(*pDepositionMap)[x + 1 + (XUnits * (y + 1))] += nhul;
			(*pDepositionMap)[x + 1 + (XUnits * (y - 1))] += nhdl;
			(*pDepositionMap)[x - 1 + (XUnits * (y + 1))] += nhur;
			(*pDepositionMap)[x - 1 + (XUnits * (y - 1))] += nhdr;

		}
	}
}

void FCGTerrainGeneratorWorker::AddDepositionToHeightMap()
{
	int32 index = 0;
	for (FVector& heightPoint : (*pHeightMap))
	{
		heightPoint.Z += (*pDepositionMap)[index];
		++index;
	}
}


void FCGTerrainGeneratorWorker::erodeHeightMapAtIndex(int32 aX, int32 aY, float aAmount)
{
	int32 XUnits = GetNumberOfNoiseSamplePoints();
	float mod1 = 0.5f;
	float mod2 = 0.4f;

	(*pHeightMap)[aX + (XUnits * aY)].Z -= aAmount;
	(*pHeightMap)[aX + (XUnits * (aY + 1))].Z		-= aAmount * mod1;
	(*pHeightMap)[aX + (XUnits * (aY - 1))].Z		-= aAmount * mod1;
	(*pHeightMap)[aX + 1 + (XUnits * (aY))].Z		-= aAmount * mod1;
	(*pHeightMap)[aX - 1 + (XUnits * (aY))].Z		-= aAmount * mod1;

	(*pHeightMap)[aX + 1 + (XUnits * (aY + 1))].Z	-= aAmount * mod2;
	(*pHeightMap)[aX + 1 + (XUnits * (aY - 1))].Z	-= aAmount * mod2;
	(*pHeightMap)[aX - 1 + (XUnits * (aY + 1))].Z	-= aAmount * mod2;
	(*pHeightMap)[aX - 1 + (XUnits * (aY - 1))].Z	-= aAmount * mod2;


}

void FCGTerrainGeneratorWorker::ProcessSingleDropletErosion()
{
	int32 XUnits = GetNumberOfNoiseSamplePoints();
	int32 YUnits = XUnits;

	// Pick a random start point that isn't on an edge
	int32 cX = FMath::RandRange(1, XUnits - 1);
	int32 cY = FMath::RandRange(1, YUnits - 1);

	float sedimentAmount = 0.0f;
	float waterAmount	 = 1.0f;
	FVector velocity = FVector(0.0f, 0.0f, 1.0f);
	

	while (waterAmount > 0.0f && cX > 0 && cX < XUnits - 1 && cY > 0 && cY < YUnits - 1)
	{
		FVector origin = (*pHeightMap)[cX + (XUnits * cY)];
		FVector up = ((*pHeightMap)[cX + (XUnits * (cY + 1))] - origin).GetSafeNormal();
		FVector down = ((*pHeightMap)[cX + (XUnits * (cY - 1))] - origin).GetSafeNormal();
		FVector left = ((*pHeightMap)[cX + 1 + (XUnits * (cY))] - origin).GetSafeNormal();
		FVector right = ((*pHeightMap)[cX - 1 + (XUnits * (cY))] - origin).GetSafeNormal();
		
		FVector upleft = ((*pHeightMap)[cX + 1 + (XUnits * (cY + 1))] - origin).GetSafeNormal();
		FVector downleft = ((*pHeightMap)[cX + 1 + (XUnits * (cY - 1))] - origin).GetSafeNormal();
		FVector upright = ((*pHeightMap)[cX - 1 + (XUnits * (cY + 1))] - origin).GetSafeNormal();
		FVector downright = ((*pHeightMap)[cX - 1 + (XUnits * (cY - 1))] - origin).GetSafeNormal();

		FVector lowestRoute = FVector(0.0f);

		int32 newCx = cX;
		int32 newCy = cY;

		if (up.Z < lowestRoute.Z) { lowestRoute = up; newCy++; }
		if (down.Z < lowestRoute.Z) { lowestRoute = down; newCy--; }
		if (left.Z < lowestRoute.Z) { lowestRoute = left; newCx++; }
		if (right.Z < lowestRoute.Z) { lowestRoute = right; newCx--; }
		if (upleft.Z < lowestRoute.Z) { lowestRoute = upleft; newCy++; newCx++; }
		if (upright.Z < lowestRoute.Z) { lowestRoute = upright; newCy++; newCx--; }
		if (downleft.Z < lowestRoute.Z) { lowestRoute = downleft; newCy--; newCx++; }
		if (downright.Z < lowestRoute.Z) { lowestRoute = downright; newCy--; newCx--; }

		// The amount of sediment to pick up depends on if we are hitting an obstacle
		float sedimentUptake = pTerrainConfig->DropletErosionMultiplier * FVector::DotProduct(velocity, lowestRoute);
		if (sedimentUptake < 0.0f) { sedimentUptake = 0.0f; }

		sedimentAmount += sedimentUptake;

		float sedimentDeposit = 0.0f;
		// Deposit sediment if we are carrying too much
		if (sedimentAmount > pTerrainConfig->DropletSedimentCapacity)
		{
			sedimentDeposit = (sedimentAmount - pTerrainConfig->DropletSedimentCapacity) * pTerrainConfig->DropletDespositionMultiplier;
		}

		// Deposit based on slope
		sedimentDeposit += sedimentAmount * FMath::Clamp(1.0f + lowestRoute.Z, 0.0f, 1.0f);

		sedimentAmount -= sedimentDeposit;
		

		velocity = lowestRoute;

		//(*pHeightMap)[cX + (XUnits * cY)].Z -= sedimentUptake + (sedimentDeposit * -1.0f);
		erodeHeightMapAtIndex(cX,cY, (sedimentUptake + (sedimentDeposit * -1.0f)));
		
		waterAmount -= pTerrainConfig->DropletEvaporationRate;

		cX = newCx;
		cY = newCy;

	}

}

//void FCGTerrainGeneratorWorker::ProcessSingleDropletErosion()
//{
//	int32 XUnits = GetNumberOfNoiseSamplePoints();
//	int32 YUnits = XUnits;
//
//	// Pick a random start point that isn't on an edge
//	int32 cX = FMath::RandRange(1, XUnits -1);
//	int32 cY = FMath::RandRange(1, YUnits -1);
//
//	float waterAmount = 1.0f;
//	float velocity = 0.0f;
//	float sedimentAmount = pTerrainConfig->DropletDespositionTheta;
//	uint8 direction = 0;
//	uint8 previousDirection = 0;
//
//	while (waterAmount > 0.0f && cX > 0 && cX < XUnits - 1 && cY > 0 && cY < YUnits -1)
//	{
//		float origin	= (*pHeightMap)[cX + (XUnits * cY)].Z;
//		float up		= (*pHeightMap)[cX + (XUnits * (cY+1))].Z;
//		float down		= (*pHeightMap)[cX + (XUnits * (cY - 1))].Z;
//		float left		= (*pHeightMap)[cX + 1 + (XUnits * (cY))].Z;
//		float right		= (*pHeightMap)[cX - 1 + (XUnits * (cY))].Z;
//
//		float upleft = (*pHeightMap)[cX + 1 +(XUnits * (cY + 1))].Z;
//		float downleft = (*pHeightMap)[cX + 1 + (XUnits * (cY - 1))].Z;
//		float upright = (*pHeightMap)[cX - 1 + (XUnits * (cY + 1))].Z;
//		float downright = (*pHeightMap)[cX - 1 + (XUnits * (cY - 1))].Z;
//
//		float dU = up - origin;
//		float dD = down - origin;
//		float dL = left - origin;
//		float dR = right - origin;
//
//		float dUL = upleft - origin;
//		float dUR = upright - origin;
//		float dDL = downleft - origin;
//		float dDR = downright - origin;
//
//
//		float dMax = 0.0;
//
//		if (dU > dMax) { dMax = dU; }
//		if (dD > dMax) { dMax = dD; }
//		if (dL > dMax) { dMax = dL; }
//		if (dR > dMax) { dMax = dR; }
//
//
//		if (dUL > dMax) { dMax = dUL; }
//		if (dUR > dMax) { dMax = dUR; }
//		if (dDL > dMax) { dMax = dDL; }
//		if (dDR > dMax) { dMax = dDR; }
//
//		float depositAmount = sedimentAmount * pTerrainConfig->DropletDespositionRate; 
//
//		(*pHeightMap)[cX + (XUnits * (cY))].Z -= depositAmount;
//
//		//// if the slope on this grid is less than previous, deposit some sediment
//		//if (dMax < velocity)
//		//{
//		//	(*pHeightMap)[cX + (XUnits * (cY))].Z += depositAmount * 0.1f;
//		//	sedimentAmount -= depositAmount * 0.1f;
//		//}
//
//		//velocity = dMax;
//
//		previousDirection = direction;
//
//		if (dU >= dMax - 0.00001f) // Flow UP
//		{
//			direction = 0;
//			if (cY + 1 < YUnits - 1) {
//				//(*pHeightMap)[cX + (XUnits * (cY + 1))].Z += sedimentAmount * pTerrainConfig->DropletDespositionRate;
//			}
//
//			cY++;
//		} 
//		else if (dD >= dMax - 0.00001f) // Flow DOWN
//		{
//			direction = 4;
//			if (cY - 1 > 0) {
//				//(*pHeightMap)[cX + (XUnits * (cY - 1))].Z += sedimentAmount * pTerrainConfig->DropletDespositionRate;
//			}
//			cY--;
//		}
//		else if (dL >= dMax - 0.00001f) // Flow LEFT
//		{
//			direction = 6;
//			if (cX + 1 < XUnits - 1) {
//				//(*pHeightMap)[cX + 1 + (XUnits * (cY))].Z += sedimentAmount * pTerrainConfig->DropletDespositionRate;
//			}
//			cX++;
//		}
//		else if (dR >= dMax - 0.00001f) // Flow RIGHT
//		{
//			direction = 2;
//			if (cX - 1 > 0) {
//				//(*pHeightMap)[cX - 1 + (XUnits * (cY))].Z += sedimentAmount * pTerrainConfig->DropletDespositionRate;
//			}
//			cX--;
//		}
//
//		if (dUL >= dMax - 0.00001f) // Flow UPLEFT
//		{
//			direction = 7;
//			if (cY + 1 < YUnits - 1 && cX + 1 < XUnits - 1) {
//				//(*pHeightMap)[cX + (XUnits * (cY + 1))].Z += sedimentAmount * pTerrainConfig->DropletDespositionRate;
//			}
//
//			cY++; cX++;
//		}
//		else if (dUL >= dMax - 0.00001f) // Flow UPRIGHT
//		{
//			direction = 1;
//			if (cY + 1 < YUnits - 1 && cX - 1 > 0) {
//				//(*pHeightMap)[cX + (XUnits * (cY + 1))].Z += sedimentAmount * pTerrainConfig->DropletDespositionRate;
//			}
//
//			cY++; cX--;
//		}
//		else if (dUL >= dMax - 0.00001f) // Flow DOWNLEFT
//		{
//			direction = 5;
//			if (cY - 1 > 0 && cX + 1 < XUnits - 1) {
//				//(*pHeightMap)[cX + (XUnits * (cY + 1))].Z += sedimentAmount * pTerrainConfig->DropletDespositionRate;
//			}
//
//			cY--; cX++;
//		}
//		else if (dUL >= dMax - 0.00001f) // Flow DOWNRIGHT
//		{
//			direction = 3;
//			if (cY - 1 > 0 && cX - 1 > 0) {
//				//(*pHeightMap)[cX + (XUnits * (cY + 1))].Z += sedimentAmount * pTerrainConfig->DropletDespositionRate;
//			}
//
//			cY--; cX--;
//		}
//
//		if (previousDirection != direction)
//		{
//			(*pHeightMap)[cX + (XUnits * (cY))].Z += depositAmount * 0.1f;
//			sedimentAmount += depositAmount * 0.1f;
//		}
//
//		waterAmount -= pTerrainConfig->DropletEvaporationRate;
//
//	}
//}

void FCGTerrainGeneratorWorker::ProcessPerBlockGeometry()
{
	int32 vertCounter = 0;
	int32 triCounter = 0;

	int32 xUnits = workLOD == 0 ? pTerrainConfig->TileXUnits : (pTerrainConfig->TileXUnits / pTerrainConfig->LODs[workLOD].ResolutionDivisor);
	int32 yUnits = workLOD == 0 ? pTerrainConfig->TileYUnits : (pTerrainConfig->TileYUnits / pTerrainConfig->LODs[workLOD].ResolutionDivisor);

	// Generate the mesh data for each block
	for (int32 y = 0; y < yUnits; ++y)
	{
		for (int32 x = 0; x < xUnits; ++x)
		{
			UpdateOneBlockGeometry(x, y, vertCounter, triCounter);
		}
	}
}

void FCGTerrainGeneratorWorker::ProcessPerVertexTasks()
{
	int32 xUnits = workLOD == 0 ? pTerrainConfig->TileXUnits : (pTerrainConfig->TileXUnits / pTerrainConfig->LODs[workLOD].ResolutionDivisor);
	int32 yUnits = workLOD == 0 ? pTerrainConfig->TileYUnits : (pTerrainConfig->TileYUnits / pTerrainConfig->LODs[workLOD].ResolutionDivisor);

	int32 rowLength = workLOD == 0 ? pTerrainConfig->TileXUnits + 1 : (pTerrainConfig->TileXUnits / (pTerrainConfig->LODs[workLOD].ResolutionDivisor) + 1);

	for (int32 y = 0; y < yUnits + 1; ++y)
	{
		for (int32 x = 0; x < xUnits + 1; ++x)
		{
			(*pNormals)[x + (y * rowLength)] = GetNormalFromHeightMapForVertex(x, y);
			// TODO: Pretty sure this is wrong, so out it goes for now
			//(*pTangents)[x + (y * rowLength)] = GetTangentFromNormal((*pNormals)[x + (y * rowLength)]);
		}
	}
}

void FCGTerrainGeneratorWorker::ProcessBiomeWeightMap()
{

}

FVector FCGTerrainGeneratorWorker::GetNormalFromHeightMapForVertex(const int32 vertexX, const int32 vertexY)
{
	FVector result;

	int32 rowLength = workLOD == 0 ? pTerrainConfig->TileXUnits + 1 : (pTerrainConfig->TileXUnits / (pTerrainConfig->LODs[workLOD].ResolutionDivisor) + 1);
	int32 heightMapRowLength = rowLength + 2;

	// the heightmapIndex for this vertex index
	int32 heightMapIndex = vertexX + 1 + ((vertexY + 1) * heightMapRowLength);

	// Get the 4 neighbouring points
	FVector up, down, left, right, upleft, upright, downleft, downright;

	up = (*pHeightMap)[heightMapIndex + heightMapRowLength] - (*pHeightMap)[heightMapIndex];
	down = (*pHeightMap)[heightMapIndex - heightMapRowLength] - (*pHeightMap)[heightMapIndex];
	left = (*pHeightMap)[heightMapIndex + 1] - (*pHeightMap)[heightMapIndex];
	right = (*pHeightMap)[heightMapIndex - 1] - (*pHeightMap)[heightMapIndex];

	FVector n1, n2, n3, n4;

	n1 = FVector::CrossProduct(left, up);
	n2 = FVector::CrossProduct(up, right);
	n3 = FVector::CrossProduct(right, down);
	n4 = FVector::CrossProduct(down, left);

	result = n1 + n2 + n3 + n4;

	return result.GetSafeNormal();
}

FRuntimeMeshTangent FCGTerrainGeneratorWorker::GetTangentFromNormal(const FVector aNormal)
{
	FVector tangentVec, bitangentVec;
	FVector c1, c2;

	c1 = FVector::CrossProduct(aNormal, FVector(0.0f, 0.0f, 1.0f));
	c2 = FVector::CrossProduct(aNormal, FVector(0.0f, 1.0f, 0.0f));

	if (c1.Size() > c2.Size())
	{
		tangentVec = c1;
	}
	else
	{
		tangentVec = c2;
	}

	tangentVec = tangentVec.GetSafeNormal();
	bitangentVec = FVector::CrossProduct(aNormal, tangentVec);

	return FRuntimeMeshTangent(bitangentVec, false);
}

void FCGTerrainGeneratorWorker::UpdateOneBlockGeometry(const int aX, const int aY, int32& aVertCounter, int32& triCounter)
{
	int32 thisX = aX;
	int32 thisY = aY;
	int32 heightMapX = thisX + 1;
	int32 heightMapY = thisY + 1;
	// LOD adjusted dimensions
	int32 rowLength = workLOD == 0 ? pTerrainConfig->TileXUnits + 1 : (pTerrainConfig->TileXUnits / (pTerrainConfig->LODs[workLOD].ResolutionDivisor) + 1);
	int32 heightMapRowLength = rowLength + 2;
	// LOD adjusted unit size
	int32 exUnitSize = workLOD == 0 ? pTerrainConfig->UnitSize : pTerrainConfig->UnitSize * (pTerrainConfig->LODs[workLOD].ResolutionDivisor);

	FVector heightMapToWorldOffset = FVector(exUnitSize, exUnitSize, 0.0f);

	// BR
	(*pVertices)[thisX + (thisY * rowLength)] = (*pHeightMap)[heightMapX + (heightMapY * heightMapRowLength)] - heightMapToWorldOffset;
	// TR
	(*pVertices)[thisX + ((thisY + 1) * rowLength)] = (*pHeightMap)[heightMapX + ((heightMapY + 1) * heightMapRowLength)] - heightMapToWorldOffset;
	// BL
	(*pVertices)[(thisX + 1) + (thisY * rowLength)] = (*pHeightMap)[(heightMapX + 1) + (heightMapY * heightMapRowLength)] - heightMapToWorldOffset;
	// BR
	(*pVertices)[(thisX + 1) + ((thisY + 1) * rowLength)] = (*pHeightMap)[(heightMapX + 1) + ((heightMapY + 1) * heightMapRowLength)] - heightMapToWorldOffset;

	//TODO: Not using Vertex Colour channels at the moment, could be handy though!
	//(*pVertexColors)[thisX + (thisY * rowLength)].R = (255 / 50000.0f);
	//(*pVertexColors)[thisX + ((thisY + 1) * rowLength)].R = (255 / 50000.0f);
	//(*pVertexColors)[(thisX + 1) + (thisY * rowLength)].R = (255 / 50000.0f);
	//(*pVertexColors)[(thisX + 1) + ((thisY + 1) * rowLength)].R = (255 / 50000.0f);
}

int32 FCGTerrainGeneratorWorker::GetNumberOfNoiseSamplePoints()
{
	return workLOD == 0 ? pTerrainConfig->TileXUnits + 3 : (pTerrainConfig->TileXUnits / (pTerrainConfig->LODs[workLOD].ResolutionDivisor)) + 3;
}
