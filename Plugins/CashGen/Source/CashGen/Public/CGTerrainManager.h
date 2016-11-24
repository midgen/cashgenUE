#pragma once
#include "GameFramework/Actor.h"
#include "CGTile.h"
#include "Struct/CGJob.h"
#include "Struct/CGTerrainConfig.h"
#include "Struct/CGMeshData.h"
#include "Struct/CGPoint.h"
#include "Struct/CGLODMeshData.h"
#include "CGTerrainManager.generated.h"


UCLASS(BlueprintType, Blueprintable)
class ACGTerrainManager : public AActor
{
	GENERATED_BODY()

	const uint8 MESH_DATA_POOL_SIZE = 10;
	bool isSetup = false;
	CGPoint currentPlayerZone = CGPoint(0,0);

	void HandleTileFlip(CGPoint deltaTile);
	UPROPERTY()
	TArray<FCGLODMeshData> MeshData;
	TArray<TSet<FCGMeshData*>> FreeMeshData;
	TArray<TSet<FCGMeshData*>> InUseMeshData;
	bool GetFreeMeshData(FCGJob& aJob);
	void ReleaseMeshData(uint8 aLOD, FCGMeshData* aDataToRelease);
	void AllocateAllMeshDataStructures();
	bool AllocateDataStructuresForLOD(FCGMeshData* aData, FCGTerrainConfig* aConfig, const uint8 aLOD);

	void SweepLODs();
	uint8 GetLODForTile(ACGTile* aTile);

	CGPoint GetXYfromIdx(const int32 idx) { return CGPoint(idx % XTiles, idx / YTiles); }

public:
	ACGTerrainManager();
	~ACGTerrainManager();

	TArray<ACGTile*> Tiles;

	int32 XTiles;
	int32 YTiles;

	AActor* TrackingActor;

	float TimeSinceLastSweep;
	const float SweepInterval = 0.1f;

	TQueue<FCGJob, EQueueMode::Spsc> PendingJobs;
	TQueue<FCGJob, EQueueMode::Spsc> GeometryJobs;
	TQueue<FCGJob, EQueueMode::Spsc> UpdateJobs;

	TSet<ACGTile*> QueuedTiles;

	FRunnableThread* WorkerThread;



	FCGTerrainConfig TerrainConfig;



	FVector WorldOffset;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintCallable, Category = "CashGen")
	void SpawnTiles(AActor* aTrackingActor, const FCGTerrainConfig aTerrainConfig, const int32 aXTiles, const int32 aYTiles);

	void CreateTileRefreshJob(FCGJob aJob);
};