

#include "UnrealLibNoise.h"
#include "../Noise.h"
#include "TranslatePoint.h"

/// Constructor.
///
/// The default translation amount to apply to the @a x coordinate is
/// set to noise::module::DEFAULT_TRANSLATE_POINT_X.
///
/// The default translation amount to apply to the @a y coordinate is
/// set to noise::module::DEFAULT_TRANSLATE_POINT_Y.
///
/// The default translation amount to apply to the @a z coordinate is
/// set to noise::module::DEFAULT_TRANSLATE_POINT_Z.
UTranslatePoint::UTranslatePoint(const FObjectInitializer& ObjectInit) : Super(ObjectInit)
{
	xTranslation = DEFAULT_TRANSLATE_POINT_X;
	yTranslation = DEFAULT_TRANSLATE_POINT_Y;
	zTranslation = DEFAULT_TRANSLATE_POINT_Z;
}

float UTranslatePoint::GetValue(FVector Coordinates)
{
	if (IsValid(GetSourceModule(0))) throw ExceptionNoModule();

	return GetSourceModule(0)->GetValue(FVector(Coordinates.X + xTranslation, Coordinates.Y + yTranslation, Coordinates.Z + zTranslation));
}
