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
        self.points = 0;

	# def __str__(self):
	# 	return str(self.job_id) + " " + str(self.arrival) + " " + \
	# 	str(self.service) + " " + str(self.priority)



def main():
    if len(sys.argv) != 3:
        print ("USAGE: python3 plan.py <mapfile> <movesfile>")
        return 1;

    #create graph (adjacency list for double array)
    mapfile = open(str(sys.argv[1]), 'r')
    movesfile = open(str(sys.argv[2]), 'w')

    #read the input file and save all info into lists
    dimensions = mapfile.readline().split()
    matrix = {}
    gridTable = {}

    #make the 2D grid
    for x in range (0, int(dimensions[0])):
        for y in range (0, int(dimensions[1])):

            name = str(x)+str(y)
            gridTable[name] = grid(0,0,0)

    #waste read line
    trash = mapfile.readline()
    #read num of obstacles and their positions
    numObstacles = mapfile.readline().rstrip()
    obstacleList = [None]*int(numObstacles)

    for i in range(0, len(obstacleList)):
        obstacleList[i] = mapfile.readline().replace(" ","").rstrip()
        spot = obstacleList[i]
        gridTable[spot].obstacle = 1

    #waste read line
    trash = mapfile.readline()
    #read num of assets and their positions
    numAssets = mapfile.readline().rstrip()
    assetList = [None]*int(numAssets)

    for i in range(0, len(assetList)):
        assetList[i] = mapfile.readline().replace(" ","").rstrip()
        gridTable[assetList[i]].asset = 1

    #waste read line
    trash = mapfile.readline()
    #read num of targets, the positions, and values
    numTargets = mapfile.readline().rstrip()
    targetList = []*int(numTargets)

    for i in range(0, len(targetList)):
        targetList[i] = mapfile.readline().split()
        name = str(targetList[i][0])+str(targetList[i][1])
        gridTable[name].target = 1
        gridTable[name].points = targetList[2]


    #make the adjacency matrix
    for x in range (0, int(dimensions[0])):
        for y in range (0, int(dimensions[1])):

            name = str(x)+str(y)
            matrix[name] = {}


            #insert the connections for the grid point above
            #name[0] = x & name[1] = y
            for x1 in range (0, int(dimensions[0])):
                for y1 in range (0, int(dimensions[1])):

                    name1 = str(x1)+str(y1)

                    if (int(name1[1]) == int(name[1])-1 or int(name1[1]) == int(name[1])+1) and int(name[0])==int(name1[0]):
                        matrix[name][name1] = 1

                    elif (int(name1[0]) == int(name[0])-1 or int(name1[0]) == int(name[0])+1) and int(name[1])==int(name1[1]):
                        matrix[name][name1] = 1

                    else:
                        matrix[name][name1] = 0

                    if gridTable[name1].obstacle == 1 or gridTable[name].obstacle == 1:
                        matrix[name][name1] = 0

    for x in range (0, int(dimensions[0])*int(dimensions[1])*2):
        print("-", end="")
    print("")
    for x in range (0, int(dimensions[0])):
        for y in range (0, int(dimensions[1])):

            print ("|", end="")
            for x1 in range (0, int(dimensions[0])):
                for y1 in range (0, int(dimensions[1])):

                    name = str(x)+str(y)
                    name1 = str(x1)+str(y1)

                    print (str(matrix[name][name1])+"|", end="")

            print ("")

    for x in range (0, int(dimensions[0])*int(dimensions[1])*2):
        print("-", end="")
    print("")

    
if __name__ == '__main__':
    main()
