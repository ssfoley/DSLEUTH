#!/usr/bin/env bash
# this script only run in master node for init the environment
# using yum list installed|grep docker && kubernetes
# to check if install these or not, if exist, yum -y remove

# how to install master & worker node:
# https://theithollow.com/2019/01/14/deploy-kubernetes-using-kubeadm-centos7/

function install_master_node_env() {
  yum install -y kubelet kubeadm kubectl --disableexcludes=kubernetes
  swapoff -a
  cat <<EOF > /etc/sysctl.d/kbs.conf
net.bridge.bridge-nf-call-ip6tables = 1
net.bridge.bridge-nf-call-iptables = 1
EOF
  sysctl -f /etc/sysctl.d/kbs.conf
  echo -e "127.0.0.1\t$(hostname)" >> /etc/hosts
  systemctl enable kubelet --now
  kubeadm init --pod-network-cidr=10.244.0.0/16 --ignore-preflight-errors=all

  export KUBECONFIG=/etc/kubernetes/admin.conf
  mkdir -vp "$HOME"/.kube
  sudo cp -vi /etc/kubernetes/admin.conf "$HOME"/.kube/config
  sudo chown "$(id -u)":"$(id -g)" "$HOME"/.kube/config
  kubectl apply -f https://raw.githubusercontent.com/coreos/flannel/master/Documentation/kube-flannel.yml

  # allow to deploy on master node:
  kubectl taint nodes --all node-role.kubernetes.io/master-

  SCRIPT_DIR=$(dirname $(readlink -f $0))
  source "$SCRIPT_DIR/nfs.sh"
  source "$SCRIPT_DIR/pv/pv.sh"
  install_nfs
  install_pv
  exit 0
}

local_ip=`ip route get 8.8.8.8 | sed -n '/src/{s/.*src *\([^ ]*\).*/\1/p;q}'`

install_master_node_env "$local_ip"

