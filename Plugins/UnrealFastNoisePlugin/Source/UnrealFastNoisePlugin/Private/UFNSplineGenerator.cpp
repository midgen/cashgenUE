#include "UnrealFastNoisePlugin.h"
#include "UFNNoiseGenerator.h"
#include "Classes/Components/SplineComponent.h"
#include "UFNSplineGenerator.h"

UUFNSplineGenerator::UUFNSplineGenerator(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

float UUFNSplineGenerator::GetNoise3D(float aX, float aY, float aZ)
{
	float LocalMinDistance = MaximumDistance;
	FVector WorldPoint = FVector(aX, aY, aZ);
	for (auto Spline : Splines)
	{
		// May need the InputKey later, so instead of finding location, find input key to prevent multiple searches
		float InputKey = Spline->FindInputKeyClosestToWorldLocation(WorldPoint);
		FVector PointOnSpline = Spline->GetLocationAtSplineInputKey(InputKey, ESplineCoordinateSpace::World);

		float Distance = FVector::Dist(FVector(aX, aY, 0.0f), PointOnSpline);

		// If we are inside a spline's width, distance from other splines is irrelevant so we can return early
		if (Distance < MinimumDistance)
		{
			return 0.0f;
		}
		else {
			LocalMinDistance = Distance < LocalMinDistance ? Distance : LocalMinDistance;
			/*
			This is not implemented

			Inside convex curves we have the problem that a point may be closer to the sides than to the points
			near the apex. This means there will be a seam down the centre rather than a soft gradient. We could
			alleviate this by ensuring we use a point on the curve that has a (roughly) orthogonal tangent to
			the vector between our world space point and the point on the tangent, but this doesn't seem to work.

			FVector TangentOnSpline = Spline->GetTangentAtSplineInputKey(InputKey, ESplineCoordinateSpace::World);
			TangentOnSpline.Normalize();
			FVector WorldPointToSplinePoint = PointOnSpline - WorldPoint;
			WorldPointToSplinePoint.Normalize();
			if (FVector::DotProduct(TangentOnSpline, WorldPointToSplinePoint) <= TangentApproximation)
			{
				LocalMinDistance = Distance < LocalMinDistance ? Distance : LocalMinDistance;
			}
			else {
				// Find a point on the curve with orthogonal tangent, and use distance to that instead.
				return 0.0f;
			}
			*/

		}
	}

	if (FalloffCurve)
	{
		return FalloffCurve->GetFloatValue((LocalMinDistance - MinimumDistance) / (MaximumDistance - MinimumDistance));
	}
	else {
		return (LocalMinDistance - MinimumDistance) / (MaximumDistance - MinimumDistance);
	}
}
float UUFNSplineGenerator::GetNoise2D(float aX, float aY)
{
	return GetNoise3D(aX, aY, 0.0f);
}

void UUFNSplineGenerator::AddSpline(USplineComponent* Spline)
{
	Splines.Add(Spline);
	//GLog->Log(FString::Printf(TEXT("Number of Splines: %d, ID %d"), Splines.Num(), GetUniqueID()));
}