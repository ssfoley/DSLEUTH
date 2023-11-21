import queue
import subprocess
import sys
import scenarioUtil
import time
import os
import parseConfig


class Main:
        nodelist = {}
        queue = queue.Queue()
        sched = "unknown"
        phase = "unknown"
        sleuthPath = ""
        scenarioPath = ""
        num_nodes = 1
        DEBUG = False
        TESTING = False
        invalid = False

        def __init__(self):
                """
                Set the type of environment for this run
                Settings are defined in the run_settings file with the scheduler type and

                number of nodes
                """
                config = parseConfig.ParseConfig()
                if(config.parse()):
                        self.sleuthPath = config.sleuthPath
                        self.scenarioPath = config.scenarioPath
                        self.sched = config.sleuthMode
                        self.phase = config.phase
                        self.num_nodes = config.processors
                        self.DEBUG = config.debug
                        self.TESTING = config.testing
                        print("DSLEUTH: scheduler type: ", self.sched)
                        print("DSLEUTH: num_nodes: ", str(self.num_nodes))
                        self.getNodelist()
                else:
                        print("There was an issue with parsing the config file.  Please ensure it is formatted correctly.")
                        self.invalid = True




        def getNodelist(self):
                """
                Returns a dictionary of nodes and their states.
                If it is SLURM, the nodes are picked up from the scheduler.
                Otherwise, the run_settings file number of nodes is used to generate arbitrary nodes.
                """
                if self.sched == "SLURM":
                        # import hostlist     look at this again when working on cluster
                        slurmlist = hostlist.expand_hostlist(os.environ["SLURM_JOB_NODELIST"])
                        self.nodelist = dict(zip(slurmlist,[-1]*len(slurmlist)))
                else:
                        # print("h1")
                        for n in range(self.num_nodes):
                                self.nodelist.update({"".join(["node", str(n)]): -1})

        def done(self, p):
                """
                Checks to see if the process is done.  If it is done, the node is freed and bookkeeping updated.
                Otherwise returns false.
                """
                cur_pid = p.pid
                if p.poll() is not None:
                        # done
                        if self.DEBUG:
                                print("DSLEUTH: pid just finished ", str(cur_pid))
                                print("DSELUTH: ", list(self.nodelist.values()).index(cur_pid))
                                print("DSLEUTH: node just freed ", list(self.nodelist.keys())[list(self.nodelist.values()).index(cur_pid)])
                        # update node list
                        self.nodelist[list(self.nodelist.keys())[list(self.nodelist.values()).index(cur_pid)]] = -1
                        # return true
                        return True
                else:
                        if self.DEBUG:
                                print("DSLEUTH: not done! ", str(cur_pid))
                        return False

        def get_free_node(self):
                # assume there is a free node
                return list(self.nodelist.keys())[list(self.nodelist.values()).index(-1)]


        def merge(self, filePath, numOfFiles, finalPath):
                numOfRun = 0
                print("DSLEUTH: ", os.getcwd())
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
                # take care of the case when it is not a calibrate job and produce a warning
                if self.phase != "calibrate":
                        print("DSLEUTH: Warning: this framework is designed for distributing across multiple nodes, but only for calibrate mode.  Mode is ", self.phase)
                        print("DSLEUTH: Just running the code as is.")
                        print(self.sleuthPath, self.phase, self.scenarioPath)
                        p = subprocess.Popen([self.sleuthPath, self.path, self.scenarioPath])
                        p.wait()
                        return


                destination_path = self.scenarioPath + "_steps/"
                try:
                        os.makedirs(destination_path)
                except OSError:
                        print("WARNING: file path exists for scenario files, old files may be overwritten")
                log_file = open(destination_path + "dsleuth.log", "w")
                scena = scenarioUtil.ScenarioUtil(self.scenarioPath, destination_path, self.num_nodes, log_file)

                if scena.num_files == -2:
                        print("Change ScenarioFile and run again")
                        return


                # if we are just testing, then return here and examine the output
                if self.TESTING:
                        return

                print(scena)
                fileNum = scena.get_num_files() + 1
                print("DSLEUTH: ", time.strftime("%H:%M:%S"), file=log_file)

                # populate the queue with the scenario file names
                for x in range(1,fileNum):
                        self.queue.put(x)
                processes = []

                # launch jobs as long as there is work and free nodes
                while not self.queue.empty():
                        # while there are available nodes and the queue is not empty
                        while len(processes) < self.num_nodes and not self.queue.empty():
                                num = self.queue.get()
                                node = self.get_free_node()
                                print("DSLEUTH: attempting to launch on node: ", node, file=log_file)
                                if self.sched == "SLURM":
                                        p = subprocess.Popen(["srun", "-N", "1", "--nodelist=" + node, self.sleuthPath, self.phase, self.scenarioPath + "_steps/" + num])
                                else:
                                        print("DSLEUTH: executing: {} {} {}+_steps/+{}".format(self.sleuthPath, self.phase, self.scenarioPath, num), file=log_file)
                                        p = subprocess.Popen([self.sleuthPath, self.phase, self.scenarioPath + "_steps/" + str(num)])
                                if self.DEBUG:
                                        print("DSLEUTH: ", p.pid, file=log_file)
                                self.nodelist[node] = p.pid
                                processes.append(p)
                        # wait a little bit and then check again
                        time.sleep(1)
                        # check for finished processes
                        #   somelist[:] = [tup for tup in somelist if determine(tup)]
                        processes[:] = [ p for p in processes if not self.done(p) ]



                # all pieces of work have been divvied out, but processes are still working
                for pro in processes:
                        pro.wait()
                print("DSLEUTH: ", time.strftime("%H:%M:%S"), file=log_file)

                outputDir = scena.get_output_dir()
                self.merge(outputDir, fileNum - 1, outputDir + "control.stats.log")

                # don't need to do this anymore, we can use osm.py later
                """
                if self.phase == "calibrate":
                        subprocess.check_output(['make'])
                        subprocess.check_output(['./readdata3', outputDir + "control.stats.log"])
                        subprocess.check_output(['mv', 'top50b.log', outputDir])
                """

if __name__ == '__main__':
        m = Main()
        m.main()
