# cashgenUE
Procedural Terrain Generator for UnrealEngine 4.15

This plugin generates heightmap-based terrain tiles in realtime, and move the tiles around to track a player pawn. 

Features:

* Multithreaded heightmap, erosion and geometry generation
* A simple hydraulic erosion algorithm
* Multiple tile LODs with per-LOD collision, tesselation and subdivision
* Dithered LOD transitions (when using a suitable material instance)

It has dependencies on :

UE4RuntimeMeshComponent by Koderz, an enhanced procedural mesh component
UnrealFastNoise by myself, a modular noise generation plugin 

1. Checkout UE4RuntimeMeshComponent into your engine or project plugins folder ( https://github.com/Koderz/UE4RuntimeMeshComponent )
2. Checkout UnrealFastNoisePlugin into your engine or project plugins folder ( https://github.com/midgen/UnrealFastNoise )
3. Checkout this repository into your engine or project plugins folder
4. Add "CashGen", "UnrealFastNoise" to your project Build.cs (required to package project)
5. Open the project, and add a CGTerrainManager. 
6. On the beginplay event, call the SetupTerrain method.


