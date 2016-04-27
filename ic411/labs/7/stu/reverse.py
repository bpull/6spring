f = open("numbers.txt", "r")

listonums = f.read()
listonums = listonums.split()

fi = open("reverse.txt", "w")

for i in listonums[::-1]:
    fi.write(str(i) + " ")