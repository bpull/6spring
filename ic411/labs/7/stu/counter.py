import random
file = open("numbers.txt", "w")

for i in range(0, 100):
    file.write(str(random.randint(1, 1000)) + " ")