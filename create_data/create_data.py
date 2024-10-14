import random

file = open("200cell.txt", "a")

cell = 200

for i in range(cell):
    tech = random.randint(1, 100)
    file.write("Inst C" + str(i+1) + " MC" + str(tech) + "\n")

for j in range(cell):
    pin = random.randint(2, 4)
    file.write("Net N" + str(j+1) + " " + str(pin)+"\n")
    
    for p in range(pin):
        c = random.randint(1, cell)
        file.write("Pin C" + str(c) + "/P1\n")

