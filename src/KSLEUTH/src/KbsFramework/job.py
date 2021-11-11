import re
import subprocess
from os import path

import yaml


class KJob(object):
    pod_name = ""
    container_name_list = []
    cur_path = path.dirname(__file__) + '/'

    def __init__(self, job_name, split_scenario_list, cpu_usage=None):
        """
        :param job_name:
        :param split_scenario_list
        """
        with open(path.join(self.cur_path, "sleuth_job_template.yaml"), 'r') as ss:
            self.data = yaml.safe_load(ss)
            self.container_template = self.data['spec']['template']['spec']['containers'][0]
            self.data['spec']['template']['spec']['containers'].clear()
        self.data['metadata']['name'] = job_name
        self.data['spec']['template']['metadata']['name'] = job_name + "-pod"
        self.__add_container(split_scenario_list, cpu_usage)
        with open(path.join(self.cur_path, "sleuth_job_exe", job_name + ".yaml"), 'w') as sleuth_job:
            self.job_name = job_name
            yaml.safe_dump(self.data, sleuth_job)

    def __add_container(self, split_scenario_name_list, cpu_usage=None):
        self.container_name_list = []
        for i in split_scenario_name_list:
            container = {
                'name': i.replace('_', '-').replace('.', '-'),
                'image': self.container_template['image'],
                'imagePullPolicy': self.container_template['imagePullPolicy'],
                'volumeMounts': self.container_template['volumeMounts'],
                'env': [{'name': 'SPLIT_SCENARIO_FILE', 'value': i}],
                'resources': self.container_template['resources']
            }
            self.container_name_list.append(container['name'])
            if cpu_usage:
                container['resources']['requests']['cpu'] = cpu_usage[0]
                container['resources']['limits']['cpu'] = cpu_usage[1]
            self.data['spec']['template']['spec']['containers'].append(container)

    def start(self):
        subprocess.Popen(["kubectl", "apply", "-f",
                          path.join(self.cur_path, "sleuth_job_exe", self.job_name + ".yaml")]).communicate()
        out, _ = subprocess.Popen(["kubectl", "describe", "job", self.job_name], stdout=subprocess.PIPE,
                                  stderr=subprocess.DEVNULL).communicate()
        self.pod_name = str(out).split()[-1][:-3]
        print("pod " + self.pod_name + " has started")

    def get_process_status(self):
        pattern = r"(?P<cur_case>\d+) of (?P<total_case>\d+).*\s(?P<percent>\d+.\d+)"
        progress_pattern = re.compile(pattern)
        progress = {
            "job_name": self.job_name,
            "job_finished": 1 if self.check_status() == 0 else 0,
            "containers_progress": []  # [{cur_case: 80, total_case: 81, percent: 98.8}]
        }
        for container_name in self.container_name_list:
            matched_group = {}
            if self.check_status() == 0:
                return progress
            else:
                out, _ = subprocess.Popen(["kubectl", "logs", self.pod_name, container_name],
                                          stdout=subprocess.PIPE, stderr=subprocess.DEVNULL).communicate()
                for i in str(out).split('\\n'):
                    if "percent complete" in i:
                        matched_group = progress_pattern.search(i).groupdict()
            progress["containers_progress"].append(matched_group)
        return progress

    def check_status(self):
        """
        :return: return the number of jobs which not completed
        """
        out, _ = subprocess.Popen(["kubectl", "get", "job", self.job_name], stdout=subprocess.PIPE).communicate()
        result = str(out).split()
        return int(result[4][2]) - int(result[4][0])  # 0/1 1/1
