#pragma once

#include "CashGen/Public/CGMcQueue.h"
#include "CashGen/Public/CGObjectPool.h"
#include "CashGen/Public/CGSettings.h"
#include "CashGen/Public/Struct/CGJob.h"
#include "CashGen/Public/Struct/CGLODMeshData.h"
#include "CashGen/Public/Struct/CGMeshData.h"
#include "CashGen/Public/Struct/CGSector.h"
#include "CashGen/Public/Struct/CGTerrainConfig.h"
#include "CashGen/Public/Struct/CGTileHandle.h"
#include "CashGen/Public/Struct/IntVector2.h"

#include <Runtime/Engine/Classes/Components/HierarchicalInstancedStaticMeshComponent.h>
#include <Runtime/Engine/Classes/GameFramework/Actor.h>

#include "CGTerrainManager.generated.h"

class ACGTile;

UCLASS(BlueprintType, Blueprintable)
class CASHGEN_API ACGTerrainManager : public AActor
{
	GENERATED_BODY()

public:
	ACGTerrainManager();
	~ACGTerrainManager();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCGSettings* Settings = GetMutableDefault<UCGSettings>();

	/* Event called when initial terrain generation is complete */
	DECLARE_EVENT(ACGTerrainManager, FTerrainCompleteEvent)
	FTerrainCompleteEvent& OnTerrainComplete() { return TerrainCompleteEvent; }

	/* Returns true once terrain has been configured */
	bool isReady = false;

	/* Main entry point for starting terrain generation */
	UFUNCTION(BlueprintCallable, Category = "CashGen")
	void SetupTerrainGenerator(UUFNNoiseGenerator* aHeightmapGenerator, UUFNNoiseGenerator* aBiomeGenerator /*FCGTerrainConfig aTerrainConfig*/);

	/* Add a new actor to track and generate terrain tiles around */
	UFUNCTION(BlueprintCallable, Category = "CashGen")
	void AddActorToTrack(AActor* aActor);

	/* Add a new actor to track and generate terrain tiles around */
	UFUNCTION(BlueprintCallable, Category = "CashGen")
	void RemoveActorToTrack(AActor* aActor);

	// Pending job queue, worker threads take jobs from here
	TCGSpmcQueue<FCGJob> myPendingJobQueue;

	// Update queue, jobs get sent here from the worker thread
	TQueue<FCGJob, EQueueMode::Mpsc> myUpdateJobQueue;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	void BeginDestroy() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UHierarchicalInstancedStaticMeshComponent* MyWaterMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CashGen")
	FCGTerrainConfig myTerrainConfig;

	UFUNCTION(BlueprintImplementableEvent, Category = "CashGen|Events")
	void OnAfterTileCreated(ACGTile* tile);

protected:
	void BroadcastTerrainComplete()
	{
		TerrainCompleteEvent.Broadcast();
	}

private:
	void SetActorSector(const AActor* aActor, const FIntVector2& aNewSector);
	void AllocateAllMeshDataStructures();
	bool AllocateDataStructuresForLOD(FCGMeshData* aData, FCGTerrainConfig* aConfig, const uint8 aLOD);
	int GetLODForRange(const int32 aRange);
	void CreateTileRefreshJob(FCGJob aJob);
	void ProcessTilesForActor(const AActor* anActor);
	TPair<ACGTile*, int32> GetAvailableTile();
	void FreeTile(ACGTile* aTile, const int32& aWaterMeshIndex);
	FIntVector2 GetSector(const FVector& aLocation);
	TArray<FCGSector> GetRelevantSectorsForActor(const AActor* aActor);

	FTerrainCompleteEvent TerrainCompleteEvent;

	// Threads
	TArray<FRunnableThread*> myWorkerThreads;

	// Geometry data storage
	UPROPERTY()
	TArray<FCGLODMeshData> myMeshData;
	TArray<TCGObjectPool<FCGMeshData>> myFreeMeshData;

	// Tile/Sector tracking
	TArray<ACGTile*> myFreeTiles;
	TArray<int32> myFreeWaterMeshIndices;
	UPROPERTY()
	TMap<FIntVector2, FCGTileHandle> myTileHandleMap;
	TSet<FIntVector2> myQueuedSectors;

	// Actor tracking
	TArray<AActor*> myTrackedActors;
	TMap<AActor*, FIntVector2> myActorLocationMap;

		// Sweep tracking
	float myTimeSinceLastSweep = 0.0f;
	const float mySweepTime = 2.0f;
	uint8 myActorIndex = 0;

	bool myIsTerrainComplete = false;
};