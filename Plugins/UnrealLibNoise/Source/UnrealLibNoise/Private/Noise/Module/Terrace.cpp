

#include "UnrealLibNoise.h"
#include "../Noise.h"
#include "Terrace.h"

UTerrace::UTerrace(const FObjectInitializer& ObjectInit) : Super(ObjectInit),
	ControlPointCount(0),
	ShouldInvertTerraces(false)
{
}

void UTerrace::AddControlPoint(float value)
{
	// Find the insertion point for the new control point and insert the new
	// point at that position.  The control point array will remain sorted by
	// value.
	int insertionPos = FindInsertionPos(value);
	InsertAtPos(insertionPos, value);
}

void UTerrace::ClearAllControlPoints()
{
	ControlPoints.Empty();
	ControlPointCount = 0;
}

int UTerrace::FindInsertionPos(float value)
{
	int insertionPos;
	for (insertionPos = 0; insertionPos < ControlPointCount; insertionPos++) {
		if (value < ControlPoints[insertionPos]) {
			// We found the array index in which to insert the new control point.
			// Exit now.
			break;
		}
		else if (value == ControlPoints[insertionPos]) {
			// Each control point is required to contain a unique value, so throw
			// an exception.
			throw ExceptionInvalidParam();
		}
	}
	return insertionPos;
}

float UTerrace::GetValue(FVector Coordinates)
{
	if(IsValid(GetSourceModule(0))) throw ExceptionNoModule();
	if(ControlPointCount < 2) throw ExceptionNoModule();

	// Get the output value from the source module.
	float sourceModuleValue = GetSourceModule(0)->GetValue(Coordinates);

	// Find the first element in the control point array that has a value
	// larger than the output value from the source module.
	int indexPos;
	for (indexPos = 0; indexPos < ControlPointCount; indexPos++) {
		if (sourceModuleValue < ControlPoints[indexPos]) {
			break;
		}
	}

	// Find the two nearest control points so that we can map their values
	// onto a quadratic curve.
	int index0 = FMath::Clamp(indexPos - 1, 0, ControlPointCount - 1);
	int index1 = FMath::Clamp(indexPos, 0, ControlPointCount - 1);

	// If some control points are missing (which occurs if the output value from
	// the source module is greater than the largest value or less than the
	// smallest value of the control point array), get the value of the nearest
	// control point and exit now.
	if (index0 == index1) {
		return ControlPoints[index1];
	}

	// Compute the alpha value used for linear interpolation.
	float value0 = ControlPoints[index0];
	float value1 = ControlPoints[index1];
	float alpha = (sourceModuleValue - value0) / (value1 - value0);
	if (ShouldInvertTerraces) {
		alpha = 1.0 - alpha;
		float tempValue = value0;
		value0 = value1;
		value1 = tempValue;
	}

	// Squaring the alpha produces the terrace effect.
	alpha *= alpha;

	// Now perform the linear interpolation given the alpha value.
	return UNoiseInterp::LinearInterp(value0, value1, alpha);
}

void UTerrace::InsertAtPos(int insertionPos, float value)
{
	// Make room for the new control point at the specified position within
	// the control point array.  The position is determined by the value of
	// the control point; the control points must be sorted by value within
	// that array.
	float* newControlPoints = new float[ControlPointCount + 1];
	for (int i = 0; i < ControlPointCount; i++) {
		if (i < insertionPos) {
			newControlPoints[i] = ControlPoints[i];
		}
		else {
			newControlPoints[i + 1] = ControlPoints[i];
		}
	}
	ControlPoints.Empty();
	++ControlPointCount;

	// Now that we've made room for the new control point within the array,
	// add the new control point.
	ControlPoints[insertionPos] = value;
}

void UTerrace::MakeControlPoints(int controlPointCount)
{
	if (controlPointCount < 2) {
		throw ExceptionInvalidParam();
	}

	ClearAllControlPoints();

	float terraceStep = 2.0 / ((float)controlPointCount - 1.0);
	float curValue = -1.0;
	for (int i = 0; i < (int)controlPointCount; i++) {
		AddControlPoint(curValue);
		curValue += terraceStep;
	}
}


