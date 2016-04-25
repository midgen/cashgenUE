

#include "UnrealLibNoise.h"
#include "RotatePoint.h"

/// Constructor.
///
/// The default rotation angle around the @a x axis, in degrees, is
/// set to noise::module::DEFAULT_ROTATE_X.
///
/// The default rotation angle around the @a y axis, in degrees, is
/// set to noise::module::DEFAULT_ROTATE_Y.
///
/// The default rotation angle around the @a z axis, in degrees, is
/// set to noise::module::DEFAULT_ROTATE_Z.
URotatePoint::URotatePoint(const FObjectInitializer& ObjectInit) : Super(ObjectInit)
{
	SetAngles(DEFAULT_ROTATE_X, DEFAULT_ROTATE_Y, DEFAULT_ROTATE_Z);
}

float URotatePoint::GetValue(FVector Coordinates)
{
	if (GetSourceModule(0) == NULL) throw ExceptionNoModule();
	
	float nx = (x1Matrix * Coordinates.X) + (y1Matrix * Coordinates.Y) + (z1Matrix * Coordinates.Z);
	float ny = (x2Matrix * Coordinates.X) + (y2Matrix * Coordinates.Y) + (z2Matrix * Coordinates.Z);
	float nz = (x3Matrix * Coordinates.X) + (y3Matrix * Coordinates.Y) + (z3Matrix * Coordinates.Z);
	return GetSourceModule(0)->GetValue(FVector(nx, ny, nz));
}

void URotatePoint::SetAngles(float xAngle, float yAngle,	float zAngle)
{
	float xCos, yCos, zCos, xSin, ySin, zSin;
	xCos = FMath::DegreesToRadians(FMath::Cos(xAngle));
	yCos = FMath::DegreesToRadians(FMath::Cos(yAngle));
	zCos = FMath::DegreesToRadians(FMath::Cos(zAngle));
	xSin = FMath::DegreesToRadians(FMath::Sin(xAngle));
	ySin = FMath::DegreesToRadians(FMath::Sin(yAngle));
	zSin = FMath::DegreesToRadians(FMath::Sin(zAngle));

	x1Matrix = ySin * xSin * zSin + yCos * zCos;
	y1Matrix = xCos * zSin;
	z1Matrix = ySin * zCos - yCos * xSin * zSin;
	x2Matrix = ySin * xSin * zCos - yCos * zSin;
	y2Matrix = xCos * zCos;
	z2Matrix = -yCos * xSin * zCos - ySin * zSin;
	x3Matrix = -ySin * xCos;
	y3Matrix = xSin;
	z3Matrix = yCos * xCos;

	xAngle = xAngle;
	yAngle = yAngle;
	zAngle = zAngle;
}



