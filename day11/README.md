# Day 11: Dumbo Octopus

Rather straightforward day, buld a map, then follow the directions.

Part 1: Increment all the map values by 1, add any locations that go over 9 to a pending set of locations that need to "flash". Then while the pending set is not empty, pull a location from it, add it to a set of already flashed locations for this iteration and increase each of it's neighbors. If any of those in turn flash and have not already flashed this iteration, add them to the pending flasher set. Then go through all the locations that increased greater than 9, count them up and reset them to 0.

For each flash() return the number of locations that flashed, the size of the flashed set.

Part 2: Already had the number of locations that flashed from flash() in part 1. Just use that to keep flashing until the number of flashes was equal to the map size (x * y).