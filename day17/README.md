# Day 17: Trick Shot

Targeting. Use algebra? No, just search

Part 1: Somewhat feels like a brute force. Roughly try a range of possible dx vector components that could possibly hit the target, add them all to a set. Do the same across dy values. Then loop over all combinations of possible dx and dy values, simulate the launch and track the largest y value reached across all launches.

Part 2: Instead of tracking the max y value reached, drop all the combinations that actually hit the target area. Most of the work was done in part 1 by my bad brute force work.
