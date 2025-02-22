# Day 14: Extended Polymerization

More or less just follow the process for "folding" or producing the string 10 times. Then counthow many times each character occurs in the resulting string. Return max - min.

Part 2, do that 40 times.
What is the optimization? Rethink as pairs that produce two new pairs at each iteration. Then we only need to keep track of the active pairs and the characters that get produced to create them.

Start with all the pairs from the staring pattern in a queue (map), for each pair, get the new center character and add two new pairs; e.g. NN -> C in the rules, when we encounter NN we add NC and CN to the next set of pairs to iterate on. Keep track of the count of these pairs and the number of characters produced, here it was C. Use the count of the pair NN to tell how many new NC, CN, and C's were produced.

Iterate 40 times and report the character counts!