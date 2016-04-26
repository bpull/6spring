sources: how to sort by specific element in list
            http://stackoverflow.com/questions/2173797/how-to-sort-2d-array-by-row-in-python
         bfs
            http://stackoverflow.com/questions/8922060/how-to-trace-the-path-in-a-breadth-first-search

Reasoning:

        When first looking at this problem, I say the grid as a graph. I believed it would be most beneficial to represent
        the grid elements as vertices and the four connections (north, east, south, west) as the edges to those boxes around
        every grid. I used both a adjacency list and matrix to accomplish this. The list is better suited to this problem
        because it is a very sparse graph. I however, still made a matrix because I planned on running Floyd Warshall's Algorithm
        to find the shortest path between two points. So by using the FW matrix and the adjacency list, I was able to sort the
        targets by their proximity to each asset and then I ran BFS to actually follow that shortest path. This is definitely
        a greedy algorithm as they (the assets) take the closest target in proximity regardless if there is another asset on its
        way to the same target. This causes unoptimized works so the greedy algorithm is sadly not the best.

Running-time:

        Many of the beginning work can be ignored because it is clear the bigger hit is taken later on in the meat of the running.
        The main loop (given m targets and n vertices) runs in O(n^4) due to the double nested target loops + the BFS search done in the
        middle loops. 
