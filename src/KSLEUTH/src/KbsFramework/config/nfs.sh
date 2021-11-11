#!/usr/bin/env bash
# common error for nfs
# https://www.xuebuyuan.com/2158147.html
function install_nfs() {
  local_ip=`ip route get 8.8.8.8 | sed -n '/src/{s/.*src *\([^ ]*\).*/\1/p;q}'`
  input="$HOME/Input"
  splitScenario="$HOME/splitScenario/"
  splitOutput="$HOME/splitOutput/"
  reportOutput="$HOME/ksleuthOut/"
  originalScenario="$HOME/Scenarios"

  yum -y install nfs-utils rpcbind

  mkdir -pv $input $splitScenario $splitOutput $reportOutput $originalScenario

  chmod 755 "$input"
  chmod 755 "$splitScenario"
  chmod 755 "$splitOutput"
  chmod 755 "$originalScenario"
  chown nobody "$input"
  chown nobody "$splitScenario"
  chown nobody "$splitOutput"
  chown nobody "$originalScenario"
  cat <<EOF > /etc/exports
$input *(rw,no_root_squash,no_all_squash,sync)
$splitScenario *(rw,no_root_squash,no_all_squash,sync)
$splitOutput *(rw,no_root_squash,no_all_squash,sync)
EOF
  systemctl unmask rpcbind.service
  systemctl enable rpcbind.service --now
  systemctl enable nfs.service --now

  showmount -e $local_ip

  cat <<EOF > $HOME/.ksleuth_config
[path]
splitScenario=$splitScenario
splitOutput=$splitOutput
reportOutput=$reportOutput
originalScenario=$originalScenario
EOF

}

