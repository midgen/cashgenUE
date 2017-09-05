#pragma once
#include "GameFramework/Actor.h"
#include "Struct/CGJob.h"
#include "Struct/CGTerrainConfig.h"
#include "Struct/CGMeshData.h"
#include "Struct/CGTileHandle.h"
#include "Struct/IntVector2.h"
#include "Struct/CGSector.h"
#include "Struct/CGLODMeshData.h"
#include "CGTerrainManager.generated.h"

class ACGTile;

UCLASS(BlueprintType, Blueprintable)
class ACGTerrainManager : public AActor
{
	GENERATED_BODY()

	// Master config
	UPROPERTY()
	FCGTerrainConfig myTerrainConfig;

	// Sweep tracking
	float myTimeSinceLastSweep = 0.0f;
	const float mySweepTime = 2.0f;
	uint8 myActorIndex = 0;

	// Threads
	TArray<FRunnableThread*> myWorkerThreads;

	// Geometry data storage
	UPROPERTY()
	TArray<FCGLODMeshData> myMeshData;
	TArray<TSet<FCGMeshData*>> myFreeMeshData;
	TArray<TSet<FCGMeshData*>> myInUseMeshData;

	// Job tracking
	TQueue<FCGJob, EQueueMode::Spsc> myPendingJobQueue;
	TArray<TQueue<FCGJob, EQueueMode::Spsc>> myGeometryJobQueues;

	// Tile/Sector tracking
	TArray<ACGTile*> myFreeTiles;
	UPROPERTY()
	TMap<FIntVector2, FCGTileHandle> myTileHandleMap;
	TSet<FIntVector2> myQueuedSectors;

	// Actor tracking
	TArray<AActor*> myTrackedActors;
	TMap<AActor*, FIntVector2> myActorLocationMap;

	bool GetFreeMeshData(FCGJob& aJob);
	void ReleaseMeshData(uint8 aLOD, FCGMeshData* aDataToRelease);
	void AllocateAllMeshDataStructures();
	bool AllocateDataStructuresForLOD(FCGMeshData* aData, FCGTerrainConfig* aConfig, const uint8 aLOD);

	int GetLODForRange(const int32 aRange);

	void CreateTileRefreshJob(FCGJob aJob);

	void ProcessTilesForActor(const AActor* anActor);

	ACGTile* GetFreeTile();
	void FreeTile(ACGTile* aTile);

	FIntVector2 GetSector(const FVector& aLocation);
	TArray<FCGSector> GetRelevantSectorsForActor(const AActor* aActor);

public:
	ACGTerrainManager();
	~ACGTerrainManager();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CashGen")
	bool isReady = false;

	TQueue<FCGJob, EQueueMode::Mpsc> myUpdateJobQueue;
	
	UFUNCTION(BlueprintCallable, Category = "CashGen")
	void SetTerrainConfig(FCGTerrainConfig aTerrainConfig);

	void HandlePlayerSectorChange(const AActor* aActor, const FIntVector2& anOldSector, const FIntVector2& aNewSector);

	UFUNCTION(BlueprintCallable, Category = "CashGen")
	void AddPawn(AActor* aActor);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	void BeginDestroy() override;

};