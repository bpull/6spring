import statistics

def maxval(listonums):
    maxnum = 0
    listonums = map(int, listonums)
    for i in listonums:
        if i > maxnum:
            maxnum = i
            
    return (str(maxnum))
    
def minval(listonums):
    minnum = 1001
    listonums = map(int, listonums)
    for i in listonums:
        if i < minnum:
            minnum = i
            
    return (str(minnum))
    
def aveval(listonums):
    summ = 0
    listonums = map(int, listonums)
    for i in listonums:
        summ = summ + i
        
    avenum = summ / 100
    return (str(avenum))
    
def medval(listonums):
    listonums = map(int, listonums)
    mednum = statistics.median(listonums)
    return (str(mednum))
    
with open('numbers.txt') as fil:
    listonums = fil.read()
    listonums = listonums.split()
    f = open("functions.txt", "w")
    f.write(maxval(listonums) + "\n")  
    f.write(minval(listonums) + "\n")
    f.write(aveval(listonums) + "\n")
    f.write(medval(listonums))
