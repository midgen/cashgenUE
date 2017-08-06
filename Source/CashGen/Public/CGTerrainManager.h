#pragma once
#include "GameFramework/Actor.h"
#include "Struct/CGJob.h"
#include "Struct/CGTerrainConfig.h"
#include "Struct/CGMeshData.h"
#include "Struct/CGTileHandle.h"
#include "Struct/IntVector2.h"
#include "Struct/CGLODMeshData.h"
#include "CGTerrainManager.generated.h"

class ACGTile;

UCLASS(BlueprintType, Blueprintable)
class ACGTerrainManager : public AActor
{
	GENERATED_BODY()

	TArray<AActor*> myTrackedActors;

	TArray<FRunnableThread*> myWorkerThreads;

	UPROPERTY()
	TArray<FCGLODMeshData> myMeshData;
	TArray<TSet<FCGMeshData*>> myFreeMeshData;
	TArray<TSet<FCGMeshData*>> myInUseMeshData;

	TQueue<FCGJob, EQueueMode::Spsc> myPendingJobQueue;
	TArray<TQueue<FCGJob, EQueueMode::Spsc>> myGeometryJobQueues;

	TSet<ACGTile*> QueuedTiles;

	UPROPERTY()
	TMap<FIntVector2, FCGTileHandle> myTileHandles;


	bool GetFreeMeshData(FCGJob& aJob);
	void ReleaseMeshData(uint8 aLOD, FCGMeshData* aDataToRelease);
	void AllocateAllMeshDataStructures();
	bool AllocateDataStructuresForLOD(FCGMeshData* aData, FCGTerrainConfig* aConfig, const uint8 aLOD);
	void CreateTileRefreshJob(FCGJob aJob);

	UPROPERTY()
	FCGTerrainConfig myTerrainConfig;


	TMap<AActor*, FIntVector> myPawnLocationMap;

	FIntVector2 GetSector(const FVector& aLocation);
	TArray<FIntVector2> GetRelevantSectorsForActor(const AActor* anActor);


public:
	ACGTerrainManager();
	~ACGTerrainManager();

	TQueue<FCGJob, EQueueMode::Mpsc> myUpdateJobQueue;
	
	void SetTerrainConfig(FCGTerrainConfig aTerrainConfig);

	void HandlePlayerSectorChange(const uint8 aPlayerID, const FIntVector2& anOldSector, const FIntVector2& aNewSector);

	void AddPawn();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	void BeginDestroy() override;
};