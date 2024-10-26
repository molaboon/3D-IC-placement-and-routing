import matplotlib.pyplot as plt
import matplotlib.patches as patches


class Instance:
    def __init__(self, name):
        self.name = name
        self.x = 0
        self.y = 0
        self.z = 0.5

        self.width = 0
        self.height = 0

        self.inflate_width = 0
        self.inflate_height = 0
        self.rotate = 0

def plot_rectangle(ax, x1, y1, x2, y2, c, style, if_fill):
   ax.add_patch(
      patches.Rectangle(
         (x1,y1),
         (x2),
         (y2),
         fill = if_fill,
         color = c,
         linestyle = style
      )
   )

def  plot_result(instances, iter, die_width, die_height, grid_info, info, plot_block_area):

    width = die_width
    height = die_height
    fig, ax = plt.subplots(2,1, figsize = (20,20))
    ax[0].plot( width, height, 'g.')
    ax[0].plot(0, 0, 'g.')

    ax[1].plot( width, height, 'g.')
    ax[1].plot(0, 0, 'g.')

    if plot_block_area:
        for y in range(grid_info[5]):
        
            for x in range(grid_info[4]):
                blok = [15, 18, 26, 31, 38, 42, 47, 50, 58, 64, 69, 72 ,79 , 83, 87, 92, 96]
                a = y*grid_info[4] + x
                
                if a in blok:
                    plot_rectangle(ax[0], x * grid_info[2] , (grid_info[1]-grid_info[3]) - y * grid_info[3], grid_info[2], grid_info[3],'r', '-.', 1)
                    plot_rectangle(ax[1], x * grid_info[2] , (grid_info[1]-grid_info[3]) - y * grid_info[3], grid_info[2], grid_info[3],'r', '-.', 1)    

    for instance in instances:
        if instance.z == 1:
            plot_rectangle(ax[1], (instance.x - 0.5*instance.width), (grid_info[1])-(instance.y + 0.5*instance.height), instance.width, instance.height, 'k', '-', 0)
            
            ax[1].annotate("{}".format(instance.name), xy=(instance.x, grid_info[1]-instance.y),fontsize=12, ha='center', va='center_baseline')

        else:
            plot_rectangle(ax[0], (instance.x - 0.5*instance.width), (grid_info[1])-(instance.y + 0.5*instance.height), instance.width, instance.height, 'k', '-', 0)
            
            ax[0].annotate("{}".format(instance.name ), xy=(instance.x, grid_info[1]-instance.y),fontsize=12, ha='center', va='center_baseline')

    ax[0].grid(True, alpha = 0.3)
    ax[1].grid(True, alpha = 0.3)

    ax[0].set_title("Top Die Placement", fontsize = 24)
    ax[1].set_title("Bottom Die Placement", fontsize = 24)

    path = 'graph\\iter ' + str(iter) 
    plt.savefig(path + '.jpg')
    plt.close()

    
    #write_cell_coordinate(instances, path + '.txt', info)
    #write_cell_info(instances, path + ' info.txt')


def plot_penalty(bins, grid_info, iteration):
   width = grid_info[0]
   height = grid_info[1]
   
   fig, ax = plt.subplots(2,1, figsize = (20,20))
   ax[0].plot( width, height, 'g.')
   ax[0].plot(0, 0, 'g.')

   ax[1].plot( width, height, 'g.')
   ax[1].plot(0, 0, 'g.')

   for j in range(grid_info[5]):

      for i in range(grid_info[4]):
         ax[1].annotate("{}".format( round( bins[0][ grid_info[4] * j + i], 0) ), xy = ( int(i * grid_info[2] + grid_info[2]/2), int( (grid_info[5] - j ) * grid_info[3] - grid_info[3]/2)), fontsize=16, ha='center', va='center_baseline')
         ax[0].annotate("{}".format( round( bins[1][ grid_info[4] * j + i], 0) ), xy = ( int(i * grid_info[2] + grid_info[2]/2), int( (grid_info[5] - j ) * grid_info[3] - grid_info[3]/2)), fontsize=16, ha='center', va='center_baseline')

  
   ax[0].grid(True, alpha = 0.3) 
   ax[1].grid(True, alpha = 0.3)
   ax[0].set_title("Top Die Penalty", fontsize = 24)
   ax[1].set_title("Bottom Die Penalty", fontsize = 24)
   
   path = 'result\\penalty ' + str(iteration) 
   plt.savefig(path + '.svg' )
   
   plt.close()

def write_cell_coordinate(instances, path, plcmt_info):
   
   f = open(path, "w")
   # a = (plcmt_info[1] + plcmt_info[3]*10 + plcmt_info[4]/plcmt_info[5])
   
   f.write("score = {}, \n".format(plcmt_info[0]))
   # f.write("normalize score: {}\n\n".format(  ) )
   f.write("wire length = {}, actual wire length : {}\n".format(plcmt_info[1], plcmt_info[2]))
   f.write( "# of TSV : {}, density score : {}\n".format(plcmt_info[3], plcmt_info[4]) )
   f.write( "penalty weight : {}\n\n".format(plcmt_info[5]) )
   f.write("tmp_CG : {}\n\n".format(plcmt_info[6]))
   f.write("last cg: {}\n\n".format(plcmt_info[7]))
   
   
   for i in instances:
      
      f.write("{} {} {} {}\n".format(i.name, i.x, i.y, i.z))
   
   f.close()

def read_data(filename):
    die_width = 0
    die_height = 0
    instances = [[], []]

    with open(filename, 'r') as file:
        tech = 0
        for line in file:
            parts = line.split()
            if parts[0] == 'TopDiePlacement':
                tech = 0
                
            elif parts[0] == 'BottomDiePlacement':
                tech = 1
            
            elif parts[0] == 'DieInfo':
                die_width = parts[1]
                die_height = parts[2]
            
            elif parts[0] == 'Inst':
                instances[tech].append(Instance(parts[1]))
                instances[tech][-1].x = int( parts[2] )
                instances[tech][-1].y = int( parts[3] )
                instances[tech][-1].z = tech
                instances[tech][-1].width = int( parts[4] )
                instances[tech][-1].height = int( parts[5] )
    
    return instances, die_width, die_height

def main():    
    inatances, die_width, die_height = read_data("data/0.txt")
    

main()

                