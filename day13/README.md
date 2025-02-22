# Day 13: Transparent Origami

Use the point struct, updated to add read_points. Read folds as points as well, use lambda to adjust so the point was the fold-point (line). Then, simply "fold" along the needed line with a simple fold function. Put the points in a set, count the number of points at the end.

Part 2 was to run all the folds, same as above. Convert final result to charmap_t and then print it out. Read the letters.

Polished up the point_t struct and charmap_t struct;

`read_points()` now will take a stream and read points up to the end of file or an empty line. It also has an optional lambda to modify the points before being put into the result vector.

Solution has a `from_points` for charmap to make map from points.