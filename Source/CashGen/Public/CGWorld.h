#pragma once
#include "cashgen.h"
#include "CGWorldConfig.h"
#include "CGWorldMeshData.h"
#include "CGWorld.generated.h"


class URuntimeMeshComponent;
class ACGWorldFace;
struct FCGWorldFaceJob;

UCLASS()
class ACGWorld : public AActor
{
	GENERATED_BODY()

	USphereComponent* SphereComponent;

	TArray<FRuntimeMeshVertexSimple> MyVertices;
	TArray<int32> MyIndices;

	TArray<ACGWorldFace*> MyFaces;
	UPROPERTY()
	TArray<FCGWorldMeshData> MyMeshData;
	TSet<FCGWorldMeshData*>  MyFreeMeshData;
	TSet<FCGWorldMeshData*> MyInUseMeshData;

	FRunnableThread* MyWorkerThread;

	float TimeSinceLastSweep;
	const float SweepInterval = 0.1f;

	bool isSetup = false;
	bool isFirstUpdate = true;

	bool GetFreeMeshData(FCGWorldFaceJob& aJob);

	void ReleaseMeshData(FCGWorldFaceJob& aJob);

public:

	UFUNCTION(BlueprintImplementableEvent, Category = "CGWorld")
	void OnDrawComplete();

	ACGWorld(const FObjectInitializer& ObjectInitializer);
	~ACGWorld();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CGWorld")
	FCGWorldConfig WorldConfig;

	TQueue<FCGWorldFaceJob, EQueueMode::Spsc> PendingJobs;
	TQueue<FCGWorldFaceJob, EQueueMode::Spsc> GeometryJobs;
	TQueue<FCGWorldFaceJob, EQueueMode::Mpsc> UpdateJobs;

	UFUNCTION(BlueprintCallable, Category = "CashGen")
	void SetupWorld(UUFNNoiseGenerator* aNoiseGen);

	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
	void AddFace(ACGWorldFace* aFace);
	void InitializeSphere(const int32 aDepth, const float aScale);
	void BeginDestroy() override;
};