# Day 4: Giant Squid

Which bingo board wins.

Instead of maintaining rows and columns separately, just maintain a single vector of rows/columns for each bingo board. For each number drawn, remove that number from all the boards. If any of the vectors of a board becomes empty, that board has won.

Which bingo board wins last.

Same idea, just keep track of active boards and wait until the last one is a winner.