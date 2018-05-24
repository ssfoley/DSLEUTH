import Queue
import subprocess
import sys
import scenarioUtil
import time
import os
#import hostlist
#import processAttribute
#import mergeResult

class Main:
	nodelist = {}
	queue = Queue.Queue()
        sched = "unknown"
        num_nodes = 1

        def __init__(self):
                """
                Set the type of environment for this run
                Settings are defined in the run_settings file with the scheduler type and 
                number of nodes
                """
                setfile = open("run_settings", "r")
                self.sched, num = setfile.read().split()
                self.num_nodes = int(num)
                print "scheduler type: ", self.sched
                print "num_nodes: ", str(self.num_nodes)

	
	def getNodelist(self):
                if self.sched is "SLURM":
                        import hostlist
                        slurmlist = hostlist.expand_hostlist(os.environ["SLURM_JOB_NODELIST"])
                        self.nodelist = dict(zip(slurmlist,[-1]*len(slurmlist)))
                else:
                        for n in range(self.num_nodes):
                                self.nodelist.update({"".join(["node", str(n)]): -1})

	def done(self, p):
	 	if p.poll() is not None:
                        # done
                        print "pid just finished ", str(p.pid)
                        # update node list
                        print self.nodelist.values().index(p.pid)
                        print "node just freed ", self.nodelist.keys()[self.nodelist.values().index(p.pid)]
                        self.nodelist[self.nodelist.keys()[self.nodelist.values().index(p.pid)]] = -1
                        # return true
                        return True
                else:
                        print "not done! ", str(p.poll())
                        return False

        def get_free_node(self):
                # assume there is a free node
                return self.nodelist.keys()[self.nodelist.values().index(-1)]

	def success(self, p):
	 	return p.returncode == 0

        def merge(self, filePath, numOfFiles, finalPath):
                numOfRun = 0
                print os.getcwd()
                with open(finalPath, 'w') as dest:
                        for i in range(1, numOfFiles + 1):
                                fileName = filePath + str(i) + '/' + 'control_stats.log'
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

	def main(self):
		args = sys.argv

		#args[3] is the scenario file path
		destination_path = args[3] + "_steps/"
		scena = scenarioUtil.ScenarioUtil()
		print time.strftime("%H:%M:%S")
		if not os.path.exists(destination_path):
			scena.makeOutputDir(destination_path)
		fileNum = scena.generatingBySplitDiffusionAndNum(args[3], destination_path, 9)
		print time.strftime("%H:%M:%S")
		#return

		self.getNodelist()
		#print "nodelist----" + self.nodelist.amount()
		for x in range(1,fileNum):
			self.queue.put(x)
		processes = []
		
		for node in self.nodelist.keys():
			print node
			if not self.queue.empty():
				num = self.queue.get()
                                print "attempting to launch on node: ", node
                                if self.sched is "SLURM":
                                        p = subprocess.Popen(["srun", "-N", "1", "--nodelist=" + node, args[1], args[2], args[3] + "_steps/" + str(num)])
                                else:
                                        p = subprocess.Popen([args[1], args[2], args[3] + "_steps/" + str(num)])
				#pid = p.pid
				#proc = processAttribute.ProcessAttribute(p, pid, node)
				print p.pid
                                self.nodelist[node] = p.pid
				processes.append(p)

		while not self.queue.empty():
                        # check for finished processes
                        #   somelist[:] = [tup for tup in somelist if determine(tup)]
                        processes[:] = [ p for p in processes if not self.done(p) ]

                        # while there are available nodes and the queue is not empty
                        while len(processes) < self.num_nodes and not self.queue.empty():
                                num = self.queue.get()
                                node = self.get_free_node()
                                print "attempting to launch on node: ", node
                                if self.sched is "SLURM":
                                        p = subprocess.Popen(["srun", "-N", "1", "--nodelist=" + node, args[1], args[2], args[3] + "_steps/" + str(num)])
                                else:
                                        p = subprocess.Popen([args[1], args[2], args[3] + "_steps/" + str(num)])
                                print p.pid
                                self.nodelist[node] = p.pid
                                processes.append(p)
                        # wait a little bit and then check again
			time.sleep(1)

		for pro in processes:
			pro.wait()
		print time.strftime("%H:%M:%S")
		
		outputDir = scena.getAttribute("OUTPUT_DIR=",args[3])
		outputDir = outputDir.replace("\n", "")
		self.merge(outputDir, fileNum - 1, outputDir + "/control.stats.log")
if __name__ == '__main__':
	m = Main()
	m.main()
