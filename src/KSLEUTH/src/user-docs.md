# KSLEUTH

A distributed framework for running the KSLEUTH application and a parallel version of the SLEUTH using Kubernetes.  SLEUTH was originally developed by Project Gigalopolis. The website where more information about the model and the original source code can be found:
http://www.ncgia.ucsb.edu/projects/gig/index.html

# Environment

* **Operation System: Centos8**
* **Kubernetes: v1.19 or higher**
* **Docker: v19.01 or higher**
* **Python: 3.6 or higher**
* **CPU: at least 2 cores**
* **Memory: at least 2GB**

# How to run

## Installation

You need to run the shell script with the following:

```shell script
# install docker and do environment initiation
sh ./KSLEUTH/KbsFramework/config/env_init.sh

# install KBS for master node
sh ./KSLEUTH/KbsFramework/config/master_node.sh

# install KBS for worker node (run on worker node after your master node is ready)
sh ./KSLEUTH/KbsFramework/config/worker_node.sh
```

The `env_init.sh` is required for both master and worker nodes and needs to be execute before `master_node.sh` and `worker_node.sh`.

The `worker_node.sh` will required your master IP address, Kubernetes bootstrap token and a SHA 256 cert hash, which all can be found and copied from the output of `master_node.sh`.

Example:

```shell
(from output of master_node.sh)
...(omitted)
Then you can join any number of worker nods bu running the follinw on each as root:

kubeadm join xxx.xxx.xxx.xxx:6443 --token 4bgw8n.luabcifh29dghhe \
	--discovery-token-ca-cert-hash sha256:chg838749hv82389dhj398ch3289wehu235y892wefbh23e89r23
...(omitted)

(execute worker_node.sh)
Enter your master ip: xxx.xxx.xxx.xxx
Enter your Kubernetes token: 4bgw8n.luabcifh29dghhe
Enter your Kubernetes ca-cert hash: sha256:chg838749hv82389dhj398ch3289wehu235y892wefbh23e89r23
```

## Configuration

The default configuration file will be generated under your HOME directory after you execute the master_node.sh on your master node and the name is `.ksleuth_config`. The default path are all configured under your HOME directory, you can change them in the configuration file. Below is a sample of the configuration file:

```ini
[path]
splitScenario=/root/splitScenario/
splitOutput=/root/splitOutput/
reportOutput=/root/ksleuthOut/
originalScenario=/root/Scenarios
```

## Running instruction

There will be 5 directories created under your HOME directory after you set up KSLEUTH, which are:

* Input/ - where to put the input images
* ksleuthOut/ - store the results of KSLEUTH
* Scenarios/ - where to put the original scenario file
* splitOutput/ - store the results of SLEUTH instances
* splitScenario/ - the split scenario files will be outputted to here

To change these paths you may need to reconfigure the NFS path and then recreate the PV and PVC resources for Kubernetes by editing the `/etc/exports` and `pv_and_pvc.yaml`.

Below are the steps to run:

2. Copy the scenario file `scenario.demo200-calibrate` from `KSLEUTH/sample_data/Scenarios/scenario_for_KSLEUTH/` into the `/$HOME/Scenarios`. You can change the destination in the configuration file that mentioned above.
3. You may want to modify the parameters in the scenario file.
4. Make sure you have copied all the input graphs into `/$HOME/KSLEUTH/Input/` or the input directory you modified.
5. Now you should be ready for running KSLEUTH, execute `python3 /$HOME/KSLEUTH/KbsFramework/main.py`
6. After KSLEUTH completed, the `control_stats.log` will be outputted to `/$HOME/ksleuthOut` or the output location you modified.

