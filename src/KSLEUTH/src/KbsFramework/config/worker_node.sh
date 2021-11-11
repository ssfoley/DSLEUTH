#!/usr/bin/env bash
function install_worker_node_env() {
  yum install -y nfs-utils kubelet kubeadm kubectl --disableexcludes=kubernetes
  swapoff -a

  cat <<EOF > /etc/sysctl.d/kbs.conf
  net.bridge.bridge-nf-call-ip6tables = 1
  net.bridge.bridge-nf-call-iptables = 1
EOF

  sysctl -f /etc/sysctl.d/kbs.conf
  echo -e "127.0.0.1\t$(hostname)" >> /etc/hosts
  systemctl enable kubelet --now
  kubeadm join $master_ip:6443 --token $token \
    --discovery-token-ca-cert-hash $ca_hash --ignore-preflight-errors=all

}

read -p "Enter your master ip: " master_ip
read -p "Enter your Kubernetes token: " token
read -p "Enter your Kubernetes ca-cert hash: " ca_hash

install_worker_node_env
