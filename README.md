"# cashgen" 

Infinite Procedural World Generator for UE 4.10

Basic terrain, RidgedMulti and Billow Noise with a Perlin Noise mask:
https://www.youtube.com/watch?v=O0udSNv8COA

With some marketplace assets configured in the biome spawning section:
https://www.youtube.com/watch?v=p7PhpJCcipA

Code now refactored into plugin format. Included is a fixed version of UnrealLibNoise, I hope to have these fixes incorporated upstream. https://github.com/NovanMK2/UnrealLibNoise for information and license.

CashGen plugin is released under the MIT license (see LICENSE.TXT)
UnrealLibNoise is as per the author, LGPL3

Feel free to contribute! Known issues/TODO :

* Normals/Tangents at the edge of mesh sections are wonky
* Biome spawning needs a bit of tweaking
* Some decent materials would be nice
