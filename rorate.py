import copy
from heapq import heappush, heappop
import matplotlib.pyplot as plt
import numpy as np
import networkx as nx
from collections import deque
name = "apte"

def turn2net():
    f = open("./input_pa2/{}.nets".format(name), "r")
    o = open("{}.net".format(name), "w")

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
            if(line[0] != "c"):
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
    block = open("./input_pa2/{}.block".format(name), "r")
    nets = open("{}.net".format(name), "r")
    newblok = open("{}.block".format(name), "w")
    newnets = open("{}.nets".format(name), "w")
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
        newblok.write("cc_{} {} {}\n".format(i+1, width[i], height[i]))
    
    line = nets.readline()
    
    while line:
        if(line[0] == "c"):
            line = line.split("\n")
            for i in range(len(blocks)):
                if line[0] == blocks[i]:
                    newnets.write("cc_{}\n".format(i+1))
        else:
            newnets.write(line)
        
        line = nets.readline()

def build_max_spanning_tree_bfs_sequence(weight_matrix):
    # 確保輸入是一個方陣
    n = len(weight_matrix)
    if n != len(weight_matrix[0]):
        raise ValueError("輸入必須是 n*n 的方陣")
    
    # 轉換為 NumPy 陣列
    weight_matrix = np.array(weight_matrix, dtype=float)
    weight_matrix[weight_matrix == 0] = -np.inf
    # print(weight_matrix)
    
    # Kruskal 演算法 - 最大生成樹
    def kruskal_max_spanning_tree():
        edges = []
        for i in range(n):
            for j in range(i + 1, n):
                if weight_matrix[i, j] > -np.inf:
                    edges.append((weight_matrix[i, j], i, j))
        edges.sort(reverse=True)  # 按權重降序
        
        parent = list(range(n))
        rank = [0] * n
        
        def find(x):
            if parent[x] != x:
                parent[x] = find(parent[x])
            return parent[x]
        
        def union(x, y):
            px, py = find(x), find(y)
            if px == py:
                return False
            if rank[px] < rank[py]:
                px, py = py, px
            parent[py] = px
            if rank[px] == rank[py]:
                rank[px] += 1
            return True
        
        mst_edges = []
        max_edge = None
        max_weight = -np.inf
        
        for weight, u, v in edges:
            if union(u, v):
                mst_edges.append((u, v, weight))
                if weight > max_weight:
                    max_weight = weight
                    max_edge = (u, v)
        
        return mst_edges, max_edge, max_weight
    
    # 建立最大生成樹
    mst_edges, max_edge, max_weight = kruskal_max_spanning_tree()
    
    # 從最大生成樹建鄰接表
    adj_list = [[] for _ in range(n)]
    for u, v, weight in mst_edges:
        adj_list[u].append((v, weight))
        adj_list[v].append((u, weight))
    
    def greedy(start):
        sequence = []
        visited = set()
        
        def find_next_node(current):
            # 找出當前節點的所有鄰居中，權重最大的未訪問節點
            neighbors = [(weight_matrix[current, j], j) for j in range(n) 
                        if weight_matrix[current, j] > -np.inf and j not in visited]
            if not neighbors:
                return None, None
            max_weight, next_node = max(neighbors)
            return max_weight, next_node
        
        current = start
        sequence.append(current)
        visited.add(current)
        
        # 遍歷直到當前連通分量無法繼續
        while True:
            max_weight, next_node = find_next_node(current)
            if next_node is None:
                break
            sequence.append(next_node)
            visited.add(next_node)
            current = next_node
        
        # 如果還有未訪問的節點，選擇一個新的起點繼續
        while len(visited) < n:
            # 從未訪問節點中選擇一個（這裡簡單按編號選最早的）
            current = copy.deepcopy(start)
            # 繼續貪婪遍歷
            while True:
                max_weight, next_node = find_next_node(current)
                if next_node is None:
                    break
                sequence.append(next_node)
                visited.add(next_node)
                current = next_node
            break

        return sequence

    # 從最大邊的兩個節點分別開始，選擇較長的序列
    start_node1, start_node2 = max_edge
    
    sequence = greedy(64)
    print(sequence)
    
    def draw_graph(weight_matrix, mst_edges, sequence):
        # 圓形布局
        theta = np.linspace(0, 2 * np.pi, n, endpoint=False)
        radius = 1.5  # 保持邊長較長
        pos = radius * np.column_stack([np.cos(theta), np.sin(theta)])
        
        plt.figure(figsize=(8, 6))
        
        # 繪製節點
        plt.scatter(pos[:, 0], pos[:, 1], s=500, c='lightgray', zorder=2)
        for i in range(n):
            plt.text(pos[i, 0], pos[i, 1], str(i), ha='center', va='center', zorder=3)
        
        # 繪製所有原始邊（灰色）
        for i in range(n):
            for j in range(i + 1, n):
                if weight_matrix[i, j] > -np.inf:
                    x = [pos[i, 0], pos[j, 0]]
                    y = [pos[i, 1], pos[j, 1]]
                    plt.plot(x, y, 'gray', alpha=0.3, zorder=1)
        
        # 繪製最大生成樹邊（藍色）
        for u, v, weight in mst_edges:
            x = [pos[u, 0], pos[v, 0]]
            y = [pos[u, 1], pos[v, 1]]
            plt.plot(x, y, 'blue', linewidth=2, zorder=1)
            mid_x, mid_y = (pos[u, 0] + pos[v, 0]) / 2, (pos[u, 1] + pos[v, 1]) / 2
            plt.text(mid_x, mid_y, f"{weight:.0f}", color='black', zorder=3)
        
        # 高亮序列路徑（紅色虛線）
        for i in range(len(sequence) - 1):
            u, v = sequence[i], sequence[i + 1]
            x = [pos[u, 0], pos[v, 0]]
            y = [pos[u, 1], pos[v, 1]]
            plt.plot(x, y, 'red', linewidth=3, linestyle='--', zorder=1)
        
        plt.title(f"Maximum Spanning Tree with BFS Sequence from Edge {max_edge} (Weight: {max_weight})")
        plt.axis('equal')
        plt.axis('off')
        plt.savefig("aaa.png")
    
    draw_graph(weight_matrix, mst_edges, sequence)
    return sequence, max_edge, max_weight

def turn2graph():
    newnets = open("./floorplan/{}.nets".format(name), "r")
    grade = [[0 for x in range(9)] for y in range(9)] 

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
            line = line.replace("cc_", "")
            arr.append(int(line))

        for j in range(degree):
            fnum = arr[j]
            for k in range(j+1, degree):
                snum = arr[k]
                grade[fnum-1][snum-1] += 1
    
    grade = np.array(grade)
    build_max_spanning_tree_bfs_sequence(grade)

def turn2img(arr):
    print(arr)
    for i in range(len(arr)):
        for j in range(i+1, len(arr)):
            if arr[i][j] > 0:
                if j == 64:
                    print("[{}, pseudo macro] = {}".format(i, arr[i][j]))
                else:
                    print("[{}, {}] = {}".format(i, j, arr[i][j]))


#   turn new blue to case_newblue
def blue2case():
    block = open("newblue1.nodes", "r")
    net = open("newblue1.nets", "r")
    bluecase = open("caseblue.txt", "w")
    
    macro = []
    macro_w = []
    macro_h = []
    pinsinmacro = [[]for y in range( len(macro) + 1 )]
    terminals = []
    
    line = block.readline().replace("\n", "")
    line = block.readline().replace("\n", "")
    line = block.readline().replace("\n", "")
    
    while line:
        line = line.split(" ")        
        if(len(line) < 4):
            if(line[2] != "12"):
                macro.append(line[0])
                macro_w.append( int(line[1]) )
                macro_h.append( int(line[2]) )
        else:
            terminals.append(line[0])
        
        line = block.readline().replace("\n", "")

    pinsinmacro = [[0 for x in range(4)]for y in range( len(macro))]
    grade = [[0 for x in range( len(macro) + 1 )] for y in range( len(macro) + 1 )] 
    
    #------------------------------------------
    #----find pin and in macros-----
    line = net.readline().replace("\n", "")
    line = line.split(" ")
    numnet = int(line[1])
    line = net.readline().replace("\n", "")
    
    for i in range(numnet):
        line = net.readline().replace("\n", "")
        line = line.split(" ")
        degree = int(line[1])
        numter = 0
        numstdcell = 0
        macrosInNet = []
        
        for j in range(degree):
            line = net.readline().replace("\n", "")
            line = line.split(" ")
            
            if line[0] in macro:
                x = int(line[1])
                y = int(line[2])

                if x < 0 and y < 0:
                    pinsinmacro[macro.index(line[0])][0] += 1
                elif x > 0 and y < 0:
                    pinsinmacro[macro.index(line[0])][1] += 1
                elif x < 0 and y > 0:
                    pinsinmacro[macro.index(line[0])][2] += 1
                else:
                    pinsinmacro[macro.index(line[0])][2] += 1
                    
            if line[0] in macro and macro.index(line[0]) not in macrosInNet:
                macrosInNet.append( macro.index(line[0]) )
            
            elif line[0] in terminals:
                numter += 1
            else:
                numstdcell +=1

        if degree - numter > 1 and len(macrosInNet) > 0:
            macrosInNet.append( len(macro) )
            macrosInNet = sorted(macrosInNet)
            for j in range( len(macrosInNet) ):
                for k in range(j+1, len(macrosInNet)):
                    grade[ macrosInNet[j] ][ macrosInNet[k] ] += numstdcell + 1
                    grade[ macrosInNet[k] ][ macrosInNet[j] ] += numstdcell + 1

    print(pinsinmacro)
    # turn2img(grade)
    # build_max_spanning_tree_bfs_sequence(grade)
    
    grade = np.array(grade)
    find_path(grade)


def find_path(grade, die):
    
    sequence = []
    visited = set()
    start = die[-1]
    n = len(die)
    
    
    def find_next_node(current):
        # 找出當前節點的所有鄰居中，權重最大的未訪問節點
        neighbors = [(grade[current, j], j) for j in range(n) 
                    if grade[current, j] > 0 and j not in visited and j in die]
        if not neighbors:
            return None, None
        max_weight, next_node = max(neighbors)
        return max_weight, next_node
    
    current = start
    sequence.append(current)
    visited.add(current)
    next_node = 0
            
    while len(visited) < n:
        while True:
            max_weight, next_node = find_next_node(current)
            if next_node is None:
                break
            sequence.append(next_node)
            visited.add(next_node)
            current = next_node
        
        if next_node is None and current == start:
            for i in range(n):
                new_start = die[i]
                if new_start not in visited:
                    sequence.append(new_start)
                    visited.add(new_start)
                    current = new_start
        else:
            current = start

    
    for i in range(len(sequence)):
        print("{}, ".format(sequence[i]), end="")
    print()
    
    # for i in range(len(path), 0, -1):
    #     print("{}, ".format(path[i-1]), end="")

def turn2case():
    block = open("newblue1.nodes", "r")
    bluecase = open("caseblue.txt", "w")
    
    macro = []
    macro_w = []
    macro_h = []
    terminals = []
    
    line = block.readline().replace("\n", "")
    line = block.readline().replace("\n", "")
    line = block.readline().replace("\n", "")
    
    while line:
        line = line.split(" ")        
        if(len(line) < 4):
            if(line[2] != "12"):
                macro.append(line[0])
                macro_w.append( int(line[1]) )
                macro_h.append( int(line[2]) )
        else:
            terminals.append(line[0])
        
        line = block.readline().replace("\n", "")

    bluecase.write("NumTechnologies 1\n")
    bluecase.write("Tech TA {}\n".format(len(macro)))
    
    for i in range(len(macro)):
        bluecase.write("LibCell Y MC{} {} {} 1\n".format( i+1, macro_w[i], macro_h[i] ))
        bluecase.write("Pin P1 0 0\n")
    bluecase.write("\n")
    bluecase.write("DieSize 0 0 40 30\n\n")
    bluecase.write("TopDieMaxUtil 80\nBottomDieMaxUtil 90\n\n")
    bluecase.write("TopDieRows 0 0 40 10 3\nBottomDieRows 0 0 40 15 2\n\n")
    bluecase.write("TopDieTech TA\nBottomDieTech TB\n\n")
    bluecase.write("TerminalSize 6 6\nTerminalSpacing 5\nTerminalCost 10\n\n")
    bluecase.write("NumInstances {}\n".format(len(macro)))
    
    for i in range(len(macro)):
        bluecase.write("Inst C{} MC{}\n".format( i+1, i+1 ))

    bluecase.write("\n")
    bluecase.write("NumNets 1\nNet N1 2\nPin C1/P1\nPin C2/P1")


def kj2case():
    case3 = open("./case/case3.txt", "r")
    kj = open("kjoutput.txt", "r") 
    
    num_tech = 0
    num_pins = 0
    num_cell = 0
    macro = []
    number = []
    top_die = []
    btm_die = []
    
    line = case3.readline().replace("\n", "").split(" ")
    num_tech = int(line[1])
    
    for tech in range(num_tech):
        line = case3.readline().replace("\n", "").split(" ")
        num_cell = int(line[2])
        
        for cell in range(num_cell):
            line = case3.readline().replace("\n", "").split(" ")
            num_pins = int(line[5])
            is_macro = line[1]
            
            if is_macro == "Y":
                macro.append(line[2])
            
            for pin in range(num_pins):
                line = case3.readline().replace("\n", "").split(" ")
    
    for empty in range(16):
        line = case3.readline().replace("\n", "").split(" ")

    line = case3.readline().replace("\n", "").split(" ")
    num_instances = int(line[1])
    
    for ins in range(num_instances):
        line = case3.readline().replace("\n", "").split(" ")
        instance = line[2]
        if instance in macro:
            num = line[1].replace("C", "")
            number.append(num)
    
    for ins in range(num_instances):
        line = kj.readline().replace("\n", "").split(" ")
        
        if line[0] in number and line[1] == "0":
            top_die.append( line[0]) 
        elif line[0] in number and line[1] == "1":
            btm_die.append( line[0])
    
    index_of_top = []
    index_of_btm = []
    
    for i in top_die:
        index_of_top.append( number.index(i) )
        
    for i in btm_die:
        index_of_btm.append( number.index(i) )
    
    # -----calculate the grade of macros-----------------------

    # pinsinmacro = [[0 for x in range(4)]for y in range( len(macro))]
    grade = [[0 for x in range( len(number) + 1 )] for y in range( len(number) + 1 )] 
    
    line = case3.readline().replace("\n", "")
    line = case3.readline().replace("\n", "")
    line = line.split(" ")
    numnet = int(line[1])
    
    for i in range(numnet):
        line = case3.readline().replace("\n", "").split(" ")
        degree = int(line[2])
        numstdcell = 0
        macrosInNet = []
        
        for j in range(degree):
            line = case3.readline().replace("\n", "").split(" ")
            line = line[1].split("/")
            line = line[0].replace("C", "")
                    
            if line in number:
                if number.index(line) not in macrosInNet:
                    macrosInNet.append( number.index(line) )
            else:
                numstdcell +=1

        if degree > 1 and len(macrosInNet) > 0:
            macrosInNet.append( len(number) )
            macrosInNet = sorted(macrosInNet)
            for j in range( len(macrosInNet) ):
                for k in range(j+1, len(macrosInNet)):
                    grade[ macrosInNet[j] ][ macrosInNet[k] ] += numstdcell + 1
                    grade[ macrosInNet[k] ][ macrosInNet[j] ] += numstdcell + 1
    
    #---decide the sequence of the top_die and btm_die

    grade = np.array(grade)
    turn2img(grade)    
    index_of_top.append(len(number))
    index_of_btm.append(len(number))

    # print(index_of_top, index_of_btm)
    find_path(grade, index_of_top)
    find_path(grade, index_of_btm)
            
kj2case()
# blue2case()
# turn2case()