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

float UNoiseInterp::CubicInterp(float n0, float n1, float n2, float n3, float a)
{
	float p = (n3 - n2) - (n0 - n1);
	float q = (n0 - n1) - p;
	float r = n2 - n0;
	float s = n1;
	return p * a * a * a + q * a * a + r * a + s;
}

float UNoiseInterp::LinearInterp(float n0, float n1, float a)
{
	return ((1.0 - a) * n0) + (a * n1);
}

float UNoiseInterp::SCurve3(float a)
{
	return (a * a * (3.0 - 2.0 * a));
}

float UNoiseInterp::SCurve5(float a)
{
	float a3 = a * a * a;
	float a4 = a3 * a;
	float a5 = a4 * a;
	return (6.0 * a5) - (15.0 * a4) + (10.0 * a3);
}