

#include "UnrealLibNoise.h"
#include "../Noise.h"
#include "Curve.h"

UCurve::UCurve(const FObjectInitializer& ObjectInit) : Super(ObjectInit)
{	
	ControlPointCount = 0;	
}


void UCurve::AddControlPoint(float inputValue, float outputValue)
{
	// Find the insertion point for the new control point and insert the new
	// point at that position.  The control point array will remain sorted by
	// input value.
	int insertionPos = FindInsertionPos(inputValue);
	InsertAtPos(insertionPos, inputValue, outputValue);
}

void UCurve::ClearAllControlPoints()
{
	ControlPoints.Empty();
	ControlPointCount = 0;
}

int32 UCurve::FindInsertionPos(float inputValue)
{
	int insertionPos;
	for (insertionPos = 0; insertionPos < ControlPointCount; insertionPos++) {
		if (inputValue < ControlPoints[insertionPos].inputValue) {
			// We found the array index in which to insert the new control point.
			// Exit now.
			break;
		}
		else if (inputValue == ControlPoints[insertionPos].inputValue) {
			// Each control point is required to contain a unique input value, so
			// throw an exception.
			throw ExceptionInvalidParam();
		}
	}
	return insertionPos;
}

float UCurve::GetValue(FVector Coordinates)
{
	if (GetSourceModule(0) == NULL || ControlPointCount < 4)
	{
		throw new Exception;
	}
	
	// Get the output value from the source module.
	float sourceModuleValue = GetSourceModule(0)->GetValue(Coordinates);

	// Find the first element in the control point array that has an input value
	// larger than the output value from the source module.
	int indexPos;
	for (indexPos = 0; indexPos < ControlPointCount; indexPos++) {
		if (sourceModuleValue < ControlPoints[indexPos].inputValue) {
			break;
		}
	}

	// Find the four nearest control points so that we can perform cubic
	// interpolation.
	int index0 = FMath::Clamp(indexPos - 2, 0, ControlPointCount - 1);
	int index1 = FMath::Clamp(indexPos - 1, 0, ControlPointCount - 1);
	int index2 = FMath::Clamp(indexPos, 0, ControlPointCount - 1);
	int index3 = FMath::Clamp(indexPos + 1, 0, ControlPointCount - 1);

	// If some control points are missing (which occurs if the value from the
	// source module is greater than the largest input value or less than the
	// smallest input value of the control point array), get the corresponding
	// output value of the nearest control point and exit now.
	if (index1 == index2) {
		return ControlPoints[index1].outputValue;
	}

	// Compute the alpha value used for cubic interpolation.
	double input0 = ControlPoints[index1].inputValue;
	double input1 = ControlPoints[index2].inputValue;
	double alpha = (sourceModuleValue - input0) / (input1 - input0);

	// Now perform the cubic interpolation given the alpha value.
	return UNoiseInterp::CubicInterp(
		ControlPoints[index0].outputValue,
		ControlPoints[index1].outputValue,
		ControlPoints[index2].outputValue,
		ControlPoints[index3].outputValue,
		alpha);
}

void UCurve::InsertAtPos(int32 insertionPos, float inputValue, float outputValue)
{
	// Make room for the new control point at the specified position within the
	// control point array.  The position is determined by the input value of
	// the control point; the control points must be sorted by input value
	// within that array.
	TArray<FControlPoint> newControlPoints;
	for (int i = 0; i < ControlPointCount; i++) {
		if (i < insertionPos) {
			newControlPoints[i] = ControlPoints[i];
		}
		else {
			newControlPoints[i + 1] = ControlPoints[i];
		}
	}
	
	ControlPoints.Empty();
	ControlPoints = newControlPoints;
	++ControlPointCount;

	// Now that we've made room for the new control point within the array, add
	// the new control point.
	ControlPoints[insertionPos].inputValue = inputValue;
	ControlPoints[insertionPos].outputValue = outputValue;
}
