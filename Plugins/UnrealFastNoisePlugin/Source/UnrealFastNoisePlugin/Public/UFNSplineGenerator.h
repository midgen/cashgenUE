#pragma once

#include "UFNNoiseGenerator.h"
#include "UFNSplineGenerator.generated.h"

UCLASS(BlueprintType)
class UUFNSplineGenerator : public UUFNNoiseGenerator
{
	GENERATED_UCLASS_BODY()
public:
	float GetNoise3D(float aX, float aY, float aZ) override;
	float GetNoise2D(float aX, float aY) override;

	UFUNCTION(BlueprintCallable, Category="FastNoise")
	void AddSpline(USplineComponent* Spline);

	UPROPERTY()
	TArray<class USplineComponent*> Splines;

	/*
	The generator will return values of 1.0 for all points greater than MaximumDistance
	from the splines
	*/
	float MaximumDistance;

	/*
	  The generator will return values of 0.0 for all points less than MinimumDistance
	  from the splines
	*/
	float MinimumDistance;

	/*
	  Values between Minimum and Maximum Distance will be run through this curve function,
	  if provided.
	*/
	UCurveFloat *FalloffCurve;
};
