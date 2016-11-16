#pragma once
#include "GameFramework/Actor.h"
#include "CGTile.h"
#include "Struct/CGJob.h"
#include "Struct/CGTerrainConfig.h"
#include "Struct/CGMeshData.h"
#include "CGTerrainManager.generated.h"


UCLASS(BlueprintType, Blueprintable)
class ACGTerrainManager : public AActor
{
	GENERATED_BODY()

public:
	ACGTerrainManager();

	TArray<ACGTile*> Tiles;

	int32 XTiles;
	int32 YTiles;

	AActor* TrackingActor;

	float TimeSinceLastSweep;
	const float SweepInterval = 1.0f;

	TQueue<FCGJob, EQueueMode::Spsc> GeometryJobs;

	FRunnableThread* WorkerThread;

	TQueue<FCGJob, EQueueMode::Mpsc> UpdateJobs;

	FCGTerrainConfig TerrainConfig;

	TMap<uint8, FCGMeshData> MeshData;

	FVector WorldOffset;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintCallable, Category = "CashGen")
	void SpawnTiles(AActor* aTrackingActor, const FCGTerrainConfig aTerrainConfig, const int32 aXTiles, const int32 aYTiles);

	void CreateTileRefreshJob(FCGJob aJob);
};