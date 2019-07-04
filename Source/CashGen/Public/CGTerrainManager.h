#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Struct/CGJob.h"
#include "Struct/CGTerrainConfig.h"
#include "Struct/CGMeshData.h"
#include "Struct/CGTileHandle.h"
#include "Struct/IntVector2.h"
#include "Struct/CGSector.h"
#include "CGSettings.h"
#include "Struct/CGLODMeshData.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "CGTerrainManager.generated.h"

class ACGTile;

UCLASS(BlueprintType, Blueprintable)
class ACGTerrainManager : public AActor
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
	void SetupTerrainGenerator(UUFNNoiseGenerator* aHeightmapGenerator, UUFNNoiseGenerator* aBiomeGenerator/*FCGTerrainConfig aTerrainConfig*/);

	/* Add a new actor to track and generate terrain tiles around */
	UFUNCTION(BlueprintCallable, Category = "CashGen")
	void AddActorToTrack(AActor* aActor);

	/* Add a new actor to track and generate terrain tiles around */
	UFUNCTION(BlueprintCallable, Category = "CashGen")
	void RemoveActorToTrack(AActor* aActor);

	// Update queue, jobs get sent here from the worker thread
	TQueue<FCGJob, EQueueMode::Mpsc> myUpdateJobQueue;

	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	void BeginDestroy() override;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UHierarchicalInstancedStaticMeshComponent* MyWaterMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CashGen")
	FCGTerrainConfig myTerrainConfig;

protected:
	void BroadcastTerrainComplete()
	{
		TerrainCompleteEvent.Broadcast();
	}

private:

	void SetActorSector(const AActor* aActor, const FIntVector2& aNewSector);

	// Master config



	// Sweep tracking
	float myTimeSinceLastSweep = 0.0f;
	const float mySweepTime = 2.0f;
	uint8 myActorIndex = 0;

	// Threads
	TArray<FRunnableThread*> myWorkerThreads;

	// Geometry data storage
	UPROPERTY()
	TArray<FCGLODMeshData>		myMeshData;
	TArray<TSet<FCGMeshData*>>	myFreeMeshData;

	// Job tracking
	TQueue<FCGJob, EQueueMode::Spsc>			myPendingJobQueue;
	TArray<TQueue<FCGJob, EQueueMode::Spsc>>	myGeometryJobQueues;

	// Tile/Sector tracking
	TArray<ACGTile*>	myFreeTiles;
	TArray<int32>		myFreeWaterMeshIndices;
	UPROPERTY()
	TMap<FIntVector2, FCGTileHandle> myTileHandleMap;
	TSet<FIntVector2> myQueuedSectors;

	// Actor tracking
	TArray<AActor*> myTrackedActors;
	TMap<AActor*, FIntVector2> myActorLocationMap;

	bool myIsTerrainComplete = false;

	bool GetFreeMeshData(FCGJob& aJob);
	void ReleaseMeshData(uint8 aLOD, FCGMeshData* aDataToRelease);
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




};
