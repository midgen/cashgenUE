"# cashgen" 

Infinite Procedural World Generator for UE 4.10

This initial release of the plugin has been pared back to the basic terrain generation element. It was previously built on UnrealLibNoise which provided a nice modular noise system but as it was LGPL I had to ditch it in favour of FastNoise (https://github.com/Auburns/FastNoise) which is MIT licensed so ready to use in your UE projects.

Instructions
============

Copy the Cashgen plugin to your project's Plugins folder. Optionally copy the example map and blueprinted WorldManager.

Configration is done in the SpawnWorld blueprint function. You must provide a Pawn reference for the world to be populated around. Some example configuration values to get you started :

NoiseConfig
============
Noise Type = Simplex Fractal
Fractal Type = Ridged Multi
Octaves = 7
Frequency = 0.000004
Seed = 1234567
Lacunarity = 2.0
Gain = 0.5
Sample Factor = 1.0

ZoneConfig
===========
X/Y Units = 32
Unit Size = 750
Amplitude = 30,000
Your choice of material
LOD0 = 100000
LOD1 = 250000
LOD2 = 250000
LOD3 = 250000

Spawn Zones
==========
NumXZones = 25
NumYZones = 25
Max Threads = 1
Render Tokens = 4

The plugin previously included more advanced functions such as instanced foliage generation and biome masking. This will be re-introduced now that I have a cleaned up optimised base to start from. Here's some demos of the system in action :

Basic terrain, RidgedMulti and Billow Noise with a Perlin Noise mask:
https://www.youtube.com/watch?v=O0udSNv8COA

With some marketplace assets configured in the biome spawning section:
https://www.youtube.com/watch?v=p7PhpJCcipA

CashGen plugin is released under the MIT license (see LICENSE.TXT)

Feel free to contribute! This is and will remain an open-source project. In particular if anyone that's good on the materials side of things wants to improve my horrible coder-art efforts that would be great :)

Known issues/TODO :

* Not sure if the tangents are correct
* Flicker on LOD change (hope to update to 4.11 and use dithered transition
* Implement noise modules and masking 
* Re-implement instanced mesh spawning with weightmap based configuration


