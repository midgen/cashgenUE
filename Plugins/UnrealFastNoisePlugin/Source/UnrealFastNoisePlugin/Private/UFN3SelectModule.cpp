
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

	if (interpType != ESelectInterpType::None)
	{
		// Outside falloff bounds
		if (control >= upperThreshold + falloff) {
			return inputModule1->GetNoise3D(aX, aY, aZ);
		}
		else if (control >= lowerThreshold + falloff && control < upperThreshold - falloff) {
			return inputModule2->GetNoise3D(aX, aY, aZ);
		}
		else if (control < lowerThreshold - falloff) {
			return inputModule3->GetNoise3D(aX, aY, aZ);
		}
		else if (control < upperThreshold + falloff && control >= upperThreshold - falloff) {
			return GetInterp3D(aX, aY, aZ, inputModule2, inputModule1, interpType, (control - (upperThreshold - falloff) / 2.0f * falloff));
		}
		else if (control < lowerThreshold + falloff && control >= lowerThreshold - falloff) {
			return GetInterp3D(aX, aY, aZ, inputModule3, inputModule2, interpType, (control - (lowerThreshold - falloff) / 2.0f * falloff));
		}
	}

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

	if (interpType != ESelectInterpType::None)
	{
		// Outside falloff bounds
		if (control >= upperThreshold + falloff) {
			return inputModule1->GetNoise2D(aX, aY);
		}
		else if (control >= lowerThreshold + falloff && control < upperThreshold - falloff) {
			return inputModule2->GetNoise2D(aX, aY);
		}
		else if (control < lowerThreshold - falloff) {
			return inputModule3->GetNoise2D(aX, aY);
		}
		else if (control < upperThreshold + falloff && control >= upperThreshold - falloff) {
			return GetInterp2D(aX, aY, inputModule2, inputModule1, interpType, (control - ((upperThreshold) - falloff) / 2.0f * falloff));
		}
		else if (control < lowerThreshold + falloff && control >= lowerThreshold - falloff) {
			return GetInterp2D(aX, aY, inputModule3, inputModule2, interpType, (control - ((lowerThreshold) - falloff) / 2.0f * falloff));
		}
	}

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

float UUFN3SelectModule::GetInterp2D(float aX, float aY, UUFNNoiseGenerator* input1, UUFNNoiseGenerator* input2, ESelectInterpType interpType, float alpha)
{
	switch (interpType)
	{
	case ESelectInterpType::CircularIn:
		return FMath::InterpCircularIn(input1->GetNoise2D(aX, aY), input2->GetNoise2D(aX, aY), alpha);
	case ESelectInterpType::CircularInOut:
		return FMath::InterpCircularInOut(input1->GetNoise2D(aX, aY), input2->GetNoise2D(aX, aY), alpha);
	case ESelectInterpType::CircularOut:
		return FMath::InterpCircularOut(input1->GetNoise2D(aX, aY), input2->GetNoise2D(aX, aY), alpha);
	case ESelectInterpType::ExponentialIn:
		return FMath::InterpExpoIn(input1->GetNoise2D(aX, aY), input2->GetNoise2D(aX, aY), alpha);
	case ESelectInterpType::ExponentialInOut:
		return FMath::InterpExpoInOut(input1->GetNoise2D(aX, aY), input2->GetNoise2D(aX, aY), alpha);
	case ESelectInterpType::ExponentialOut:
		return FMath::InterpExpoOut(input1->GetNoise2D(aX, aY), input2->GetNoise2D(aX, aY), alpha);
	case ESelectInterpType::SineIn:
		return FMath::InterpSinIn(input1->GetNoise2D(aX, aY), input2->GetNoise2D(aX, aY), alpha);
	default:
	case ESelectInterpType::SineInOut:
		return FMath::InterpSinInOut(input1->GetNoise2D(aX, aY), input2->GetNoise2D(aX, aY), alpha);
	case ESelectInterpType::SineOut:
		return FMath::InterpSinInOut(input1->GetNoise2D(aX, aY), input2->GetNoise2D(aX, aY), alpha);
	case ESelectInterpType::Step:
		return FMath::InterpStep(input1->GetNoise2D(aX, aY), input2->GetNoise2D(aX, aY), alpha, numSteps);
	}
}

float UUFN3SelectModule::GetInterp3D(float aX, float aY, float aZ, UUFNNoiseGenerator* input1, UUFNNoiseGenerator* input2, ESelectInterpType interpType, float alpha)
{
	switch (interpType)
	{
	case ESelectInterpType::CircularIn:
		return FMath::InterpCircularIn(input1->GetNoise3D(aX, aY, aZ), input2->GetNoise3D(aX, aY, aZ), alpha);
	case ESelectInterpType::CircularInOut:
		return FMath::InterpCircularInOut(input1->GetNoise3D(aX, aY, aZ), input2->GetNoise3D(aX, aY, aZ), alpha);
	case ESelectInterpType::CircularOut:
		return FMath::InterpCircularOut(input1->GetNoise3D(aX, aY, aZ), input2->GetNoise3D(aX, aY, aZ), alpha);
	case ESelectInterpType::ExponentialIn:
		return FMath::InterpExpoIn(input1->GetNoise3D(aX, aY, aZ), input2->GetNoise3D(aX, aY, aZ), alpha);
	case ESelectInterpType::ExponentialInOut:
		return FMath::InterpExpoInOut(input1->GetNoise3D(aX, aY, aZ), input2->GetNoise3D(aX, aY, aZ), alpha);
	case ESelectInterpType::ExponentialOut:
		return FMath::InterpExpoOut(input1->GetNoise3D(aX, aY, aZ), input2->GetNoise3D(aX, aY, aZ), alpha);
	case ESelectInterpType::SineIn:
		return FMath::InterpSinIn(input1->GetNoise3D(aX, aY, aZ), input2->GetNoise3D(aX, aY, aZ), alpha);
	default:
	case ESelectInterpType::SineInOut:
		return FMath::InterpSinInOut(input1->GetNoise3D(aX, aY, aZ), input2->GetNoise3D(aX, aY, aZ), alpha);
	case ESelectInterpType::SineOut:
		return FMath::InterpSinInOut(input1->GetNoise3D(aX, aY, aZ), input2->GetNoise3D(aX, aY, aZ), alpha);
	case ESelectInterpType::Step:
		return FMath::InterpStep(input1->GetNoise3D(aX, aY, aZ), input2->GetNoise3D(aX, aY, aZ), alpha, numSteps);
	}
}

