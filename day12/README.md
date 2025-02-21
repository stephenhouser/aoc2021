# Day 12: Passage Pathing

Part 1: Breadth first search. Read all the nodes into a map of name to vector of neighbors. Using bfs, add current cave to a running path, if we are in a small cave, remove it cave from map passed to recursive children, then recurse on each neighbor of the current cave. When recursion reaches "end" add the collected path to the vector of possible paths. Return the size of the collected paths for our solution. 

Revised after doing part 2 to make more things `const`. Use an `in_path` function instead of modify the cavemap. `in_path` checks if a name is in the current recursed path. We can thus avoid traversing small caves multiple times.

Part 2: Can visit one small cave twice. Similar Breadth first search, but pass a `promoted` node to `bfs`. Then for each small cave call bfs with that node as promoted, collect all paths at their ends. In `bfs`, when we are about to recurse with neighbors, check if we are traversing the promoted cave, and "demote" it (by not passing it down) for future recrusions on this branch. This way we can traverse the one promoted cave twice and all other small caves only once. Collect all paths like in part 1, but keep them in a set of strings to easily de-duplicate them.
