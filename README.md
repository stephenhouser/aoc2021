# Advent of Code 2021
## Stephen Houser

Contains my solutions to the [Advent of Code 2021](https://adventofcode.com/2021).

Tools:
- C++ 23 (STL only)
- Visual Studio Code
- macOS / Linux (primary)
- git
- cmake

## Running

Within each day, when developing

    - run `cmake .` to generate a Makefile
    - to build `make day21`
    - to run on test data (`test.txt`) run `make test21`
    - to run on live data (`input.txt`) run `make live21`

To check and run everything, in the project root

```
    make build
    cd build
    cmake ..    # generate Makefile
    make        # builds everything
```

I'm not sure what my `summary.cpp` file is doing, but it's not working. It should run 
