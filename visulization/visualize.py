import matplotlib.pyplot as plt
import matplotlib.patches as patches
from moviepy import ImageSequenceClip


class Instance:
    def __init__(self, name):
        self.name = name
        self.x = 0
        self.y = 0
        self.z = 0

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
        #  linestyle = style
      )
   )

def plot_result(instances, iter, die_width, die_height, plot_block_area, die):

    width = die_width
    height = die_height
    fig, ax = plt.subplots(1, 1, figsize = (20,20))
    ax.plot( width, height, 'g.')
    ax.plot(0, 0, 'g.')

    # ax[1].plot( width, height, 'g.')
    # ax[1].plot(0, 0, 'g.')

    for instance in instances:
        if instance.z == die:
            plot_rectangle(ax, (instance.x), (instance.y), instance.width, instance.height, 'k', '-', 0)
            # ax.annotate("{}".format(instance.name), xy=(instance.x + 0.5*instance.width, instance.y + 0.5*instance.height),fontsize=12, ha='center', va='center_baseline')

    ax.grid(True, alpha = 0.3)
    # ax[1].grid(True, alpha = 0.3)

    if die == 1:
        ax.set_title("Top Die Placement", fontsize = 24)
    else:
        ax.set_title("Bottom Die Placement", fontsize = 24)

    path = 'visulization/graph/iter' + str(iter) 
    plt.savefig( path + '.jpg')
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
    instances = []

    with open(filename, 'r') as file:
        tech = 0
        for line in file:
            parts = line.split()
            if parts[0] == 'TopDiePlacement':
                tech = 1
                
            elif parts[0] == 'BottomDiePlacement':
                tech = 0
            
            elif parts[0] == 'DieInfo':
                die_width = int( parts[1] )
                die_height = int( parts[2] )
            
            elif parts[0] == 'Inst':
                instances.append(Instance(parts[1]))
                instances[-1].x = int( parts[2] )
                instances[-1].y = int( parts[3] )
                instances[-1].z = tech
                instances[-1].width = int( parts[4] )
                instances[-1].height = int( parts[5] )
    
    return instances, die_width, die_height

def gif(iter):
    
    image_files = []
    for i in range(iter):
        image_files.append('visulization/graph/iter{}.jpg'.format(i))

    # 創建圖片序列剪輯
    clip = ImageSequenceClip(image_files, fps=10)  # fps 表示每秒幀數，可以根據需要調整

    # 將剪輯寫入文件
    clip.write_videofile('output_video.mp4', codec='libx264')

def main():
    iteration = 59
    for i in range(iteration):       
        instances, die_width, die_height = read_data("visulization/data/{}.txt".format(i))
        plot_result(instances, i, die_width, die_height, 0, 0)
        # plot_result(instances, " top", die_width, die_height, 0, 1)
    gif(iteration)

main()

                