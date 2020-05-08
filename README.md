# cashgenUE
Procedural Terrain Generator for UnrealEngine 4.25

This plugin generates heightmap-based terrain tiles in realtime, and move the tiles around to track a player pawn. 

[![CashGen Demo](http://img.youtube.com/vi/r68mpFKRAbA/0.jpg)](http://www.youtube.com/watch?v=r68mpFKRAbA "Video Title")

Features:

* Multithreaded heightmap, erosion and geometry generation
* A simple hydraulic erosion algorithm
* Multiple tile LODs with per-LOD collision, tesselation and subdivision
* Dithered LOD transitions (when using a suitable material instance)
* Slope scalar in vertex colour channel 
* Depth map texture generation for water material

It has dependencies on :

* UnrealFastNoise by myself, a modular noise generation plugin 

1. Create a new C++ project
2. Checkout UnrealFastNoisePlugin into your engine or project plugins folder ( https://github.com/midgen/UnrealFastNoise )
3. Checkout this repository into your engine or project plugins folder
4. Add "CashGen", "UnrealFastNoisePlugin" to your project Build.cs (required to package project)
```csharp
PrivateDependencyModuleNames.AddRange(new string[] { "CashGen", "UnrealFastNoisePlugin" });
```
6. Create a new Blueprint based on CGTerrainManager
7. OnBeginPlay in the blueprint, call SetupTerrain() and fill out all required parameters
8. Add a CGTerrainTrackerComponent to any actors you wish to have terrain formed around
9. You can optionally tell the tracker component to hide/disable gravity on the spawned actors until terrain generation is complete
10. Vertex Colours - Red = slope. Green = the biome mask specified in terrain config

For samples, please check :

https://github.com/midgen/CashDemo


