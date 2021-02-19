import rasterio
import os
import numpy as np


def generate_changes_image(input_directory, output_directory, file1, file2, colorized=0):
    one = rasterio.open(input_directory +"/"+ file1)
    two = rasterio.open(input_directory + "/" + file2)
    band = one.read(1)
    band2 = two.read(1)

    a = np.ndarray([one.height, one.width])
    a.shape = (one.height, one.width)

    row_count = 0
    for row, row2 in zip(band, band2):
        cell_count = 0
        for cell, cell2 in zip(row, row2):
            a[row_count, cell_count] = 10
            if cell2 > 0 and cell2 < 9:
                a[row_count, cell_count] = 12
            if cell != cell2:
                if (cell != 0 and cell2 !=255) and (cell!=255 and cell2!= 0):
                    a[row_count, cell_count] = 255
                    if colorized:
                        a[row_count, cell_count] = cell2
            cell_count += 1
        row_count += 1

    a = a.astype(np.uint8)
    ras_meta = one.profile
    ras_meta['transform'] = None
    f, f2 = str(file1).split("."), str(file2).split(".")
    name = f[1] + "_to_" + f2[1]
    if colorized:
        name += "_colorized"
    os.chdir(output_directory)
    n = os.path.join('change_maps', name + '.gif')
    new = rasterio.open(n, 'w', **ras_meta)
    new.write(a, 1)
    new.write_colormap(
        1, {
            #hard coded in based on colors and data in scenario file
            0: (0, 0, 0, 255),
            1: (139, 35, 35, 255), #red urban
            2: (255, 236, 139, 255), #yellow agric
            3: (238, 154, 73, 255), #orange range
            4: (0, 100, 0, 255), #green forest
            5: (16, 78, 139, 255), #blue water
            6: (72, 61, 139), #blue wetland
            7: (238, 197, 145), #tan barren
            8: (50, 50, 50), #black? tundra
            9: (255, 255, 255, 255), #white snow/ice
            11: (255,255,255,100), #white
            10: (0,0,255,255), # blue
            12: (240, 240, 240), #grey
            255: (0, 0, 0, 100) # black
        }
    )
    new.close()


if __name__=="__main__":
    files = []
    file_num = 0
    
    if not os.path.exists('change_maps'):
        os.makedirs('change_maps')
    
    current_directory = os.getcwd()
    for file in os.listdir(current_directory + '/Output'):
        if file.endswith(".gif") and (str(file).split(".")[1] != 'gif'):
            files.append(file)
            file_num += 1
    files.sort()
    for i in range (0, file_num-1):
        b = i+1
        generate_changes_image(os.getcwd() + '/Output', files[i], files[b])
    generate_changes_image(os.getcwd() + '/Output', files[0], files[file_num - 1])
    generate_changes_image(os.getcwd() + '/Output', files[0], files[file_num - 1], 1)

