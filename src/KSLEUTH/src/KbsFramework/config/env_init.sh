#!/usr/bin/env bash
yum -y update
yum -y install yum-utils
systemctl disable firewalld --now

cat <<EOF > /etc/yum.repos.d/kubernetes.repo
[kubernetes]
name=Kubernetes
baseurl=https://packages.cloud.google.com/yum/repos/kubernetes-el7-x86_64
enabled=1
gpgcheck=1
repo_gpgcheck=1
gpgkey=https://packages.cloud.google.com/yum/doc/yum-key.gpg https://packages.cloud.google.com/yum/doc/rpm-package-key.gpg
EOF

setenforce 0
sed -i 's/^SELINUX=enforcing$/SELINUX=disabled/' /etc/selinux/config

#INSTALL PYTHON:
sudo yum install -y python36 python36-libs python36-devel python36-pip
pip3 install pyyaml

# Install Docker CE
## Set up the repository
### Install required packages.
yum install -y device-mapper-persistent-data lvm2

### Add Docker repository.
yum-config-manager --add-repo \
  https://download.docker.com/linux/centos/docker-ce.repo

## Install Docker CE.
yum install -y \
  containerd.io \
  docker-ce-19.03.15 \
  docker-ce-cli-19.03.15

mkdir -pv /etc/docker

# Setup daemon.
cat > /etc/docker/daemon.json <<EOF
{
  "exec-opts": ["native.cgroupdriver=systemd"],
  "log-driver": "json-file",
  "log-opts": {
    "max-size": "100m"
  },
  "storage-driver": "overlay2",
  "storage-opts": [
    "overlay2.override_kernel_check=true"
  ]
}
EOF

# Restart Docker
systemctl daemon-reload
systemctl enable docker.service --now
docker pull feifanzhang/init-sleuth:1.0
docker pull feifanzhang/sleuth:1.0
systemctl restart docker.service
