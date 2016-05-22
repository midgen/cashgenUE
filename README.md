"# cashgen" 

Infinite Procedural World Generator for UE 4.10

This initial release of the plugin has been pared back to the basic terrain generation element. It is built using the MIT-licensed FastNoise for noise generation (https://github.com/Auburns/FastNoise).

#Instructions

Copy the Cashgen plugin to your project's Plugins folder. The main class you are interested in is WorldManager, place an instance in your level and call the SpawnZones function, passing in a reference to the player pawn and the noise generation parameters (see below for examples).

There is a development thread on the UnrealEngine forums which contains some example blueprints you can use.

https://forums.unrealengine.com/showthread.php?109894-Cashgen-(kinda)-Infinite-Procedural-World-Generator

##NoiseConfig

Noise Type = Simplex Fractal  
Fractal Type = Ridged Multi  
Octaves = 7  
Frequency = 0.000004  
Seed = 1234567  
Lacunarity = 2.0  
Gain = 0.5  
Sample Factor = 1.0  

##ZoneConfig
X/Y Units = 32  
Unit Size = 750  
Amplitude = 30,000  
Your choice of material  
LOD0 = 100000  
LOD1 = 250000  
LOD2 = 250000  
LOD3 = 250000  

##Spawn Zones
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

* LOD change flicker, will update to 4.11 and use dithered LOD transition effect
* Implement noise modules and masking 
* Re-implement instanced mesh spawning with weightmap based configuration


