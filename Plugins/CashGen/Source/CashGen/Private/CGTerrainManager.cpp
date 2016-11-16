#include "cashgen.h"
#include "CGTerrainManager.h"


ACGTerrainManager::ACGTerrainManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACGTerrainManager::BeginPlay()
{

}

void ACGTerrainManager::Tick(float DeltaSeconds)
{

}

void ACGTerrainManager::SpawnTiles(AActor* aTrackingActor, const FCGTerrainConfig aTerrainConfig, const int32 aXTiles, const int32 aYTiles)
{

}

void ACGTerrainManager::CreateTileRefreshJob(FCGJob aJob)
{

}
