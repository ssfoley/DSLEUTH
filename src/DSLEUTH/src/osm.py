# Read the SLEUTH output control_stats.log file and compute OSM, then sort them in descending order.
# Author: Annika Wille
# Date: November 13, 2019

class OSM:

    allData = []
    header1 = ""
    header2 = ""
    import operator

    def read_data(theFile):
        header1 = theFile.readline()
        header2 = theFile.readline()
        line = theFile.readline()
        while line!= '' and line != '\n' and line != ' ':
            vals = line.split()
            results = {}
            results.update({"run":int(vals[0])})
            results.update({"product":float(vals[1])})
            results.update({"compare":float(vals[2])})
            results.update({"pop":float(vals[3])})
            results.update({"edges":float(vals[4])})
            results.update({"clusters":float(vals[5])})
            results.update({"size":float(vals[6])})
            results.update({"leesalee":float(vals[7])})
            results.update({"slope":float(vals[8])})
            results.update({"pc_urban":float(vals[9])})
            results.update({"xmean":float(vals[10])})
            results.update({"ymean":float(vals[11])})
            results.update({"rad":float(vals[12])})
            results.update({"fmatch":float(vals[13])})
            results.update({"diff":int(vals[14])})
            results.update({"brd":int(vals[15])})
            results.update({"sprd":int(vals[16])})
            results.update({"slp":int(vals[17])})
            results.update({"rg":int(vals[18])})
            results.update({"osm":float(0.0)})
            allData.append(results)
            line = myFile.readline()

    def osm():
        for x in allData:
            x["osm"] = x["compare"]*x["pop"]*x["edges"]*x["clusters"]*x["slope"]*x["xmean"]* x["ymean"]

    def sortData():
        allData.sort(key = operator.itemgetter('osm'), reverse = True)

    def top50():
        myFile = open("top50b.log", "w+")
        myFile.write("Top fifty from file: " + ".\control_stats.log")
        myFile.write("\n" + "OSM                Diff Brd Sprd Slp Road\n")
        for s in range(0, 50):
            myFile.write(str(allData[s].get('osm')) + "  ")
            myFile.write(str(allData[s].get('diff')) + "  ")
            myFile.write(str(allData[s].get('brd')) + "  ")
            myFile.write(str(allData[s].get('sprd')) + "  ")
            myFile.write(str(allData[s].get('slp')) + "  ")
            myFile.write(str(allData[s].get('rg')) + "  ")
            myFile.write("\n")
        myFile.close()

    def __init__(self):
        # left off here

if __name__=="__main__":
    filename = "control_stats.log"
    myFile = open(filename, 'r')
    read_data(myFile)
    osm()
    sortData()
    top50()