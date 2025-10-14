#!/usr/bin/env python3

# (score, position)
p1 = (7, 0)
p2 = (0, 0)
# p1 = (4, 0)
# p2 = (8, 0)
die = 1

def turn(player):
    # move 3 positions
    move = die + (die + 1) + (die + 2)
    pos = (player[0] + move) % 10
    score = player[1] + pos if pos != 0 else player[1] + 10
    return (pos, score)

while p1[1] < 1000 and p2[1] < 1000:
    p1 = turn(p1)
    die += 3
    print(f"Player 1 rolls {die} and moves to space {p1[0]} for a total score of {p1[1]}.")

    if p1[1] >= 1000:
        break

    p2 = turn(p2)
    die += 3
    print(f"Player 2 rolls {die} and moves to space {p2[0]} for a total score of {p2[1]}.")

losing_score = min(p1[1], p2[1])
rolls = die - 1
print(f"Final score: {losing_score * rolls}")