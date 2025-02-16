# Day 7: The Treachery of Whales

Among list of horizontal positions, find the one location where all outer subs can meet at with minimum expense of fuel. Fuel costs 1 per 1 unit traveled.

Turns out, this is just the median location.

Part 2 the fuel expense cost 1 more per distance, 1, 2, 3, 4... so the gaussian sum for fuel cost. This makes the meeting place be near the mean (average) point. Change up to use (n * (n+1))/2 for fuel cost then use the min of the mean and mean+1 positions as it could be one of these two and rounding does not manage it.