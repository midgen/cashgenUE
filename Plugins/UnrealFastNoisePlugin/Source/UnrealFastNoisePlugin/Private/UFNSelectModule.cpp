
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



	if (interpType != ESelectInterpType::None)
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

			switch (interpType)
			{
			case ESelectInterpType::CircularIn:
				return FMath::InterpCircularIn(inputModule2->GetNoise3D(aX, aY, aZ), inputModule1->GetNoise3D(aX, aY, aZ), (control - (threshold - falloff) / (2.0f * falloff)));
			case ESelectInterpType::CircularInOut:
				return FMath::InterpCircularInOut(inputModule2->GetNoise3D(aX, aY, aZ), inputModule1->GetNoise3D(aX, aY, aZ), (control - (threshold - falloff) / (2.0f * falloff)));
			case ESelectInterpType::CircularOut:
				return FMath::InterpCircularOut(inputModule2->GetNoise3D(aX, aY, aZ), inputModule1->GetNoise3D(aX, aY, aZ), (control - (threshold - falloff) / (2.0f * falloff)));
			case ESelectInterpType::ExponentialIn:
				return FMath::InterpExpoIn(inputModule2->GetNoise3D(aX, aY, aZ), inputModule1->GetNoise3D(aX, aY, aZ), (control - (threshold - falloff) / (2.0f * falloff)));
			case ESelectInterpType::ExponentialInOut:
				return FMath::InterpExpoInOut(inputModule2->GetNoise3D(aX, aY, aZ), inputModule1->GetNoise3D(aX, aY, aZ), (control - (threshold - falloff) / (2.0f * falloff)));
			case ESelectInterpType::ExponentialOut:
				return FMath::InterpExpoOut(inputModule2->GetNoise3D(aX, aY, aZ), inputModule1->GetNoise3D(aX, aY, aZ), (control - (threshold - falloff) / (2.0f * falloff)));
			case ESelectInterpType::SineIn:
				return FMath::InterpSinIn(inputModule2->GetNoise3D(aX, aY, aZ), inputModule1->GetNoise3D(aX, aY, aZ), (control - (threshold - falloff) / (2.0f * falloff)));
			case ESelectInterpType::SineInOut:
				return FMath::InterpSinInOut(inputModule2->GetNoise3D(aX, aY, aZ), inputModule1->GetNoise3D(aX, aY, aZ), (control - (threshold - falloff) / (2.0f * falloff)));
			case ESelectInterpType::SineOut:
				return FMath::InterpSinInOut(inputModule2->GetNoise3D(aX, aY, aZ), inputModule1->GetNoise3D(aX, aY, aZ), (control - (threshold - falloff) / (2.0f * falloff)));
			case ESelectInterpType::Step:
				return FMath::InterpStep(inputModule2->GetNoise3D(aX, aY, aZ), inputModule1->GetNoise3D(aX, aY, aZ), (control - (threshold - falloff) / (2.0f * falloff)), numSteps);
			}
		}
	}
	
	// If there's no interpolation, easy mode
	if (control > threshold) {
		return inputModule1->GetNoise3D(aX, aY, aZ);
	}
	else {
		return inputModule2->GetNoise3D(aX, aY, aZ);
	}
	
}

float UUFNSelectModule::GetNoise2D(float aX, float aY)
{
	if (!(inputModule1 && inputModule2 && selectModule)) {
		return 0.0f;
	}

	float control = (selectModule->GetNoise2D(aX, aY));

	

	if (interpType != ESelectInterpType::None)
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
			switch (interpType)
			{
			case ESelectInterpType::CircularIn:
				return FMath::InterpCircularIn(inputModule2->GetNoise2D(aX, aY), inputModule1->GetNoise2D(aX, aY), (control - (threshold - falloff) / (2.0f * falloff)));
			case ESelectInterpType::CircularInOut:
				return FMath::InterpCircularInOut(inputModule2->GetNoise2D(aX, aY), inputModule1->GetNoise2D(aX, aY), (control - (threshold - falloff) / (2.0f * falloff)));
			case ESelectInterpType::CircularOut:
				return FMath::InterpCircularOut(inputModule2->GetNoise2D(aX, aY), inputModule1->GetNoise2D(aX, aY), (control - (threshold - falloff) / (2.0f * falloff)));
			case ESelectInterpType::ExponentialIn:
				return FMath::InterpExpoIn(inputModule2->GetNoise2D(aX, aY), inputModule1->GetNoise2D(aX, aY), (control - (threshold - falloff) / (2.0f * falloff)));
			case ESelectInterpType::ExponentialInOut:
				return FMath::InterpExpoInOut(inputModule2->GetNoise2D(aX, aY), inputModule1->GetNoise2D(aX, aY), (control - (threshold - falloff) / (2.0f * falloff)));
			case ESelectInterpType::ExponentialOut:
				return FMath::InterpExpoOut(inputModule2->GetNoise2D(aX, aY), inputModule1->GetNoise2D(aX, aY), (control - (threshold - falloff) / (2.0f * falloff)));
			case ESelectInterpType::SineIn:
				return FMath::InterpSinIn(inputModule2->GetNoise2D(aX, aY), inputModule1->GetNoise2D(aX, aY), (control - (threshold - falloff) / (2.0f * falloff)));
			case ESelectInterpType::SineInOut:
				return FMath::InterpSinInOut(inputModule2->GetNoise2D(aX, aY), inputModule1->GetNoise2D(aX, aY), (control - (threshold - falloff) / (2.0f * falloff)));
			case ESelectInterpType::SineOut:
				return FMath::InterpSinInOut(inputModule2->GetNoise2D(aX, aY), inputModule1->GetNoise2D(aX, aY), (control - (threshold - falloff) / (2.0f * falloff)));
			case ESelectInterpType::Step:
				return FMath::InterpStep(inputModule2->GetNoise2D(aX, aY), inputModule1->GetNoise2D(aX, aY), (control - (threshold - falloff) / (2.0f * falloff)), numSteps);
			}
		}
	}
	

	if (control > threshold) {
		return inputModule1->GetNoise2D(aX, aY);
	}
	else {
		return inputModule2->GetNoise2D(aX, aY);
	}
	

}

