# Day19: Beacon Scanner

3D space, align scanners in 3D space

Start with distances among points and look for 12+11+10+9+8+7+6+5+4+3+2+1=67 matching distances or more. This would mean the two scanners match in at least 12 points between them.

select one of the common points as an origin in first scanner, try rotations and see if second scanner points match up (should have at least 12). if so we have a transformation for s2 to map to s1's space.

Can I then just merge the scanners and reduce the number of spaces, then keep reducing the new set until there is just one?




Old thoughts

Find three distances connected to 3 points across both scanners. These would define a common plane. 
    - track points that have distance and choose a common one?
    - find a point that has 2 common distances to the other scanner

    - distance from 0,0 for 3 points that match and have the same distances between them

        A---B
        \   /
         \ C 

    - Choose 3 points randomly and keep choosing?

Find 3 points that line up when rotated