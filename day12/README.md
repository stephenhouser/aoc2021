# Day 12: Passage Pathing

Part 1: Breadth first search.
Read all the nodes into a map of name to vector of neighbors. Using bfs, add current cave to a running path, if we are in a small cave, remove it cave from map passed to recursive children, then recurse on each neighbor of the current cave. When recursion reaches "end" add the collected path to the vector of possible paths. Return the size of the collected paths for our solution.

Part 2: Can visit one small cave twice.

note that all caves are >= 2 chars except initial small test file.
for each small cave
    make 2nd char uppercase (promoted)
    run bfs as normal except,
        when promoted small cave is encoutered, rather than remove it, demote it making both chars lower case.
        the second time it is encoutered it will be removed as normal.
