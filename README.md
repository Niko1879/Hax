# Hax

### What
Hex engine usable on any board size. Plays a strong game but haven't implemented the swap rule yet. 
Hax uses a root-parallelized montecarlo tree search with a two-bridge heuristic.
Currently moves are output as a 1-dimensional index, starting from the top-left of the board and finishing at the bottom-right. E.g. the first row of an 11x11 board has indicies 0 through 10, the next row has indices 11 through 21, and so on.

### Why
Hax was motivated by my passion for AI and desire to learn more about C++.
