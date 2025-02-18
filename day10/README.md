# Day 10: Syntax Scoring

Part 1:
Key idea was to reduce string to non-conforming by repeatedly removing pairs of {}, [], (), <> until nothing could be removed. A corrupt string would have a closing character (},],),>) left somewhere within it. Then the result was scoring that character.


Part 2:
Filter to keep only incomplete sequences, then mirror those characters to get the closing string. Score this with the wacky *5 logic.
