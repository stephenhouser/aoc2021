# Day 9: Smoke Basin

Better at using the functional views/ranges on this one.

Create a vector contianing the low points:
Use `charmap_t` to read the map. Then filter `all_points` with a predicate `is_low` that returns true of all of that points neighbors (using `neighbors_of`) have a higher value. Transform this to just the point.

Part 1:
Transform the low points their risk level (char + 1), then reduce (sum) and return.

Part 2:
Transform the low points to basin sizes using a flood fill. Then sort, take top 3, and reduce using multiplies. Flood fill looks for 9 or invalid as border. Returns the size of the filled area, could return the points in the area.

