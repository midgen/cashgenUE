// FastNoise.h
//
// MIT License
//
// Copyright(c) 2016 Jordan Peck
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// The developer's email is jorzixdan.me2@gzixmail.com (for great email, take
// off every 'zix'.)
//
// UnrealEngine-ified by Chris Ashworth 2016
// 

#pragma once
#include "UnrealFastNoisePlugin.h"
#include "Object.h"
#include "UFNNoiseGenerator.h"
#include "FastNoise.generated.h"

UENUM(BlueprintType)
enum ENoiseType { Value, ValueFractal, Gradient, GradientFractal, Simplex, SimplexFractal, Cellular, CellularHQ, WhiteNoise };
UENUM(BlueprintType)
enum EInterp { InterpLinear = 0, InterpHermite = 1, InterpQuintic = 2 };
UENUM(BlueprintType)
enum EFractalType { FBM, Billow, RigidMulti };
UENUM(BlueprintType)
enum ECellularDistanceFunction { Euclidean, Manhattan, Natural };
UENUM(BlueprintType)
enum ECellularReturnType { CellValue, NoiseLookup, Distance2Center, Distance2CenterXValue, Distance2CenterSq, Distance2CenterSqXValue, Distance2Edge, Distance2EdgeXValue, Distance2EdgeSq, Distance2EdgeSqXValue };


UCLASS()
class UFastNoise : public UUFNNoiseGenerator
{
	GENERATED_UCLASS_BODY()
public:
	~UFastNoise() { delete m_cellularNoiseLookup; }

	UFUNCTION(BlueprintCallable, Category="UnrealFastNoise")
	void SetSeed(int seed) { m_seed = seed; }
	UFUNCTION(BlueprintCallable, Category = "UnrealFastNoise")
	int GetSeed() { return m_seed; }
	UFUNCTION(BlueprintCallable, Category = "UnrealFastNoise")
	void SetFrequency(float frequency) { m_frequency = frequency; }
	UFUNCTION(BlueprintCallable, Category = "UnrealFastNoise")
	void SetInterp(EInterp interp) { m_interp = interp; }
	UFUNCTION(BlueprintCallable, Category = "UnrealFastNoise")
	void SetNoiseType(ENoiseType noiseType) { m_noiseType = noiseType; }
	UFUNCTION(BlueprintCallable, Category = "UnrealFastNoise")
	void SetFractalOctaves(int octaves) { m_octaves = octaves; }
	UFUNCTION(BlueprintCallable, Category = "UnrealFastNoise")
	void SetFractalLacunarity(float lacunarity) { m_lacunarity = lacunarity; }
	UFUNCTION(BlueprintCallable, Category = "UnrealFastNoise")
	void SetFractalGain(float gain) { m_gain = gain; }
	UFUNCTION(BlueprintCallable, Category = "UnrealFastNoise")
	void SetFractalType(EFractalType fractalType) { m_fractalType = fractalType; }
	UFUNCTION(BlueprintCallable, Category = "UnrealFastNoise")
	void SetCellularDistanceFunction(ECellularDistanceFunction cellularDistanceFunction) { m_cellularDistanceFunction = cellularDistanceFunction; }
	UFUNCTION(BlueprintCallable, Category = "UnrealFastNoise")
	void SetCellularReturnType(ECellularReturnType cellularReturnType) { m_cellularReturnType = cellularReturnType; }
	UFUNCTION(BlueprintCallable, Category = "UnrealFastNoise")
	void SetCellularNoiseLookup(UFastNoise* noise) { m_cellularNoiseLookup = noise; }

	/*
	Timing below are averages of time taken for 1 million iterations on a single thread
	Default noise settings
	CPU: i7 4790k @ 4.0Ghz
	VS 2013 - C++ Console Application
	*/

	//3D												// Win32	x64
	float GetValue(float x, float y, float z);			// 14 ms	14 ms
	float GetValueFractal(float x, float y, float z);	// 48 ms	49 ms

	float GetGradient(float x, float y, float z);		// 23 ms	22 ms
	float GetGradientFractal(float x, float y, float z);// 80 ms	73 ms

	float GetSimplex(float x, float y, float z);		// 30 ms	30 ms
	float GetSimplexFractal(float x, float y, float z);	// 98 ms	101 ms

	float GetCellular(float x, float y, float z);		// 123 ms	113 ms
	float GetCellularHQ(float x, float y, float z);		// 433 ms	449 ms

	float GetWhiteNoise(float x, float y, float z);		// 1.5 ms	1.5 ms
	float GetWhiteNoiseInt(int x, int y, int z);

	virtual float GetNoise3D(float x, float y, float z) override;

	//2D												// Win32	x64
	float GetValue(float x, float y);					// 8 ms 	8 ms
	float GetValueFractal(float x, float y);			// 29 ms	29 ms

	float GetGradient(float x, float y);				// 12 ms	11 ms
	float GetGradientFractal(float x, float y);			// 43 ms	40 ms

	float GetSimplex(float x, float y);					// 17 ms	17 ms
	float GetSimplexFractal(float x, float y);			// 55 ms	52 ms

	float GetCellular(float x, float y);				// 35 ms	33 ms
	float GetCellularHQ(float x, float y);				// 96 ms	90 ms

	float GetWhiteNoise(float x, float y);				// 1 ms		1 ms
	float GetWhiteNoiseInt(int x, int y);				// 1 ms		1 ms

	virtual float GetNoise2D(float x, float y) override;

	//4D
	float GetSimplex(float x, float y, float z, float w);

	float GetWhiteNoise(float x, float y, float z, float w);
	float GetWhiteNoiseInt(int x, int y, int z, int w);

protected:
	int m_seed = 0;
	float m_frequency = 0.01f;
	EInterp m_interp = EInterp::InterpQuintic;
	ENoiseType m_noiseType = ENoiseType::Value;

	unsigned int m_octaves = 3;
	float m_lacunarity = 2.0f;
	float m_gain = 0.5f;
	EFractalType m_fractalType = EFractalType::FBM;

	ECellularDistanceFunction m_cellularDistanceFunction = ECellularDistanceFunction::Euclidean;
	ECellularReturnType m_cellularReturnType = ECellularReturnType::CellValue;
	UFastNoise* m_cellularNoiseLookup = nullptr;

	//3D
	float _ValueFractalFBM(float x, float y, float z);
	float _ValueFractalBillow(float x, float y, float z);
	float _ValueFractalRigidMulti(float x, float y, float z);
	float _Value(int seed, float x, float y, float z);

	float _GradientFractalFBM(float x, float y, float z);
	float _GradientFractalBillow(float x, float y, float z);
	float _GradientFractalRigidMulti(float x, float y, float z);
	float _Gradient(int seed, float x, float y, float z);

	float _SimplexFractalFBM(float x, float y, float z);
	float _SimplexFractalBillow(float x, float y, float z);
	float _SimplexFractalRigidMulti(float x, float y, float z);
	float _Simplex(int seed, float x, float y, float z);

	float _Cellular(float x, float y, float z);
	float _CellularHQ(float x, float y, float z);
	float _Cellular2Edge(float x, float y, float z);
	float _Cellular2EdgeHQ(float x, float y, float z);

	inline static int CoordLUTIndex(int seed, int x, int y, int z);
	inline float GetValCoord(int seed, int x, int y, int z);
	inline float GetGradCoord(int seed, int xi, int yi, int zi, float x, float y, float z);

	//2D
	float _ValueFractalFBM(float x, float y);
	float _ValueFractalBillow(float x, float y);
	float _ValueFractalRigidMulti(float x, float y);
	float _Value(int seed, float x, float y);

	float _GradientFractalFBM(float x, float y);
	float _GradientFractalBillow(float x, float y);
	float _GradientFractalRigidMulti(float x, float y);
	float _Gradient(int seed, float x, float y);

	float _SimplexFractalFBM(float x, float y);
	float _SimplexFractalBillow(float x, float y);
	float _SimplexFractalRigidMulti(float x, float y);
	float _Simplex(int seed, float x, float y);

	float _Cellular(float x, float y);
	float _CellularHQ(float x, float y);
	float _Cellular2Edge(float x, float y);
	float _Cellular2EdgeHQ(float x, float y);
	
	inline int CoordLUTIndex(int seed, int x, int y);
	inline float GetValCoord(int seed, int x, int y);
	inline float GetGradCoord(int seed, int xi, int yi, float x, float y);

	//4D
	float _Simplex(float x, float y, float z, float w);
	inline static int CoordLUTIndex(int seed, int x, int y, int z, int w);
};
