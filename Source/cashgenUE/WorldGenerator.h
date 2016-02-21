// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "ZoneBlock.h"
#include "PerlinNoise.h"
struct GridRow;

class CASHGENUE_API WorldGenerator
{

	TArray<GridRow> MyGrid;
	float MyMaxHeight;

public:
	int8 InitialiseTerrainGrid(const int aX, const int aY, double aFloor, double aPersistence, double aFrequency, double aAmplitude, int aOctaves, int aRandomseed);
	TArray<GridRow>* GetTerrainGrid();

};



struct GridRow
{
	TArray<ZoneBlock> blocks;
};

