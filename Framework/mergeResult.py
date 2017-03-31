class MergeResult:
    def merge(self, filePath, numOfFiles, finalPath):
        numOfRun = 0
        with open(finalPath, 'w') as dest:
            for i in range(1, numOfFiles + 1):
                fileName = filePath + '/' + str(i) + '/' + 'control_stats.log'
                with open(fileName) as source:
                    firstLine = next(source)
                    secondLine = next(source)
                    if i == 1:
                        dest.write(firstLine)
                        dest.write(secondLine)
                    for line in source:
                        temp = line.split(None, 1)
                        rest = temp[1]
                        firstWord = '{0:>5}'.format(numOfRun)
                        numOfRun = numOfRun + 1
                        newLine = (firstWord + '  ' + rest).rstrip("\n\r") + '\n'
                        #print newLine
                        dest.write(newLine)
    
#if __name__ == '__main__':
#    m = MergeResult()
#    m.merge('/home/zhang.sheng/test/DSLEUTH/Output/demo200_cal', 2, '/home/zhang.sheng/test/DSLEUTH/Output/demo200_cal/dcontrol.stats.log')
