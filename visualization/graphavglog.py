import matplotlib.pyplot as plt
import numpy as np
import os
import random

files = []
header = ""

#reading through the avg.log file and storing data values in a list of dictionaries
def read_data(the_file):
    all_data = []
    header = the_file.readline()
    line = the_file.readline()
    while line!= '' and line != '\n' and line != ' ':
        vals = line.split()
        #create a dictionary for each line of data in the log file
        results = {}
        results.update({"run":int(vals[0])})
        results.update({"year":int(vals[1])})
        results.update({"index":int(vals[2])})
        results.update({"sng":float(vals[3])})
        results.update({"sdg":float(vals[4])})
        results.update({"sdc":float(vals[5])})
        results.update({"og":float(vals[6])})
        results.update({"rt":float(vals[7])})
        results.update({"pop":float(vals[8])})
        results.update({"area":float(vals[9])})
        results.update({"edges":float(vals[10])})
        results.update({"clusters":float(vals[11])})
        results.update({"xmean":float(vals[12])})
        results.update({"ymean":float(vals[13])})
        results.update({"rad":float(vals[14])})
        results.update({"slope":float(vals[15])})
        results.update({"cl_size":float(vals[16])})
        results.update({"diffus":float(vals[17])})
        results.update({"spread":float(vals[18])})
        results.update({"breed":float(vals[19])})
        results.update({"slp_res":float(vals[20])})
        results.update({"rd_grav":float(vals[21])})
        results.update({"urban":float(vals[22])})
        results.update({"road":float(vals[23])})
        results.update({"grw_rate":float(vals[24])})
        results.update({"leesalee":float(vals[25])})
        results.update({"grw_pix":float(vals[26])})
        all_data.append(results)
        line = the_file.readline()
    return all_data

#generate a bar graph of whatever dictionary values you want to study 
def growth_graph(graphtype, file_data):
    a = 0
    years = []
    file_num = len(file_data)
    file_vals = [[] for i in range(file_num)]
    name = graphtype[0].capitalize() + graphtype[1:]

    for i in file_data:
        values = []
        num_of_years = 0
        for counter in i:
            num_of_years += 1
            years.append(counter["year"])
            values.append(counter[graphtype])
        file_vals[a] = values
        a += 1

    plt.figure(figsize = (10.0, 5.0))
    #plt.subplot(111)
    index = np.arange(num_of_years)
    bar_pos = 0
    b = 0

    for f in files:
        bar_index = index + bar_pos
        plt.bar(bar_index, file_vals[b], width = 0.2, label = f, color = ((b*0.1), (b*0.2), (b*0.3), 1.0), align='edge')
        bar_pos += 0.2
        b+=1
    
    print(b)
    print(bar_pos)
    
    # offset = 0
    # if b != 1:
    offset = (bar_pos/2)
    print(offset)
    plt.xlabel('Years', fontsize = 10)
    plt.legend()
    plt.xticks(index + offset, years, fontsize = 7, rotation= 0)
    plt.title(name + " Growth")
    plt.savefig(os.path.join('results', name + '.png'))
        

if __name__=="__main__":
    file_num = 0
    
    if not os.path.exists('results'):
        os.makedirs('results')
    
    current_directory = os.getcwd()
    for file in os.listdir(current_directory + '/input'):
        if file.endswith(".log"):
            files.append(file)
            file_num += 1
    
    file_data = [[] for i in range(file_num)]
    
    file_num = 0
    for i in files:
        my_file = open(current_directory + '/input/' + i, 'r')
        file_data[file_num] = (read_data(my_file))
        file_num += 1
        
    growth_graph("sdg", file_data)
    growth_graph("sng", file_data)
    growth_graph("og", file_data)
    growth_graph("rt", file_data)