#include "cashgen.h"
#include "ZoneConfig.h"
#include "ZoneManager.h"
#include "FMiniMapGeneratorWorker.h"
#include "CGPoint.h"

void FMiniMapGeneratorWorker::GenerateMiniMapData()
{
	//for (FColor& elem : pColorData)
	//{
	//	elem.R = 0;
	//	elem.G = 0;
	//	elem.B = 255;
	//}
}

FMiniMapGeneratorWorker::FMiniMapGeneratorWorker(AZoneManager* apZoneManager, FZoneConfig* aZoneConfig, CGPoint* aOffSet, TArray<float>* aHeightMapData, TArray<FColor>* aColorData)
{

}

FMiniMapGeneratorWorker::~FMiniMapGeneratorWorker()
{

}

bool FMiniMapGeneratorWorker::Init()
{
	return true;
}

uint32 FMiniMapGeneratorWorker::Run()
{
	GenerateMiniMapData();

	return 1;
}

void FMiniMapGeneratorWorker::Stop()
{

}

void FMiniMapGeneratorWorker::Exit()
{

}

