import os
from configparser import ConfigParser
from functools import reduce
import math

from scenario_template import Scenario

path = os.path
# select the 'initContainer' --> make directory as --> sources Root


class DevideScenario:
    cp = ConfigParser()

    split_scenario_file = ''
    split_output_file = ''

    def __init__(self, sf):
        """
        init the class
        :param sf: str original scenario name
        """
        print("ss is : " + sf)
        self.cp.read(path.join(os.getenv('HOME'), ".ksleuth_config"))
        self.split_scenario_file = self.cp['path']['splitScenario']
        self.split_output_file = self.cp['path']['splitOutput']
        self.scenario_file = Scenario(sf)
        self.node_num = int(os.popen("kubectl get nodes|wc -l").read()) - 2
        # self.node_num = 5
        self.max_pod_num = 2
        self.split_parameter_set = []

    def main(self):

        """
        main function of init_container, generate split scenario
        :return: list [split_scenario_name_list, num_of_pods, num_of_container_per_pod]
        """

        split_way = self.__split_parameters()
        self.__gen_split_data(split_way['split_way'])
        # create files for each split scenario
        split_scenario_names = self.__gen_split_scenario()
        for ss_name in split_scenario_names:
            os.mkdir(path.join(self.split_output_file, ss_name))
        split_way.update({
            "split_scenario_names": split_scenario_names
        })
        return split_way

    def __gen_split_data(self, split_way, split_parameter=None):

        """
        according the split_way to generate the data of each split scenario
        and add the each set of data into split_parameter_set(is a list)
        :param split_way: str
        :param split_parameter: list
        :return: None
        """
        if split_parameter is None:
            split_parameter = []
        if len(split_parameter) == 15:
            self.split_parameter_set.append(split_parameter)
            return
        elif split_way[int((len(split_parameter)) / 3)] == '1':  # parameter is divisible
            if len(split_parameter) == 0:
                for index in range(self.scenario_file.diffStart, self.scenario_file.diffStop + 1,
                                   self.scenario_file.diffStep):
                    self.__gen_split_data(split_way, split_parameter + [index, self.scenario_file.diffStep, index])

            elif len(split_parameter) == 3:
                for index in range(self.scenario_file.spreadStart, self.scenario_file.spreadStop + 1,
                                   self.scenario_file.spreadStep):
                    self.__gen_split_data(split_way, split_parameter + [index, self.scenario_file.spreadStep, index])

            elif len(split_parameter) == 6:
                for index in range(self.scenario_file.slopeStart, self.scenario_file.slopeStop + 1,
                                   self.scenario_file.slopeStep):
                    self.__gen_split_data(split_way, split_parameter + [index, self.scenario_file.slopeStep, index])

            elif len(split_parameter) == 9:
                for index in range(self.scenario_file.breedStart, self.scenario_file.breedStop + 1,
                                   self.scenario_file.breedStep):
                    self.__gen_split_data(split_way, split_parameter + [index, self.scenario_file.breedStep, index])

            elif len(split_parameter) == 12:
                for index in range(self.scenario_file.roadStart, self.scenario_file.roadStop + 1,
                                   self.scenario_file.roadStep):
                    self.__gen_split_data(split_way, split_parameter + [index, self.scenario_file.roadStep, index])
        else:
            if len(split_parameter) == 0:
                self.__gen_split_data(split_way, split_parameter + [self.scenario_file.diffStart,
                                                                    self.scenario_file.diffStep,
                                                                    self.scenario_file.diffStop])

            elif len(split_parameter) == 3:
                self.__gen_split_data(split_way, split_parameter + [self.scenario_file.spreadStart,
                                                                    self.scenario_file.spreadStep,
                                                                    self.scenario_file.spreadStop])

            elif len(split_parameter) == 6:
                self.__gen_split_data(split_way, split_parameter + [self.scenario_file.slopeStart,
                                                                    self.scenario_file.slopeStep,
                                                                    self.scenario_file.slopeStop])

            elif len(split_parameter) == 9:
                self.__gen_split_data(split_way, split_parameter + [self.scenario_file.breedStart,
                                                                    self.scenario_file.breedStep,
                                                                    self.scenario_file.breedStop])

            elif len(split_parameter) == 12:
                self.__gen_split_data(split_way, split_parameter + [self.scenario_file.roadStart,
                                                                    self.scenario_file.roadStep,
                                                                    self.scenario_file.roadStop])
        return

    def __gen_split_scenario(self):

        """
        store the split_parameter_set into scenario file
        :return: list split_scenario_name
        """

        split_scenario_name = []
        counter = 0
        for split_parameter in self.split_parameter_set:
            self.scenario_file.diffStart = split_parameter[0]
            self.scenario_file.diffStep = split_parameter[1]
            self.scenario_file.diffStop = split_parameter[2]

            self.scenario_file.spreadStart = split_parameter[3]
            self.scenario_file.spreadStep = split_parameter[4]
            self.scenario_file.spreadStop = split_parameter[5]

            self.scenario_file.slopeStart = split_parameter[6]
            self.scenario_file.slopeStep = split_parameter[7]
            self.scenario_file.slopeStop = split_parameter[8]

            self.scenario_file.breedStart = split_parameter[9]
            self.scenario_file.breedStep = split_parameter[10]
            self.scenario_file.breedStop = split_parameter[11]

            self.scenario_file.roadStart = split_parameter[12]
            self.scenario_file.roadStep = split_parameter[13]
            self.scenario_file.roadStop = split_parameter[14]

            self.scenario_file.save_split_scenario(str(os.path.split(self.scenario_file.origin_scenario_file)[1])
                                                   + str(counter))

            split_scenario_name.append(str(os.path.split(self.scenario_file.origin_scenario_file)[1])
                                       + str(counter))
            counter += 1
        return split_scenario_name
    
    def __split_node_work(self, node_work):
        results = []        
        for x in range(1, node_work+1):
            if node_work % x == 0:
                results.append((x, node_work // x))
        for x in sorted(results, key=lambda x: x[0] - x[1]):
            if x[0] - x[1] > 0:
                return x
        return None

    def __split_parameters(self):

        """
        try all the ways to split the scenario and choose the one that closest to the specified number of pod
        return the split_way to describe how to split the parameters
        :return: str
        """
        split_arg = []
        rest = 0
        nums = [self.scenario_file.diffNum, self.scenario_file.spreadNum,
                self.scenario_file.slopeNum, self.scenario_file.breedNum,
                self.scenario_file.roadNum]
        min_score = reduce(lambda x, y: x * y, nums)
        # from 1 to 32 is all the possibility to split scenario([0,0,0,0,1] to [1,1,1,1,1])
        # 0 means not split; 1 means split
        for sp in range(1, 32):
            # scenario_pieces indicates the number of splitted scenario files
            # sleuth_runtime is the runtime for each container
            scenario_pieces = 1
            sleuth_runtime = 1
            parameters = bin(sp)[2:].rjust(5, '0')
            #  cal the num of containers shall be needed
            for index in range(len(parameters)):
                if parameters[index] == '1':
                    scenario_pieces = scenario_pieces * nums[index]
                else:
                    sleuth_runtime = sleuth_runtime * nums[index]
            # average as much as possible, the rest part will be considered separately
            rest = scenario_pieces % self.node_num if scenario_pieces > self.node_num else 0
            
            score = abs(sleuth_runtime - scenario_pieces)

            if scenario_pieces < self.node_num:
                score += 1
            
            print("-"*10)
            print("Trying Case#{}-{}\nScenario Pieces:{}\nScore: {}".format(sp, parameters, scenario_pieces, score))            
            
            # grade=[A, B] means need A pod and B containers for each pod
            grade = [scenario_pieces, 1]

            if score > 0 and score < min_score:
                min_score = score
                split_arg = [sp, grade[0], grade[1], scenario_pieces, rest]

        print("the way to split scenario is: " + bin(split_arg[0])[2:].rjust(5, '0'))

        node_work = split_arg[1] // self.node_num if split_arg[1] > self.node_num else 1

        if node_work == 1:
            number_of_pod_per_node = 1
            number_of_container_per_pod = 1
        else:
            number_of_pod_per_node, number_of_container_per_pod = self.__split_node_work(node_work)

        cpu_request = "{}m".format(math.floor((os.cpu_count()-1) * 1000 / (number_of_pod_per_node + 1)))
        cpu_limit = "{}m".format((os.cpu_count()-1) * 1000)

        return {
            "split_way": bin(split_arg[0])[2:].rjust(5, '0'),
            "number_of_node": self.node_num,
            "number_of_pod": number_of_pod_per_node,
            "container_per_pod": number_of_container_per_pod,
            "rest": split_arg[4],
            "cpu_usage": (cpu_request, cpu_limit)
        }
        # return [bin(split_arg[0])[2:].rjust(5, '0'), split_arg[1], split_arg[2]]
