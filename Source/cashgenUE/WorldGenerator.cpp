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
			row.blocks.Add(ZoneBlock(x*50.0f, FColor::Cyan));
		}
		MyGrid.Add(row);
	}

	// Now set the LRUD pointers
	for (int x = 0; x < aX; ++x)
	{
		for (int y = 0; y < aY; ++y)
		{
			// Set left pointers
			if (x == 0) {
				MyGrid[x].blocks[y].left = nullptr;
			}
			else {
				MyGrid[x].blocks[y].left = &MyGrid[x - 1].blocks[y];
			}
			// Set right pointers
			if (x == aX - 1) {
				MyGrid[x].blocks[y].right = nullptr;
			}
			else {
				MyGrid[x].blocks[y].right = &MyGrid[x + 1].blocks[y];
			}

			// Set bottom pointers
			if (y == 0) {
				MyGrid[x].blocks[y].down = nullptr;
			}
			else {
				MyGrid[x].blocks[y].down = &MyGrid[x].blocks[y - 1];
			}
			// Set top pointers
			if (y == aY - 1) {
				MyGrid[x].blocks[y].up = nullptr;
			}
			else {
				MyGrid[x].blocks[y].up = &MyGrid[x].blocks[y + 1];
			}


		}
		
	}

}
