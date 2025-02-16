# Day 8: Seven Segment Search

Part 1: How many output strings have 2, 3, 4, or 8 characters. Simple count of those.

Part 2: Decode all the digits and sum the result.

Tricky part is the deduction logic to figure out which observed pattern of characters is equivalent to which displayed digit. The easy ones are used to see what differes in the five and six digit codes. Just needed to work them out manually and then code the logic.

The output digit codes don't match the observed ones. That is the letters could be in any order in the output values. Shortcut solution was to sort the chars in the observed and output, then they will match and can be compared.

Alternative would be to use bitset<8> and set bits for each letter. This might make a number of other places more concise.