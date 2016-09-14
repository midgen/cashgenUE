
#include "UnrealFastNoisePlugin.h"
#include "UFNNoiseGenerator.h"
#include "UFN3SelectModule.h"

UUFN3SelectModule::UUFN3SelectModule(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

float UUFN3SelectModule::GetNoise3D(float aX, float aY, float aZ)
{
	if (!(inputModule1 && inputModule2 && inputModule3 && selectModule)) {
		return 0.0f;
	}

	float control = (selectModule->GetNoise3D(aX, aY, aZ));


	// If there's no interpolation, easy mode
	if (control >= upperThreshold) {
		return inputModule1->GetNoise3D(aX, aY, aZ);
	}
	else if (control < upperThreshold && control >= lowerThreshold)
	{
		return inputModule2->GetNoise3D(aX, aY, aZ);
	}
	else {
		return inputModule3->GetNoise3D(aX, aY, aZ);
	}

}

float UUFN3SelectModule::GetNoise2D(float aX, float aY)
{
	if (!(inputModule1 && inputModule2 && inputModule3 && selectModule)) {
		return 0.0f;
	}

	float control = (selectModule->GetNoise2D(aX, aY));

	// If there's no interpolation, easy mode
	if (control >= upperThreshold) {
		return inputModule1->GetNoise2D(aX, aY);
	}
	else if (control < upperThreshold && control >= lowerThreshold)
	{
		return inputModule2->GetNoise2D(aX, aY);
	}
	else {
		return inputModule3->GetNoise2D(aX, aY);
	}


}

