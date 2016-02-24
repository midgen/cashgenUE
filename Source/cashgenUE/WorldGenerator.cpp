// Fill out your copyright notice in the Description page of Project Settings.

#include "cashgenUE.h"
#include "WorldGenerator.h"
#include "PerlinNoise.h"
#include "Kismet/KismetMathLibrary.h"


int8 WorldGenerator::InitialiseTerrainGrid(TArray<GridRow>* outZoneData, TArray<float>* outHeightMap, Point aOffset, int aX, int aY, double aFloor, double aPersistence, double aFrequency, double aAmplitude, int aOctaves, int aRandomseed)
{
	PerlinNoise noiseGen(aPersistence, aFrequency, aAmplitude, aOctaves, aRandomseed);
	MyMaxHeight = 0.0f;

	int32 exX = aX + 2;
	int32 exY = aY + 2;

	outZoneData->Reserve(exX);
	for (int x = 0; x < exX; ++x)
	{
		GridRow row;
		row.blocks.Reserve(exY);
		for (int y = 0; y < exY; ++y)
		{
			row.blocks.Add(ZoneBlock(noiseGen.GetHeight((aOffset.x * aX) + x, (aOffset.y * aY) + y), FColor::Cyan, x, y));
		}
		outZoneData->Add(row);
	}

	GridRow* MyGrid = outZoneData->GetData();

	// Floor pass
	for (int x = 0; x < exX; ++x)
	{
		for (int y = 0; y < exY; ++y)
		{
			if (MyGrid[x].blocks[y].Height < aFloor)
			{
				MyGrid[x].blocks[y].Height = 0;
			}
			else {
				MyGrid[x].blocks[y].Height -=aFloor;
			}

			if (MyGrid[x].blocks[y].Height > MyMaxHeight)
			{
				MyMaxHeight = MyGrid[x].blocks[y].Height;
			}
		}
	}

	// Now set the LRUD pointers
	for (int x = 0; x < exX; ++x)
	{
		for (int y = 0; y < exY; ++y)
		{ 
			// Bottom left case
			if (x == 0 && y == 0)
			{
				MyGrid[x].blocks[y].topLeftCorner.topLeftBlock = nullptr;
				MyGrid[x].blocks[y].topLeftCorner.topRightBlock = &MyGrid[x].blocks[y + 1];
				MyGrid[x].blocks[y].topLeftCorner.bottomLeftBlock = nullptr;
				MyGrid[x].blocks[y].topLeftCorner.bottomRightBlock = &MyGrid[x].blocks[y];

				MyGrid[x].blocks[y].topRightCorner.topLeftBlock = &MyGrid[x].blocks[y + 1];
				MyGrid[x].blocks[y].topRightCorner.topRightBlock = &MyGrid[x + 1].blocks[y + 1];
				MyGrid[x].blocks[y].topRightCorner.bottomLeftBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].topRightCorner.bottomRightBlock = &MyGrid[x + 1].blocks[y];

				MyGrid[x].blocks[y].bottomLeftCorner.topLeftBlock = nullptr;
				MyGrid[x].blocks[y].bottomLeftCorner.topRightBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].bottomLeftCorner.bottomLeftBlock = nullptr;
				MyGrid[x].blocks[y].bottomLeftCorner.bottomRightBlock = nullptr;

				MyGrid[x].blocks[y].bottomRightCorner.topLeftBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].bottomRightCorner.topRightBlock = &MyGrid[x + 1].blocks[y];
				MyGrid[x].blocks[y].bottomRightCorner.bottomLeftBlock = nullptr;
				MyGrid[x].blocks[y].bottomRightCorner.bottomRightBlock = nullptr;
			}
			// top left case
			if (x == 0 && y == exY -1)
			{
				MyGrid[x].blocks[y].topLeftCorner.topLeftBlock = nullptr;
				MyGrid[x].blocks[y].topLeftCorner.topRightBlock = nullptr;
				MyGrid[x].blocks[y].topLeftCorner.bottomLeftBlock = nullptr;
				MyGrid[x].blocks[y].topLeftCorner.bottomRightBlock = &MyGrid[x].blocks[y];

				MyGrid[x].blocks[y].topRightCorner.topLeftBlock = nullptr;
				MyGrid[x].blocks[y].topRightCorner.topRightBlock = nullptr;
				MyGrid[x].blocks[y].topRightCorner.bottomLeftBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].topRightCorner.bottomRightBlock = &MyGrid[x + 1].blocks[y];

				MyGrid[x].blocks[y].bottomLeftCorner.topLeftBlock = nullptr;
				MyGrid[x].blocks[y].bottomLeftCorner.topRightBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].bottomLeftCorner.bottomLeftBlock = nullptr;
				MyGrid[x].blocks[y].bottomLeftCorner.bottomRightBlock = &MyGrid[x].blocks[y - 1];

				MyGrid[x].blocks[y].bottomRightCorner.topLeftBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].bottomRightCorner.topRightBlock = &MyGrid[x + 1].blocks[y];
				MyGrid[x].blocks[y].bottomRightCorner.bottomLeftBlock = &MyGrid[x].blocks[y - 1];
				MyGrid[x].blocks[y].bottomRightCorner.bottomRightBlock = &MyGrid[x + 1].blocks[y - 1];
			}
			// top right case
			if (x == exX - 1 && y == exY - 1)
			{
				MyGrid[x].blocks[y].topLeftCorner.topLeftBlock = nullptr;
				MyGrid[x].blocks[y].topLeftCorner.topRightBlock = nullptr;
				MyGrid[x].blocks[y].topLeftCorner.bottomLeftBlock = &MyGrid[x - 1].blocks[y];
				MyGrid[x].blocks[y].topLeftCorner.bottomRightBlock = &MyGrid[x].blocks[y];

				MyGrid[x].blocks[y].topRightCorner.topLeftBlock = nullptr;
				MyGrid[x].blocks[y].topRightCorner.topRightBlock = nullptr;
				MyGrid[x].blocks[y].topRightCorner.bottomLeftBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].topRightCorner.bottomRightBlock = nullptr;

				MyGrid[x].blocks[y].bottomLeftCorner.topLeftBlock = &MyGrid[x - 1].blocks[y];
				MyGrid[x].blocks[y].bottomLeftCorner.topRightBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].bottomLeftCorner.bottomLeftBlock = &MyGrid[x - 1].blocks[y - 1];
				MyGrid[x].blocks[y].bottomLeftCorner.bottomRightBlock = &MyGrid[x].blocks[y - 1];

				MyGrid[x].blocks[y].bottomRightCorner.topLeftBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].bottomRightCorner.topRightBlock = nullptr;
				MyGrid[x].blocks[y].bottomRightCorner.bottomLeftBlock = &MyGrid[x].blocks[y - 1];
				MyGrid[x].blocks[y].bottomRightCorner.bottomRightBlock = nullptr;
			}
			// bottom right case
			if (x == exX - 1 && y == 0)
			{
				MyGrid[x].blocks[y].topLeftCorner.topLeftBlock = &MyGrid[x - 1].blocks[y + 1];
				MyGrid[x].blocks[y].topLeftCorner.topRightBlock = &MyGrid[x].blocks[y + 1];
				MyGrid[x].blocks[y].topLeftCorner.bottomLeftBlock = &MyGrid[x - 1].blocks[y];
				MyGrid[x].blocks[y].topLeftCorner.bottomRightBlock = &MyGrid[x].blocks[y];

				MyGrid[x].blocks[y].topRightCorner.topLeftBlock = &MyGrid[x].blocks[y + 1];
				MyGrid[x].blocks[y].topRightCorner.topRightBlock = nullptr;
				MyGrid[x].blocks[y].topRightCorner.bottomLeftBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].topRightCorner.bottomRightBlock = nullptr;

				MyGrid[x].blocks[y].bottomLeftCorner.topLeftBlock = &MyGrid[x - 1].blocks[y];
				MyGrid[x].blocks[y].bottomLeftCorner.topRightBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].bottomLeftCorner.bottomLeftBlock = nullptr;
				MyGrid[x].blocks[y].bottomLeftCorner.bottomRightBlock = nullptr;

				MyGrid[x].blocks[y].bottomRightCorner.topLeftBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].bottomRightCorner.topRightBlock = nullptr;
				MyGrid[x].blocks[y].bottomRightCorner.bottomLeftBlock = nullptr;
				MyGrid[x].blocks[y].bottomRightCorner.bottomRightBlock = nullptr;
			}

			// Set left edge pointers
			if (x == 0 && y > 0 && y < exY - 1)
			{
				MyGrid[x].blocks[y].topLeftCorner.topLeftBlock = nullptr;
				MyGrid[x].blocks[y].topLeftCorner.topRightBlock = &MyGrid[x].blocks[y + 1];
				MyGrid[x].blocks[y].topLeftCorner.bottomLeftBlock = nullptr;
				MyGrid[x].blocks[y].topLeftCorner.bottomRightBlock = &MyGrid[x].blocks[y];

				MyGrid[x].blocks[y].topRightCorner.topLeftBlock = &MyGrid[x].blocks[y + 1];
				MyGrid[x].blocks[y].topRightCorner.topRightBlock = &MyGrid[x + 1].blocks[y + 1];
				MyGrid[x].blocks[y].topRightCorner.bottomLeftBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].topRightCorner.bottomRightBlock = &MyGrid[x + 1].blocks[y];

				MyGrid[x].blocks[y].bottomLeftCorner.topLeftBlock = nullptr;
				MyGrid[x].blocks[y].bottomLeftCorner.topRightBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].bottomLeftCorner.bottomLeftBlock = nullptr;
				MyGrid[x].blocks[y].bottomLeftCorner.bottomRightBlock = &MyGrid[x].blocks[y - 1];

				MyGrid[x].blocks[y].bottomRightCorner.topLeftBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].bottomRightCorner.topRightBlock = &MyGrid[x + 1].blocks[y];
				MyGrid[x].blocks[y].bottomRightCorner.bottomLeftBlock = &MyGrid[x].blocks[y - 1];
				MyGrid[x].blocks[y].bottomRightCorner.bottomRightBlock = &MyGrid[x + 1].blocks[y - 1];
			}
			// Set right edge pointers
			if (x == exX - 1 && y > 0 && y < exY - 1) {
				MyGrid[x].blocks[y].topLeftCorner.topLeftBlock = &MyGrid[x - 1].blocks[y + 1];
				MyGrid[x].blocks[y].topLeftCorner.topRightBlock = &MyGrid[x].blocks[y + 1];
				MyGrid[x].blocks[y].topLeftCorner.bottomLeftBlock = &MyGrid[x - 1].blocks[y];
				MyGrid[x].blocks[y].topLeftCorner.bottomRightBlock = &MyGrid[x].blocks[y];

				MyGrid[x].blocks[y].topRightCorner.topLeftBlock = &MyGrid[x].blocks[y + 1];
				MyGrid[x].blocks[y].topRightCorner.topRightBlock = nullptr;
				MyGrid[x].blocks[y].topRightCorner.bottomLeftBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].topRightCorner.bottomRightBlock = nullptr;

				MyGrid[x].blocks[y].bottomLeftCorner.topLeftBlock = &MyGrid[x - 1].blocks[y];
				MyGrid[x].blocks[y].bottomLeftCorner.topRightBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].bottomLeftCorner.bottomLeftBlock = &MyGrid[x - 1].blocks[y - 1];
				MyGrid[x].blocks[y].bottomLeftCorner.bottomRightBlock = &MyGrid[x].blocks[y - 1];

				MyGrid[x].blocks[y].bottomRightCorner.topLeftBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].bottomRightCorner.topRightBlock = nullptr;
				MyGrid[x].blocks[y].bottomRightCorner.bottomLeftBlock = &MyGrid[x].blocks[y - 1];
				MyGrid[x].blocks[y].bottomRightCorner.bottomRightBlock = nullptr;
			}
			// Set bottom edge pointers
			if (y == 0 && x > 0 && x < exX -1) {
				MyGrid[x].blocks[y].topLeftCorner.topLeftBlock = &MyGrid[x - 1].blocks[y + 1];
				MyGrid[x].blocks[y].topLeftCorner.topRightBlock = &MyGrid[x].blocks[y + 1];
				MyGrid[x].blocks[y].topLeftCorner.bottomLeftBlock = &MyGrid[x - 1].blocks[y];
				MyGrid[x].blocks[y].topLeftCorner.bottomRightBlock = &MyGrid[x].blocks[y];

				MyGrid[x].blocks[y].topRightCorner.topLeftBlock = &MyGrid[x].blocks[y + 1];
				MyGrid[x].blocks[y].topRightCorner.topRightBlock = &MyGrid[x + 1].blocks[y + 1];
				MyGrid[x].blocks[y].topRightCorner.bottomLeftBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].topRightCorner.bottomRightBlock = &MyGrid[x + 1].blocks[y];

				MyGrid[x].blocks[y].bottomLeftCorner.topLeftBlock = &MyGrid[x - 1].blocks[y];
				MyGrid[x].blocks[y].bottomLeftCorner.topRightBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].bottomLeftCorner.bottomLeftBlock = nullptr;
				MyGrid[x].blocks[y].bottomLeftCorner.bottomRightBlock = nullptr;

				MyGrid[x].blocks[y].bottomRightCorner.topLeftBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].bottomRightCorner.topRightBlock = &MyGrid[x + 1].blocks[y];
				MyGrid[x].blocks[y].bottomRightCorner.bottomLeftBlock = nullptr;
				MyGrid[x].blocks[y].bottomRightCorner.bottomRightBlock = nullptr;
			}
			// Set top edge pointers
			if (y == exY - 1 && x > 0 && x < exX -1) {
				MyGrid[x].blocks[y].topLeftCorner.topLeftBlock = nullptr;
				MyGrid[x].blocks[y].topLeftCorner.topRightBlock = nullptr;
				MyGrid[x].blocks[y].topLeftCorner.bottomLeftBlock = &MyGrid[x - 1].blocks[y];
				MyGrid[x].blocks[y].topLeftCorner.bottomRightBlock = &MyGrid[x].blocks[y];

				MyGrid[x].blocks[y].topRightCorner.topLeftBlock = nullptr;
				MyGrid[x].blocks[y].topRightCorner.topRightBlock = nullptr;
				MyGrid[x].blocks[y].topRightCorner.bottomLeftBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].topRightCorner.bottomRightBlock = &MyGrid[x + 1].blocks[y];

				MyGrid[x].blocks[y].bottomLeftCorner.topLeftBlock = &MyGrid[x - 1].blocks[y];
				MyGrid[x].blocks[y].bottomLeftCorner.topRightBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].bottomLeftCorner.bottomLeftBlock = &MyGrid[x - 1].blocks[y - 1];
				MyGrid[x].blocks[y].bottomLeftCorner.bottomRightBlock = &MyGrid[x].blocks[y - 1];

				MyGrid[x].blocks[y].bottomRightCorner.topLeftBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].bottomRightCorner.topRightBlock = &MyGrid[x + 1].blocks[y];
				MyGrid[x].blocks[y].bottomRightCorner.bottomLeftBlock = &MyGrid[x].blocks[y - 1];
				MyGrid[x].blocks[y].bottomRightCorner.bottomRightBlock = &MyGrid[x + 1].blocks[y - 1];
			}

			// Normal cases :
			if (x > 0 && x < exX - 1 && y > 0 && y < exY - 1) {
				MyGrid[x].blocks[y].topLeftCorner.topLeftBlock = &MyGrid[x - 1].blocks[y + 1];
				MyGrid[x].blocks[y].topLeftCorner.topRightBlock = &MyGrid[x].blocks[y + 1];
				MyGrid[x].blocks[y].topLeftCorner.bottomLeftBlock = &MyGrid[x - 1].blocks[y];
				MyGrid[x].blocks[y].topLeftCorner.bottomRightBlock = &MyGrid[x].blocks[y];

				MyGrid[x].blocks[y].topRightCorner.topLeftBlock = &MyGrid[x].blocks[y + 1];
				MyGrid[x].blocks[y].topRightCorner.topRightBlock = &MyGrid[x + 1].blocks[y + 1];
				MyGrid[x].blocks[y].topRightCorner.bottomLeftBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].topRightCorner.bottomRightBlock = &MyGrid[x + 1].blocks[y];

				MyGrid[x].blocks[y].bottomLeftCorner.topLeftBlock = &MyGrid[x - 1].blocks[y];
				MyGrid[x].blocks[y].bottomLeftCorner.topRightBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].bottomLeftCorner.bottomLeftBlock = &MyGrid[x - 1].blocks[y - 1];
				MyGrid[x].blocks[y].bottomLeftCorner.bottomRightBlock = &MyGrid[x].blocks[y - 1];

				MyGrid[x].blocks[y].bottomRightCorner.topLeftBlock = &MyGrid[x].blocks[y];
				MyGrid[x].blocks[y].bottomRightCorner.topRightBlock = &MyGrid[x + 1].blocks[y];
				MyGrid[x].blocks[y].bottomRightCorner.bottomLeftBlock = &MyGrid[x].blocks[y - 1];
				MyGrid[x].blocks[y].bottomRightCorner.bottomRightBlock = &MyGrid[x + 1].blocks[y - 1];
			}
		}

		
	}

	// Now run through and calculate vertex heights
	for (int x = 0; x < exX; ++x)
	{
		for (int y = 0; y < exY; ++y)
		{
			MyGrid[x].blocks[y].ProcessCorners(MyMaxHeight);

		}
	}

	return 0;
}
