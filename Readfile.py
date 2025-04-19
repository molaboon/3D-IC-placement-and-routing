class LibCell:
    def __init__(self, name, width, height):
        self.name = name
        self.width = width
        self.height = height
        self.inflate_width = 0
        self.inflate_height = 0 
        
class DieSize:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        
class DieRows:
    def __init__(self, left, bottom, right, top, num):
        self.left = left
        self.bottom = bottom
        self.right = right
        self.top = top
        self.num = num

class Terminal:
    def __init__(self, size, spacing):
        self.size = size
        self.spacing = spacing

class Instance:
    def __init__(self, name):
        self.name = name
        self.width = 0
        self.height = 0
        self.inflate_width = 0
        self.inflate_height = 0

class Net:
    def __init__(self, name):
        self.name = name
        self.cells = []       

# Function to parse the data
def parse_data(filename):
    lib_cells = {}
    die_size = []
    top_die_max_util = None
    bottom_die_max_util = None
    top_die_rows = None
    bottom_die_rows = None
    top_die_tech = None
    bottom_die_tech = None
    terminal_size = None
    terminal_spacing = None
    instances = []
    nets = []

    with open(filename, 'r') as file:
        tech = 'A'
        for line in file:
            parts = line.split()
            if len(parts) < 2:
                continue
            if parts[0] == 'Tech':
            
                tech = parts[1]
                # print(tech)
            
            elif parts[0] == 'LibCell':
                
                if tech == "TB":
                    lib_cells[parts[1]].inflate_width = int(parts[2])
                    lib_cells[parts[1]].inflate_height = int(parts[3])

                else:
                    lib_cells[parts[1]] = LibCell(parts[1], int(parts[2]), int(parts[3]))
            
            elif parts[0] == 'Pin':
            
                continue
            
            elif parts[0] == 'DieSize':
            
                die_size.append(int(parts[3]) )
                die_size.append(int(parts[4]))
            
            elif parts[0] == 'TopDieMaxUtil':
            
                top_die_max_util = int(parts[1])
            
            elif parts[0] == 'BottomDieMaxUtil':
            
                bottom_die_max_util = int(parts[1])
            
            elif parts[0] == 'TopDieRows':
            
                top_die_rows = [ int(parts[1]), int(parts[2]), int(parts[3]), int(parts[4]), int(parts[5]) ]
            
            elif parts[0] == 'BottomDieRows':
            
                bottom_die_rows = [ int(parts[1]), int(parts[2]), int(parts[3]), int(parts[4]), int(parts[5]) ]
            
            elif parts[0] == 'TopDieTech':
                top_die_tech = parts[1]
            
            elif parts[0] == 'BottomDieTech':
                bottom_die_tech = parts[1]
            
            elif parts[0] == 'TerminalSize':
                terminal_size = Terminal(int(parts[1]), int(parts[2]))
            
            elif parts[0] == 'TerminalSpacing':
                terminal_spacing = int(parts[1])
            
            elif parts[0] == 'Inst':
                instances.append(Instance(parts[1]))
                instances[-1].width = lib_cells[ parts[2] ].width
                instances[-1].height = lib_cells[ parts[2] ].height
                instances[-1].inflate_width = lib_cells[ parts[2] ].inflate_width
                instances[-1].inflate_height = lib_cells[ parts[2] ].inflate_height
                

            elif parts[0] == 'Net':
                
                current_net = Net(parts[1])
                
                nets.append(current_net)
                
                num_pins = int(parts[2])
                
                for i in range(num_pins):
                    pin_line = next(file).split()
                    net_cell = pin_line[1].split('/')
                
                    for instance in instances:
                        if ( net_cell[0] == instance.name ):
                            current_net.cells.append(instance)
 

    return instances, nets
