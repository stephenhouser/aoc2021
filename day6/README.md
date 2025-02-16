# Day 6: Lanternfish

Fish have ages, when they reach age 0 they reproduce a new age 8 fish and reset to 6. Just store all the fish in a vector by age, e.g. all age 0 fish are in fish[0]. Then iterate the 80 days, decreasing the age of the fish, and adding in new fish as needed. At the end, we have our solution.

Part 2 was just for a longer 256 day period.

Change to just use vector rather than map. Had a map in first version, totally not needed.

Key insight was not having to track all the fish, just the number of fish at each age. Then iterate.