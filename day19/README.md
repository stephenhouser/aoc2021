# Day19: Beacon Scanner

3D space, align scanners in 3D space. Good thing my point_t is 3d

Start with distances among points and look for (12+(12+1))/2 = 67 matching distances or more. This means the two scanners match in at least 12 points between them.

Select one of the common distances from both scanners and the points that are that distance apart. Use the first one as the origin to merge into. Try mapping (transform and rotate) all the other points in the second space into the first. If we get more than the required 12 matches, merge the points into the first (origin) space. Add the transformed origin into the new space as a "scanner".

common points as an origin in first scanner, try rotations and see if second scanner points match up (should have at least 12). if so we have a transformation for s2 to map to s1's space. Then just merge the scanners and reduce the number of spaces, then keep reducing the new set until there is just one. Works.

Part 2: add scanners to scanner_t and track the merged transformed origins in a vector. Then just look for the max distance among these.

