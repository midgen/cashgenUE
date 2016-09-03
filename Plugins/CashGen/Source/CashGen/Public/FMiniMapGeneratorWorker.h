#pragma once
#include "cashgen.h"
#include "ZoneManager.h"
#include "BiomeWeights.h"
#include "ZoneConfig.h"

class FMiniMapGeneratorWorker : public FRunnable
{
	AZoneManager* pCallingZoneManager;
	FZoneConfig*  pZoneConfig;
	CGPoint*		  pOffset;

	TArray<float>*	pHeightMapData;
	TArray<FColor>*		pColorData;

	void GenerateMiniMapData();

public:
	FMiniMapGeneratorWorker(AZoneManager*		apZoneManager,
		FZoneConfig*			aZoneConfig,
		CGPoint*				aOffSet,
		TArray<float>* aHeightMapData,
		TArray<FColor>* aColorData);

	virtual ~FMiniMapGeneratorWorker();

	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	virtual void Exit();


};