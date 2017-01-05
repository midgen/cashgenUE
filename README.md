"# cashgen" 

Infinite Procedural World Generator for UE 4.14

The plugin generates a tiled heightmap-based terrain dynamically around a specified actor in the level. As the actor moves, tiles are repositioned and regenerated around it.

[![Cashgen Demo](http://img.youtube.com/vi/T-dM0blrBa3LY/0.jpg)](https://www.youtube.com/watch?v=dM0blrBa3LY)

Features:

* Modular noise generation system ( https://github.com/midgen/UnrealFastNoise )
* Biome blending (both materials and noise modules)
* Simple hydraulic (droplet) erosion
* Threaded heightmap and erosion system
* LOD system with hidden seams and dithered transition


#Instructions

* Have UE 4.14 or higher installed. (Tested on 4.14.0)
* Have the RuntimeMeshComponent plugin installed from the UE marketplace.
* Clone the repo.
* Right click the UPROJECT file, selected 'Generate Visual Studio Files'
* Open the .SLN
* Build, launch!



Credits to :
* Chris Conway (koderz) for the RuntimeMeshComponent (pre-requisite, install from UE Marketplace)
* https://github.com/Auburns/FastNoise for the noise generation code
* The OceanPlugin contributors
