# Day 16: Packet Decoder

Part 1; made a `bitstream_t` to enable reading `n` bits at a time from the decoded stream. Maintains the position, like a file handle. Then just straightforward recursive parsing of the packets. Hoping this works for part 2.

Part 2; With a parsed tree in hand, just crank out a recursive `eval()` function. Complication with `accumulate` needed to set the starting result to `(size_t)` as compiler defaulted to `int` even though `size_t` was where the result was going.

Other idea online, read the string in reverse then the bits will be in proper order and not have to tweak like I did in bitstream.