import random

file = open("100cell_case.txt", "a")

cell = 100

# for i in range(cell):
#     tech = random.randint(1, 217)
#     file.write("Inst C" + str(i+1) + " MC" + str(tech) + "\n")

for j in range(cell):
    pin = random.randint(2, 3)
    file.write("Net N" + str(j+1) + " " + str(pin)+"\n")
    
    for p in range(pin):
        c = random.randint(1, 100)
        file.write("Pin C" + str(c) + "/P1\n")

