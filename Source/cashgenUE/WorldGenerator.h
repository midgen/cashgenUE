// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "ZoneBlock.h"
struct GridRow;

class CASHGENUE_API WorldGenerator
{

	TArray<GridRow> MyGrid;
	

public:
	static const int X_SIZE = 20;
	static const int Y_SIZE = 20;

	int8 InitialiseTerrainGrid(const int aX, const int aY);
	TArray<GridRow>* GetTerrainGrid();

};



struct GridRow
{
	TArray<ZoneBlock> blocks;
};

