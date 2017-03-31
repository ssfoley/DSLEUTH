import Queue
import subprocess
import sys
import scenarioUtil
import time
import os
import hostlist
import processAttribute
import mergeResult

class Main:
	nodelist = []
	queue = Queue.Queue()

	def getNodelist(self):
		self.nodelist = hostlist.expand_hostlist(os.environ["SLURM_JOB_NODELIST"])

	def done(self, p):
	 	return p.poll()

	def success(self, p):
	 	return p.returncode == 0

	def main(self):
		args = sys.argv

		#args[3] is the scenario file path
		destination_path = args[3] + "_steps/"
		scena = scenarioUtil.ScenarioUtil()
		if not os.path.exists(destination_path):
			scena.makeOutputDir(destination_path)
		fileNum = scena.generatingBySplitDiffusion(args[3], destination_path)

		self.getNodelist()
		for x in range(1,fileNum):
			self.queue.put(x)
		processes = []
		for node in self.nodelist:
			if not self.queue.empty():
				num = self.queue.get()
				p = subprocess.Popen(["srun", "-N", "1", "--nodelist=" + node, args[1], args[2], args[3] + "_steps/" + str(num)])
				pid = p.pid
				print pid
				proc = processAttribute.ProcessAttribute(p, pid, node)

				processes.append(proc)

		while not self.queue.empty():
			for pro in processes:
				if self.done(pro.popen) == 0 and not self.queue.empty():
					print pro.pid
					print "is done"
					num = self.queue.get()
					pro.popen = subprocess.Popen(["srun", "-N", "1", "--nodelist=" + pro.node, args[1], args[2], args[3] + "_steps/" + str(num)])
					pro.pid = p.pid

			time.sleep(10)

		for pro in processes:
			pro.popen.wait()

		outputDir = scena.getAttribute("OUTPUT_DIR=",args[3])
		outputDir = outputDir.replace("\n", "")
		m = mergeResult.MergeResult()
		m.merge(outputDir, fileNum - 1, outputDir + "/control.stats.log")

if __name__ == '__main__':
	m = Main()
	m.main()
