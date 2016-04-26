#!/usr/bin/env python3

# SI 335 Algorithms, Spring 2016
# Project 3
# Brandon Pullig

import sys

class target:

    def __init__ (self, posx, posy, point):
        self.posx = posx
        self.posy = posy
        self.point = point

class asset:

    def __init__ (self, posx, posy):
        self.posx = posx
        self.posy = posy
        self.goal = ""


def main():
    if len(sys.argv) != 3:
        print ("USAGE: python3 plan.py <mapfile> <movesfile>")
        return 1

    #create graph (adjacency list for double array)
    mapfile = open(str(sys.argv[1]), 'r')
    movesfile = open(str(sys.argv[2]), 'w')

    #read the input file and save all info into lists
    dimensions = mapfile.readline().split()
    matrix = {}
    obstacleList = {}

    #waste read line
    trash = mapfile.readline()
    #read num of obstacles and their positions
    numObstacles = mapfile.readline().rstrip()

    for i in range(0, int(numObstacles)):
        pos = mapfile.readline().rstrip().split()
        obstacleList[",".join(pos)] = 1

    #waste read line
    trash = mapfile.readline()
    #read num of assets and their positions
    numAssets = mapfile.readline().rstrip()
    assetList = [None]*int(numAssets)

    for i in range(0, int(numAssets)):
        pos = mapfile.readline().rstrip().split()
        assetList[i] = asset(pos[0], pos[1])

    #waste read line
    trash = mapfile.readline()
    #read num of targets, the positions, and values
    numTargets = mapfile.readline().rstrip()
    targetList = [None]*int(numTargets)

    for i in range(0, int(numTargets)):
        tinfo = mapfile.readline().split()
        targetList[i] = target(tinfo[0], tinfo[1], tinfo[2])
        name = str(targetList[i].posx)+str(targetList[i].posy)


    #make the adjacency matrix
    makeAdjacencyMatrix(matrix, dimensions, obstacleList)
    adjList = {}
    makeAdjacencyList(adjList, dimensions, obstacleList)

    #get all paths shortest
    FloydWarshall(matrix, dimensions)


    #now I have all shortest paths, adjacency matrix, list of assets and posi, and list of targets and posi
    #obstacles are not a problem as their existence is dealt with in makeAdjacencyMatrix()
    #start meat of the program

    #find the next targets to be got
    #while targets exist
    while len(targetList) != 0:

        for j in range(0, len(assetList)):

            closestTargets = []*len(targetList)
            for i in range(0, len(targetList)):

                tposi = str(targetList[i].posx)+","+str(targetList[i].posy)
                aposi = str(assetList[j].posx) +","+ str(assetList[j].posy)

                #distance object will be a list; list[0] is posi of target and list[1] is distance away from FW
                distance = [1,1,1]
                distance[0] = tposi
                distance[1] = int(matrix[aposi][tposi])
                distance[2] = i

                closestTargets.append(distance)

            #sort to find the closest target to asset
            closestTargets.sort(key=lambda x: x[1])

            #set endpoint for the asset
            assetList[j].goal = closestTargets[0][0]

            #preform BFS to find fastest path
            path = bfs(adjList, str(assetList[j].posx) +","+str(assetList[j].posy), assetList[j].goal)

            #determine what direction to move
            orig = path[0].split(",")
            step = path[1].split(",")
            if int(step[0]) < int(orig[0]):
                movesfile.write("U")
                assetList[j].posx = int(assetList[j].posx) - 1
            elif int(step[0]) > int(orig[0]):
                movesfile.write("D")
                assetList[j].posx = int(assetList[j].posx) + 1
            elif int(step[1]) < int(orig[1]):
                movesfile.write("L")
                assetList[j].posy = int(assetList[j].posy) - 1
            elif int(step[1]) > int(orig[1]):
                movesfile.write("R")
                assetList[j].posy = int(assetList[j].posy) + 1

            #check if at the target
            if assetList[j].goal == (str(assetList[j].posx) +","+ str(assetList[j].posy)):
                targetList.pop(closestTargets[0][2])
                if len(targetList) == 0:
                    movesfile.close()
                    return

        #check if points are dead
        for i in range(0, len(targetList)):
            targetList[i].point = int(targetList[i].point) - 1
            if int(targetList[i].point) == 0:
                targetList.pop(i)
                if len(targetList) == 0:
                    movesfile.close()
                    return

        movesfile.write("\n")

    movesfile.close()
    return

#code cited in readme.txt
def bfs(graph, start, end):
    # maintain a queue of paths
    queue = []
    # push the first path into the queue
    queue.append([start])
    while queue:
        # get the first path from the queue
        path = queue.pop(0)
        # get the last node from the path
        node = path[-1]
        # path found
        if node == end:
            return path
        # enumerate all adjacent nodes, construct a new path and push it into the queue
        for adjacent in graph.get(node, []):
            new_path = list(path)
            new_path.append(adjacent)
            queue.append(new_path)

#make an adjacency matrix to be used with floyd warshall
def makeAdjacencyMatrix(matrix, dimensions, obstacleList):
    for x in range (0, int(dimensions[0])):
        for y in range (0, int(dimensions[1])):

            name = [None]*2
            name[0] = str(x)
            name[1] = str(y)
            matrix[",".join(name)] = {}

            #insert the connections for the grid point above
            #name[0] = x & name[1] = y
            for x1 in range (0, int(dimensions[0])):
                for y1 in range (0, int(dimensions[1])):

                    name1 = [None]*2
                    name1[0] = str(x1)
                    name1[1] = str(y1)
                    if name[0] == name1[0] and ((int(name1[1])==int(name[1])-1) or (int(name1[1])==int(name[1])+1)):
                        matrix[",".join(name)][",".join(name1)] = 1
                    elif name[1] == name1[1] and ((int(name1[0])==int(name[0])-1) or (int(name1[0])==int(name[0])+1)):
                        matrix[",".join(name)][",".join(name1)] = 1
                    else:
                        matrix[",".join(name)][",".join(name1)] = float("inf")
                    if ",".join(name1) in obstacleList or ",".join(name) in obstacleList == 1:
                        matrix[",".join(name)][",".join(name1)] = float("inf")

#make an adjaceny list
def makeAdjacencyList(adjList, dimensions, obstacleList):
    for k in range(0, int(dimensions[0])):
        for h in range(0, int(dimensions[1])):
            name = str(k)+","+str(h)
            adjList[name] = []
            for i in range(0, int(dimensions[0])):
                for l in range(0, int(dimensions[1])):
                    name1 = str(i)+","+str(l)
                    if name1 in obstacleList:
                        continue
                    elif int(k) == int(i) and ((int(l)==int(h)-1) or (int(l)==int(h)+1)):
                        adjList[name].append(name1)
                    elif int(h) == int(l) and ((int(i)==int(k)-1) or (int(i)==int(k)+1)):
                        adjList[name].append(name1)


def FloydWarshall(mcopy, dimensions):
    #Floyd Warshall Code taken from SI335 notes page
    '''Calculates EVERY shortest path length between any two vertices
       in the original adjacency matrix graph.'''
    for k in range(0, int(dimensions[0])):
        for h in range(0, int(dimensions[1])):
            n1 = str(k)+","+str(h)
            for i in range(0, int(dimensions[0])):
                for l in range(0, int(dimensions[1])):
                    n2 = str(i)+","+str(l)
                    for j in range(0, int(dimensions[0])):
                        for g in range(0, int(dimensions[1])):
                            n3 = str(j)+","+str(g)

                            mcopy[n2][n3] = min(mcopy[n2][n3], mcopy[n2][n1] + mcopy[n1][n3])

    return mcopy

if __name__ == '__main__':
    main()
