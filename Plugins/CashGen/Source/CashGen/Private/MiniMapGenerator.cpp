#include "cashgen.h"
#include "MiniMapGenerator.h"
#include "WorldManager.h"

UMiniMapGenerator::UMiniMapGenerator(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UMiniMapGenerator::Init(AWorldManager* aWorldManager, FZoneConfig aZoneConfig)
{
	worldManager = aWorldManager;
	zoneConfig = aZoneConfig;
	isWorkerCompleted = false;
}

void UMiniMapGenerator::UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData)
{
	if (Texture && Texture->Resource)
	{
		struct FUpdateTextureRegionsData
		{
			FTexture2DResource* Texture2DResource;
			int32 MipIndex;
			uint32 NumRegions;
			FUpdateTextureRegion2D* Regions;
			uint32 SrcPitch;
			uint32 SrcBpp;
			uint8* SrcData;
		};

		FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;

		RegionData->Texture2DResource = (FTexture2DResource*)Texture->Resource;
		RegionData->MipIndex = MipIndex;
		RegionData->NumRegions = NumRegions;
		RegionData->Regions = Regions;
		RegionData->SrcPitch = SrcPitch;
		RegionData->SrcBpp = SrcBpp;
		RegionData->SrcData = SrcData;

		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
			UpdateTextureRegionsData,
			FUpdateTextureRegionsData*, RegionData, RegionData,
			bool, bFreeData, bFreeData,
			{
				for (uint32 RegionIndex = 0; RegionIndex < RegionData->NumRegions; ++RegionIndex)
				{
					int32 CurrentFirstMip = RegionData->Texture2DResource->GetCurrentFirstMip();
					if (RegionData->MipIndex >= CurrentFirstMip)
					{
						RHIUpdateTexture2D(
							RegionData->Texture2DResource->GetTexture2DRHI(),
							RegionData->MipIndex - CurrentFirstMip,
							RegionData->Regions[RegionIndex],
							RegionData->SrcPitch,
							RegionData->SrcData
							+ RegionData->Regions[RegionIndex].SrcY * RegionData->SrcPitch
							+ RegionData->Regions[RegionIndex].SrcX * RegionData->SrcBpp
							);
					}
				}
		if (bFreeData)
		{
			FMemory::Free(RegionData->Regions);
			FMemory::Free(RegionData->SrcData);
		}
		delete RegionData;
			});
	}
}

void UMiniMapGenerator::CreateHeightmapTexture()
{
	heightmapUpdateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, WORLD_HEIGHTMAP_TEXTURE_SIZE, WORLD_HEIGHTMAP_TEXTURE_SIZE);
	
	// Create echo texture
	worldManager->MiniMapTexture = UTexture2D::CreateTransient(WORLD_HEIGHTMAP_TEXTURE_SIZE, WORLD_HEIGHTMAP_TEXTURE_SIZE);
	worldManager->MiniMapTexture->UpdateResource();

	// Initialize data
	colorData.Init(FColor(0, 0, 255, 255), WORLD_HEIGHTMAP_DATA_LENGTH);
	heightMapData.Init(0.0f, WORLD_HEIGHTMAP_DATA_LENGTH);

}

void UMiniMapGenerator::UpdateHeightmapTexture()
{
	UpdateTextureRegions(worldManager->MiniMapTexture, (int32)0, (uint32)1, heightmapUpdateTextureRegion, (uint32)(4 * WORLD_HEIGHTMAP_TEXTURE_SIZE), (uint32)4, (uint8*)colorData.GetData(), false);
}