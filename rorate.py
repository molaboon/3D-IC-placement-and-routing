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

def find_max_weight_path(weight_matrix, start, end):
    # 確保輸入是一個方陣
    n = len(weight_matrix)
    if n != len(weight_matrix[0]):
        raise ValueError("輸入必須是 n*n 的方陣")
    
    # 轉換為 NumPy 陣列
    weight_matrix = np.array(weight_matrix, dtype=float)
    # 將 0 轉為 -inf 表示無邊
    weight_matrix[weight_matrix == 0] = -np.inf
    
    # Dijkstra 演算法 - 最大權重路徑
    def max_weight_dijkstra(start, end):
        weights = np.full(n, -np.inf)  # 到每個節點的最大權重
        weights[start] = 0
        paths = {i: [start] for i in range(n)}  # 記錄路徑
        pq = [(-0, start)]  # 優先隊列，負值模擬最大堆
        visited = np.zeros(n, dtype=bool)
        
        while pq:
            curr_weight, curr = heappop(pq)
            curr_weight = -curr_weight
            
            if visited[curr]:
                continue
            visited[curr] = True
            
            if curr == end:
                return curr_weight, paths[curr]
            
            # 遍歷鄰居
            for neighbor in range(n):
                if weight_matrix[curr, neighbor] > -np.inf:
                    new_weight = curr_weight + weight_matrix[curr, neighbor]
                    if new_weight > weights[neighbor]:
                        weights[neighbor] = new_weight
                        paths[neighbor] = paths[curr] + [neighbor]
                        heappush(pq, (-new_weight, neighbor))
        
        raise ValueError("無法從起點到達終點")
    
    max_weight, path = max_weight_dijkstra(start, end)
    print(path)

def build_max_spanning_tree_bfs_sequence(weight_matrix):
    # 確保輸入是一個方陣
    n = len(weight_matrix)
    if n != len(weight_matrix[0]):
        raise ValueError("輸入必須是 n*n 的方陣")
    
    # 轉換為 NumPy 陣列
    weight_matrix = np.array(weight_matrix, dtype=float)
    weight_matrix[weight_matrix == 0] = -np.inf
    
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
    
    # 使用 BFS 從最大邊的兩個節點開始生成序列
    def bfs_generate_sequence(start):
        sequence = []
        visited = set()
        queue = deque([start])
        visited.add(start)
        
        while queue:
            node = queue.popleft()
            sequence.append(node)
            # 按節點編號排序鄰居以確保一致性
            neighbors = sorted([neighbor for neighbor, _ in adj_list[node] if neighbor not in visited])
            for neighbor in neighbors:
                queue.append(neighbor)
                visited.add(neighbor)
        
        return sequence
    
    # 從最大邊的兩個節點分別開始，選擇較長的序列
    start_node1, start_node2 = max_edge
    sequence1 = bfs_generate_sequence(start_node1)
    sequence2 = bfs_generate_sequence(start_node2)
    
    # 選擇包含更多節點的序列（通常應該相同，因為是連通圖）
    sequence = sequence1 if len(sequence1) >= len(sequence2) else sequence2
    
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
        plt.show()
    
    draw_graph(weight_matrix, mst_edges, sequence)
    return sequence, max_edge, max_weight

def turn2graph():
    newnets = open("{}.nets".format(name), "r")
    grade = [[0 for x in range(8)] for y in range(8)] 

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

# turn2num()
turn2graph()
        
