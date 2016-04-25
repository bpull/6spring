#!/usr/bin/env python3

# SI 335 Algorithms, Spring 2016
# Project 3
# Brandon Pullig

import sys

def main():
    if len(sys.argv) != 3:
        print ("USAGE: python3 plan.py <mapfile> <movesfile>")
        return 1;

    #create graph (adjacency list for double array)
    mapfile = open(str(sys.argv[1]), 'r')
    dimensions = mapfile.readline().split()

    for x in range (0, int(dimensions[0])):
        for y in range (0, int(dimensions[1])):

            #check if x on border
            if x == 0:
                
            if x == int(dimensions[0]) - 1:



if __name__ == '__main__':
    main()
