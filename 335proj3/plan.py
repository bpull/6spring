#!/usr/bin/env python3

# SI 335 Algorithms, Spring 2016
# Project 3
# Brandon Pullig

import sys

class grid:

    def __init__ (self, obstacle, asset, target):
        self.obstacle = obstacle
        self.asset = asset
        self.target = target
        self.tracked = 0

	# def __str__(self):
	# 	return str(self.job_id) + " " + str(self.arrival) + " " + \
	# 	str(self.service) + " " + str(self.priority)



def main():
    if len(sys.argv) != 3:
        print ("USAGE: python3 plan.py <mapfile> <movesfile>")
        return 1;

    #create graph (adjacency list for double array)
    mapfile = open(str(sys.argv[1]), 'r')
    dimensions = mapfile.readline().split()
    matrix = {}

    numObstacles = mapfile.readline()
    assetList = []

    for i in range(0,   numObstacles):



    for x in range (0, int(dimensions[0])):
        for y in range (0, int(dimensions[1])):

            name = str(x)+str(y)
            #check if x on border
            if x == 0:
                martix[name] = grid()
            if x == int(dimensions[0]) - 1:
                print ("yes")


if __name__ == '__main__':
    main()
