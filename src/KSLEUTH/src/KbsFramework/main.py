import os
import re
import shutil
import subprocess
import time
from configparser import ConfigParser
from os import path

from devide_scenario import DevideScenario
from job import KJob


def clear_previous_data(*clear_paths):
    for clear_path in clear_paths:
        for _, dirs, files in os.walk(clear_path):
            if len(files) != 0:
                for file in files:
                    os.remove(clear_path + file)
            if len(dirs) != 0:
                for d in dirs:
                    shutil.rmtree(clear_path + d)


class Main:
    cp = ConfigParser()
    split_scenario_list = []
    sleuth_job_exe = path.join(path.dirname(path.abspath(__file__)), "job", "sleuth_job_exe")
    final_output_dir = ""
    split_scenario_dir = ""
    split_output_dir = ""
    jobs = []
    finished_jobs = []

    def __init__(self, sceanrio_file: str):
        self.cp.read(path.join(os.getenv('HOME'), ".ksleuth_config"))
        self.final_output_dir = self.cp['path']['reportOutput']
        self.split_scenario_dir = self.cp['path']['splitScenario']
        self.split_output_dir = self.cp['path']['splitOutput']

        subprocess.Popen(["kubectl", "delete", "job", "--all"]).wait()
        pattern = r'[a-z0-9]([-a-z0-9]*[a-z0-9])?(\.[a-z0-9]([-a-z0-9]*[a-z0-9])?)*'
        if re.match(pattern, sceanrio_file) is None:
            print("name of scenario file must consist of lower case alphanumeric characters, "
                  "'-' or '.', and must start and end with an alphanumeric character")
        else:
            clear_previous_data(
                self.split_scenario_dir, self.split_output_dir, self.sleuth_job_exe
            )
            self.scenario_name = sceanrio_file
            self.final_output_dir = path.join(self.final_output_dir, self.scenario_name, time.strftime(
                "%Y-%m-%d %H:%M:%S", time.localtime()))
            split_info = DevideScenario(sf=self.scenario_name).main()
            start = 0
            end = split_info['container_per_pod']
            job_idx = 1
            scenario_list = split_info['split_scenario_names']
            cpu_usage = split_info['cpu_usage']
            print(split_info)
            for j_idx in range(split_info['number_of_pod'] * split_info['number_of_node']):
                self.jobs.append(KJob(split_scenario_list=scenario_list[start:end], job_name="ksleuth-{}".format(job_idx), cpu_usage=cpu_usage))
                start += split_info['container_per_pod']
                end += split_info['container_per_pod']
                job_idx += 1
            for rest_idx in range(split_info['rest']):
                self.jobs.append(KJob(split_scenario_list=scenario_list[start:end], job_name="ksleuth-{}".format(job_idx), cpu_usage=cpu_usage))
                start += 1
                end += 1
                job_idx += 1
            self.split_scenario_list = split_info['split_scenario_names']
        return

    def main(self):
        # exit(233)
        for job in self.jobs:
            job.start()
        status = [1] * len(self.jobs)
        while True:
            time.sleep(2)
            self.print_progress()
            for i in range(len(self.jobs)):
                status[i] = self.jobs[i].check_status()
            if set(status) == {0}:
                break
        print("All Jobs Complete!")
        print("Writing control_stats.log...")
        self.merge(self.split_scenario_list)
        for job in self.jobs:
            subprocess.Popen(["kubectl", "delete", "job", job.job_name]).wait()
        print("All Set! :)")

    def merge(self, split_scenario_list: []):
        shutil.copytree(self.split_output_dir, self.final_output_dir)
        feature_name = "Run".rjust(5) + "Product".rjust(9) + "Compare".rjust(8) + "Pop".rjust(8) + \
                       "Edges".rjust(8) + "Clusters".rjust(9) + "Size".rjust(7) + "Leesalee".rjust(9) + \
                       "Slope".rjust(7) + "%Urban".rjust(8) + "Xmean".rjust(8) + "Ymean".rjust(8) + \
                       "Rad".rjust(8) + "Fmatch".rjust(8) + "Diff".rjust(5) + "Brd".rjust(5) + \
                       "Sprd".rjust(5) + "Slp".rjust(5) + "RG".rjust(5) + '\n'
        output_data = ["Cluster".center(150) + '\n', feature_name]
        with open(path.join(self.final_output_dir, "control_stats.log"), "w") as final_control_stats:
            final_control_stats.writelines(output_data)
            run_num = 0
            for split_scenario in split_scenario_list:
                with open(path.join(self.final_output_dir, split_scenario, "control_stats.log"),
                          "r") as split_control_stats:
                    data_list = split_control_stats.readlines()
                    data_list.pop(0)
                    data_list.pop(0)
                    for index in range(len(data_list)):
                        data_list[index] = str(run_num).rjust(5) + data_list[index][5:-1] + '\n'
                        run_num += 1
                    final_control_stats.writelines(data_list)
        return

    def print_progress(self):
        print("=" * 48)
        for job in self.jobs:
            if job in self.finished_jobs:
                continue
            job_progress = job.get_process_status()
            # progress = {
            #     "job_name": self.job_name,
            #     "job_finished": 1 if self.check_status() == 0 else 0,
            #     "containers_progress": []  # [{cur_case: 80, total_case: 81, percent: 98.8}]
            # }
            print("Job: {}".format(job_progress['job_name']))
            if job_progress["job_finished"] == 1:
                print("\tAll Done.")
                self.finished_jobs.append(job)
            else:
                counter = 1
                for c in job_progress['containers_progress']:
                    if c:
                        print("\tContainer#{}: {} of {} ({} percent complete)".format(
                            counter, c['cur_case'], c['total_case'], c['percent']))
                    else:
                        print("\tWaiting for the container to start...")
                    counter += 1


# test function
if __name__ == '__main__':
    start_time = time.time()
    print(os.path.dirname(os.getcwd()))
    sf = "scenario.demo200-calibrate"
    Main(sf).main()
    end_time = time.time() - start_time
    print("Cost %.2fs" % end_time)
