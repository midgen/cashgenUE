// Original Code Copyright (C) 2003, 2004 Jason Bevins
// Unreal Adaptation (C) 2016 Matthew Hamlin
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License (COPYING.txt) for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Original Developers Email jlbevins@gmail.com
// Email: DarkTreePrd@gmail.com
//



#include "UnrealLibNoise.h"
#include "NoiseInterp.h"
#include "RandomVectorTable.h"
#include "NoiseGen.h"

float UNoiseGen::GradientCoherentNoise3D(FVector Coordinates, int32 Seed, FNoiseQuality NoiseQuality)
{
	// Create a unit-length cube aligned along an integer boundary.  This cube
	// surrounds the input point.
	int x0 = (Coordinates.X > 0.0 ? (int)Coordinates.X : (int)Coordinates.X - 1);
	int x1 = x0 + 1;
	int y0 = (Coordinates.Y > 0.0 ? (int)Coordinates.Y : (int)Coordinates.Y - 1);
	int y1 = y0 + 1;
	int z0 = (Coordinates.Z > 0.0 ? (int)Coordinates.Z : (int)Coordinates.Z - 1);
	int z1 = z0 + 1;

	float xs = 0, ys = 0, zs = 0;

	switch (NoiseQuality)
	{
	case FNoiseQuality::QUALITY_FAST:
		xs = (Coordinates.X - (float)x0);
		ys = (Coordinates.Y - (float)y0);
		zs = (Coordinates.Z - (float)z0);
		break;
	case FNoiseQuality::QUALITY_STD:
		xs = UNoiseInterp::SCurve3(Coordinates.X - (float)x0);
		ys = UNoiseInterp::SCurve3(Coordinates.Y - (float)y0);
		zs = UNoiseInterp::SCurve3(Coordinates.Z - (float)z0);
		break;
	case FNoiseQuality::QUALITY_BEST:
		xs = UNoiseInterp::SCurve5(Coordinates.X - (float)x0);
		ys = UNoiseInterp::SCurve5(Coordinates.Y - (float)y0);
		zs = UNoiseInterp::SCurve5(Coordinates.Z - (float)z0);
		break;
	}

	// Now calculate the noise values at each vertex of the cube.  To generate
	// the coherent-noise value at the input point, interpolate these eight
	// noise values using the S-curve value as the interpolant (trilinear
	// interpolation.)
	float n0, n1, ix0, ix1, iy0, iy1;
	n0 = GradientNoise3D(Coordinates, x0, y0, z0, Seed);
	n1 = GradientNoise3D(Coordinates, x1, y0, z0, Seed);
	ix0 = UNoiseInterp::LinearInterp(n0, n1, xs);
	n0 = GradientNoise3D(Coordinates, x0, y1, z0, Seed);
	n1 = GradientNoise3D(Coordinates, x1, y1, z0, Seed);
	ix1 = UNoiseInterp::LinearInterp(n0, n1, xs);
	iy0 = UNoiseInterp::LinearInterp(ix0, ix1, ys);
	n0 = GradientNoise3D(Coordinates, x0, y0, z1, Seed);
	n1 = GradientNoise3D(Coordinates, x1, y0, z1, Seed);
	ix0 = UNoiseInterp::LinearInterp(n0, n1, xs);
	n0 = GradientNoise3D(Coordinates, x0, y1, z1, Seed);
	n1 = GradientNoise3D(Coordinates, x1, y1, z1, Seed);
	ix1 = UNoiseInterp::LinearInterp(n0, n1, xs);
	iy1 = UNoiseInterp::LinearInterp(ix0, ix1, ys);

	return UNoiseInterp::LinearInterp(iy0, iy1, zs);
}


float UNoiseGen::GradientNoise3D(FVector Coordinates, int32 NearX, int32 NearY, int32 NearZ, int32 Seed)
{
	// Randomly generate a gradient vector given the integer coordinates of the
	// input value.  This implementation generates a random number and uses it
	// as an index into a normalized-vector lookup table.
	int32 vectorIndex = (X_NOISE_GEN * NearX + Y_NOISE_GEN * NearY + Z_NOISE_GEN * NearZ + SEED_NOISE_GEN * Seed) & 0xffffffff;
	vectorIndex ^= (vectorIndex >> SHIFT_NOISE_GEN);
	vectorIndex &= 0xff;

	float xvGradient = g_randomVectors[(vectorIndex << 2)];
	float yvGradient = g_randomVectors[(vectorIndex << 2) + 1];
	float zvGradient = g_randomVectors[(vectorIndex << 2) + 2];
	
	//FRandomStream tempStream;
	//tempStream.Initialize(Seed);

	//float xvGradient = tempStream.FRandRange(-1.0, 1.0);
	//float yvGradient = tempStream.FRandRange(-1.0, 1.0);
	//float zvGradient = tempStream.FRandRange(-1.0, 1.0);

	// Set up us another vector equal to the distance between the two vectors
	// passed to this function.
	float xvPoint = (Coordinates.X - (float)NearX);
	float yvPoint = (Coordinates.Y - (float)NearY);
	float zvPoint = (Coordinates.Z - (float)NearZ);

	// Now compute the dot product of the gradient vector with the distance
	// vector.  The resulting value is gradient noise.  Apply a scaling value
	// so that this noise value ranges from -1.0 to 1.0.
	return ((xvGradient * xvPoint) + (yvGradient * yvPoint) + (zvGradient * zvPoint)) * 2.12;
}

int UNoiseGen::IntValueNoise3D(int32 X, int32 Y, int32 Z, int32 Seed)
{
	// All constants are primes and must remain prime in order for this noise
	// function to work correctly.
	int n = (X_NOISE_GEN * X + Y_NOISE_GEN * Y + Z_NOISE_GEN * Z + SEED_NOISE_GEN * Seed) & 0x7fffffff;
	n = (n >> 13) ^ n;
	return (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
}

float UNoiseGen::MakeInt32Range(float Number)
{
	if (Number >= 1073741824.0) 
	{
		return (2.0 * FMath::Fmod(Number, 1073741824.0)) - 1073741824.0;
	}
	else if (Number <= -1073741824.0) 
	{
		return (2.0 * FMath::Fmod(Number, 1073741824.0)) + 1073741824.0;
	}
	else 
	{
		return Number;
	}
}


float UNoiseGen::ValueCoherentNoise3D(FVector Coordinates, int32 Seed, FNoiseQuality noiseQuality)
{
	// Create a unit-length cube aligned along an integer boundary.  This cube
	// surrounds the input point.
	int x0 = (Coordinates.X > 0.0 ? (int)Coordinates.X : (int)Coordinates.X - 1);
	int x1 = x0 + 1;
	int y0 = (Coordinates.Y > 0.0 ? (int)Coordinates.Y : (int)Coordinates.Y - 1);
	int y1 = y0 + 1;
	int z0 = (Coordinates.Z > 0.0 ? (int)Coordinates.Z : (int)Coordinates.Z - 1);
	int z1 = z0 + 1;

	// Map the difference between the coordinates of the input value and the
	// coordinates of the cube's outer-lower-left vertex onto an S-curve.
	double xs = 0, ys = 0, zs = 0;
	switch (noiseQuality) {
	case FNoiseQuality::QUALITY_FAST:
		xs = (Coordinates.X - (double)x0);
		ys = (Coordinates.Y - (double)y0);
		zs = (Coordinates.Z - (double)z0);
		break;
	case FNoiseQuality::QUALITY_STD:
		xs = UNoiseInterp::SCurve3(Coordinates.X - (double)x0);
		ys = UNoiseInterp::SCurve3(Coordinates.Y - (double)y0);
		zs = UNoiseInterp::SCurve3(Coordinates.Z - (double)z0);
		break;
	case FNoiseQuality::QUALITY_BEST:
		xs = UNoiseInterp::SCurve5(Coordinates.X - (double)x0);
		ys = UNoiseInterp::SCurve5(Coordinates.Y - (double)y0);
		zs = UNoiseInterp::SCurve5(Coordinates.Z - (double)z0);
		break;
	}

	// Now calculate the noise values at each vertex of the cube.  To generate
	// the coherent-noise value at the input point, interpolate these eight
	// noise values using the S-curve value as the interpolant (trilinear
	// interpolation.)
	double n0, n1, ix0, ix1, iy0, iy1;
	n0 = ValueNoise3D(x0, y0, z0, Seed);
	n1 = ValueNoise3D(x1, y0, z0, Seed);
	ix0 = UNoiseInterp::LinearInterp(n0, n1, xs);
	n0 = ValueNoise3D(x0, y1, z0, Seed);
	n1 = ValueNoise3D(x1, y1, z0, Seed);
	ix1 = UNoiseInterp::LinearInterp(n0, n1, xs);
	iy0 = UNoiseInterp::LinearInterp(ix0, ix1, ys);
	n0 = ValueNoise3D(x0, y0, z1, Seed);
	n1 = ValueNoise3D(x1, y0, z1, Seed);
	ix0 = UNoiseInterp::LinearInterp(n0, n1, xs);
	n0 = ValueNoise3D(x0, y1, z1, Seed);
	n1 = ValueNoise3D(x1, y1, z1, Seed);
	ix1 = UNoiseInterp::LinearInterp(n0, n1, xs);
	iy1 = UNoiseInterp::LinearInterp(ix0, ix1, ys);
	return UNoiseInterp::LinearInterp(iy0, iy1, zs);
}

float UNoiseGen::ValueNoise3D(int32 X, int32 Y, int32 Z, int32 Seed)
{
	return 1.0 - ((float)IntValueNoise3D(X, Y, Z, Seed) / 1073741824.0);
}