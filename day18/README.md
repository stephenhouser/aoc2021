# Day 18: Snailfish


I used an AST to solve. Thought about just parsing the string and doing it the quick way, but wanting to play with more data structures in C++. It's a hack though, needs some serious cleanup. Especially around the prev/next elements in the in-order traversal. An iterator would be nice.

Tree iterator on SO:

https://stackoverflow.com/questions/12684191/implementing-an-iterator-over-binary-or-arbitrary-tree-using-c-11

Also, using a visitor pattern rather than just re-implementing every traversal.

Part 2 was really just to make sure I copied the tree and used copies and not modify the originals. Then cartesian addition of all of them to find the max.
