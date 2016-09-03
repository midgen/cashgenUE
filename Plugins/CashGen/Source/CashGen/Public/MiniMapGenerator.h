#pragma once

#include "Object.h"
#include "ZoneConfig.h"
#include "MiniMapGenerator.generated.h"

class AWorldManager;

UCLASS(DefaultToInstanced)
class UMiniMapGenerator : public UObject
{
	GENERATED_UCLASS_BODY()

	FZoneConfig zoneConfig;

	void Init(AWorldManager* aWorldManager, FZoneConfig aZoneConfig);

	/** Update texture region from https://wiki.unrealengine.com/Dynamic_Textures */
	void UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData);

	void CreateHeightmapTexture();

	void UpdateHeightmapTexture();

	bool isWorkerCompleted;

private:
	UPROPERTY()
	TArray<FColor> colorData;
	UPROPERTY()
	TArray<float> heightMapData;

	FUpdateTextureRegion2D* heightmapUpdateTextureRegion;

	AWorldManager* worldManager;
};