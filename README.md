# cashgenUE
Procedural Terrain Generator for UnrealEngine 4.17

This plugin generates heightmap-based terrain tiles in realtime, and move the tiles around to track a player pawn. 

Features:

* Multithreaded heightmap, erosion and geometry generation
* A simple hydraulic erosion algorithm
* Multiple tile LODs with per-LOD collision, tesselation and subdivision
* Dithered LOD transitions (when using a suitable material instance)

It has dependencies on :

* UE4RuntimeMeshComponent by Koderz, an enhanced procedural mesh component
* UnrealFastNoise by myself, a modular noise generation plugin 

1. Checkout UE4RuntimeMeshComponent into your engine or project plugins folder ( https://github.com/Koderz/UE4RuntimeMeshComponent )
2. Checkout UnrealFastNoisePlugin into your engine or project plugins folder ( https://github.com/midgen/UnrealFastNoise )
3. Checkout this repository into your engine or project plugins folder
4. Add "CashGen", "UnrealFastNoisePlugin" to your project Build.cs (required to package project)
```csharp
PrivateDependencyModuleNames.AddRange(new string[] { "CashGen", "UnrealFastNoisePlugin" });
PublicDependencyModuleNames.AddRange(new string[] { "CashGen", "UnrealFastNoisePlugin" });
```
5. Open the project. Create a new Blueprint based on CGTerrainManager
6. OnBeginPlay in the blueprint, call SetupTerrain() and fill out all required parameters
7. Add a CGTerrainTrackerComponent to any actors you wish to have terrain formed around
8. You can optionally tell the tracker component to hide/disable gravity on the spawned actors until terrain generation is complete
9. Vertex Colours - Red = slope. Green = the biome mask specified in terrain config


