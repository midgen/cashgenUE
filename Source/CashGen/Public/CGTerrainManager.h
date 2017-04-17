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

	const uint8 MESH_DATA_POOL_SIZE = 10; // The number of mesh data structs to have in the pool
	bool isSetup = false;
	CGPoint currentPlayerZone = CGPoint(0,0);
	FRunnableThread* WorkerThread;

	void HandleTileFlip(CGPoint deltaTile);
	UPROPERTY()
	TArray<FCGLODMeshData> MeshData;
	TArray<TSet<FCGMeshData*>> FreeMeshData;
	TArray<TSet<FCGMeshData*>> InUseMeshData;
	FVector WorldOffset;
	TArray<ACGTile*> Tiles;

	bool GetFreeMeshData(FCGJob& aJob);
	void ReleaseMeshData(uint8 aLOD, FCGMeshData* aDataToRelease);
	void AllocateAllMeshDataStructures();
	bool AllocateDataStructuresForLOD(FCGMeshData* aData, FCGTerrainConfig* aConfig, const uint8 aLOD);
	void CreateTileRefreshJob(FCGJob aJob);
	void SweepLODs();
	uint8 GetLODForTile(ACGTile* aTile);

	CGPoint GetXYfromIdx(const int32 idx) { return CGPoint(idx % XTiles, idx / YTiles); }
	float TimeSinceLastSweep;
	const float SweepInterval = 0.1f;

public:
	ACGTerrainManager();
	~ACGTerrainManager();

	UPROPERTY()
	AActor* TrackingActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
	int32 XTiles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
	int32 YTiles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
	FCGTerrainConfig TerrainConfig;


	UFUNCTION(BlueprintCallable, Category = "CashGen")
	void SetUpTerrain(UUFNNoiseGenerator* aNoiseGen, UUFNNoiseGenerator* aBiomeBlendGen, AActor* aTrackingActor) { TerrainConfig.NoiseGenerator = aNoiseGen; TerrainConfig.BiomeBlendGenerator = aBiomeBlendGen; TrackingActor = aTrackingActor; }



	TQueue<FCGJob, EQueueMode::Spsc> PendingJobs;
	TQueue<FCGJob, EQueueMode::Spsc> GeometryJobs;
	TQueue<FCGJob, EQueueMode::Spsc> UpdateJobs;

	TSet<ACGTile*> QueuedTiles;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintCallable, Category = "CashGen")
	void SpawnTiles(AActor* aTrackingActor, const FCGTerrainConfig aTerrainConfig, const int32 aXTiles, const int32 aYTiles);

	
};