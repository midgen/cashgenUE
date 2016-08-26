
#include "UnrealFastNoisePlugin.h"
#include "UFNNoiseGenerator.h"
#include "UFNSelectModule.h"

UUFNSelectModule::UUFNSelectModule(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

float UUFNSelectModule::GetNoise3D(float aX, float aY, float aZ)
{
	if (!(inputModule1 && inputModule2 && selectModule)) {
		return 0.0f;
	}

	float control = (selectModule->GetNoise3D(aX, aY, aZ));



	if (falloff > 0.00001f || falloff < -0.00001f)
	{
		// outside lower falloff bound
		if (control <= (threshold - falloff)) {
			return inputModule2->GetNoise3D(aX, aY, aZ);
		}
		// outside upper falloff bound
		else if (control >= (threshold + falloff)) {
			return inputModule1->GetNoise3D(aX, aY, aZ);
		}
		// otherwise must be inside the threshold bounds, so smooth it
		else {
			return FMath::InterpSinInOut(inputModule2->GetNoise3D(aX, aY, aZ), inputModule1->GetNoise3D(aX, aY, aZ), (control - (threshold - falloff) / (2.0f * falloff)));
		}
	}
	else {

		if (control > threshold) {
			return inputModule1->GetNoise3D(aX, aY, aZ);
		}
		else {
			return inputModule2->GetNoise3D(aX, aY, aZ);
		}
	}
}

float UUFNSelectModule::GetNoise2D(float aX, float aY)
{
	if (!(inputModule1 && inputModule2 && selectModule)) {
		return 0.0f;
	}

	float control = (selectModule->GetNoise2D(aX, aY));

	

	if (falloff > 0.00001f || falloff < -0.00001f)
	{
		// outside lower falloff bound
		if (control <= (threshold - falloff)) {
			return inputModule2->GetNoise2D(aX, aY);
		}
		// outside upper falloff bound
		else if (control >= (threshold + falloff)) {
			return inputModule1->GetNoise2D(aX, aY);
		} 
		// otherwise must be inside the threshold bounds, so smooth it
		else {
			return FMath::InterpSinInOut(inputModule2->GetNoise2D(aX, aY), inputModule1->GetNoise2D(aX, aY), (control - (threshold - falloff) / (2.0f * falloff)));
		}
	}
	else {

		if (control > threshold) {
			return inputModule1->GetNoise2D(aX, aY);
		}
		else {
			return inputModule2->GetNoise2D(aX, aY);
		}
	}

}

