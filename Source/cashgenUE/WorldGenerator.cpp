// Fill out your copyright notice in the Description page of Project Settings.

#include "cashgenUE.h"
#include "WorldGenerator.h"


TArray<GridRow>* WorldGenerator::GetTerrainGrid()
{
	return &MyGrid;
}

void WorldGenerator::InitialiseTerrainGrid(int aX, int aY)
{
	for (int x = 0; x < aX; ++x)
	{
		GridRow row;
		for (int y = 0; y < aY; ++y)
		{
			row.blocks.Add(ZoneBlock(x *  y * 10.0f, FColor::Cyan));
		}
		MyGrid.Add(row);
	}
}
