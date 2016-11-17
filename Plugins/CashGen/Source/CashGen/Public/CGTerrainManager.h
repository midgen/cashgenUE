#pragma once
#include "GameFramework/Actor.h"
#include "CGTile.h"
#include "Struct/CGJob.h"
#include "Struct/CGTerrainConfig.h"
#include "Struct/CGMeshData.h"
#include "Struct/CGPoint.h"
#include "CGTerrainManager.generated.h"


UCLASS(BlueprintType, Blueprintable)
class ACGTerrainManager : public AActor
{
	GENERATED_BODY()

	bool isSetup = false;
	CGPoint currentPlayerZone;

	void HandleTileFlip(CGPoint deltaTile);

	TArray<TArray<FCGMeshData>> MeshData;
	TArray<TSet<FCGMeshData*>> FreeMeshData;
	TArray<TSet<FCGMeshData*>> InUseMeshData;
	FCGMeshData* GetFreeMeshData(uint8 aLOD);
	void ReleaseMeshData(uint8 aLOD, FCGMeshData* aDataToRelease);

	void SweepLODs();
	uint8 GetLODForTile(ACGTile* aTile);

	CGPoint GetXYfromIdx(const int32 idx) { return CGPoint(idx % XTiles, idx / YTiles); }

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



	FVector WorldOffset;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintCallable, Category = "CashGen")
	void SpawnTiles(AActor* aTrackingActor, const FCGTerrainConfig aTerrainConfig, const int32 aXTiles, const int32 aYTiles);

	void CreateTileRefreshJob(FCGJob aJob);
};