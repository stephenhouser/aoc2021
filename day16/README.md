# Day 16: Packet Decoder

Part 1; made a `bitstream_t` to enable reading `n` bits at a time from the decoded stream. Maintains the position, like a file handle. Then just straightforward recursive parsing of the packets. Hoping this works for part 2.