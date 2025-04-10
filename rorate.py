import copy

def turn2net():
    f = open("./input_pa2/ami33.nets", "r")
    o = open("ami33.net", "w")

    line = f.readline().replace("\n", "")

    numnet = line.split(" ")
    numnet = int(numnet[1])
    nextnet = copy.deepcopy(numnet)

    for i in range(numnet):
        line = f.readline().replace("\n", "")
        line = line.split(" ")
        degree = int(line[1])
        nextdegree = copy.deepcopy(degree)
        net = []
        
        for j in range(degree):
            line = f.readline().replace("\n", "")
            if(line[0] != "b"):
                nextdegree-=1
            else:
                net.append(line)
        
        
        if nextdegree > 1:
            o.write("NetDegree: {}\n".format(nextdegree))
            for i in range(nextdegree):
                o.write(net[i]+"\n")
        
        else:
            nextnet -= 1

    o.write("Numdegree : {}\n".format(nextnet))

            
def turn2num():
    block = open("./input_pa2/ami33.block", "r")
    nets = open("ami33.net", "r")
    newblok = open("ami33.block", "w")
    newnets = open("ami33.nets", "w")
    newblocks = []
    
    line = block.readline()
    line = block.readline()
    line = block.readline()
    line = block.readline().replace("\n", "")
    
    blocks = []
    width = []
    height = []
    
    while line:
        line = line.split(" ")
        blocks.append(line[0])
        width.append(line[1])
        height.append(line[2])
        line = block.readline().replace("\n", "")
    
    for i in range(len(blocks)):
        newblok.write("bk{} {} {}\n".format(i+1, width[i], height[i]))
    
    line = nets.readline()
    
    while line:
        if(line[0] == "b"):
            line = line.split("\n")
            for i in range(len(blocks)):
                if line[0] == blocks[i]:
                    newnets.write("bk{}\n".format(i+1))
        else:
            newnets.write(line)
        
        line = nets.readline()

def turn2graph():
    newnets = open("ami33.nets", "r")
    grade = [[0 for x in range(33)] for y in range(33)] 

    line = newnets.readline().replace("\n", "")
    line = line.split(" ")
    numnet = int(line[1])
    
    for i in range(numnet):
        line = newnets.readline().replace("\n", "")
        line = line.split(" ")
        
        degree = int(line[1])
        arr = []
        
        for j in range(degree):
            line = newnets.readline().replace("\n", "")
            line = line.replace("bk", "")
            arr.append(int(line))

        for j in range(degree):
            fnum = arr[j]
            for k in range(j+1, degree):
                snum = arr[k]
                grade[fnum-1][snum-1] += 1
                
    print(grade)

turn2graph()
        


    
        
