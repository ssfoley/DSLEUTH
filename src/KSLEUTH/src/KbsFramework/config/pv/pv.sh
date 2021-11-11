#!/usr/bin/env bash
# modify the ip of server
function install_pv() {
  local_ip=`ip route get 8.8.8.8 | sed -n '/src/{s/.*src *\([^ ]*\).*/\1/p;q}'`
  SCRIPT_DIR=$(dirname $(readlink -f $0))
  sed -i "/server/s/:.*/: $local_ip/g" "$SCRIPT_DIR/pv/pv_and_pvc.yaml"
  kubectl apply -f "$SCRIPT_DIR/pv/pv_and_pvc.yaml"
}
