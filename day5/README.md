# Day 5: Hydrothermal Venture

Find number of points where lines cross. Another use of my point_t structure.

Use the line segments to fill a map<point_t, size_t> that counts how many lines end up crossing the given point. Just "draw" the lines into the map, then look for points with more than 1.

Part 2 just added diagoanl lines, so modify to accurately draw them.

Key part was rewrite of `points_on` functin that returned all the points that were on the given line.