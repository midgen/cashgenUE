
#include "UnrealFastNoisePlugin.h"
#include "UFNNoiseGenerator.h"
#include "UFNAddModule.h"

UUFNAddModule::UUFNAddModule(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	threshold = 0.0f;
}

float UUFNAddModule::GetNoise3D(float aX, float aY, float aZ)
{

	if (!(inputModule1 || inputModule2)) {
		return 0.0f;
	}

	float modifier = 1.0f;
	if (maskModule)
	{
		float mask = maskModule->GetNoise3D(aX, aY, aZ);
		if (mask >= threshold)
		{
			modifier = mask;
		}
		else {
			return inputModule1->GetNoise3D(aX, aY, aZ);
		}
	}


	return modifier * (inputModule1->GetNoise3D(aX, aY, aZ) + inputModule2->GetNoise3D(aX, aY, aZ));


}

float UUFNAddModule::GetNoise2D(float aX, float aY)
{
	return GetNoise3D(aX, aY, 0.0f);
}

