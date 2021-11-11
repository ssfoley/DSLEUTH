function install_metrics-server(){
  for file in auth-delegator.yaml auth-reader.yaml metrics-apiservice.yaml metrics-server-deployment.yaml metrics-server-service.yaml resource-reader.yaml;
    do wget https://raw.githubusercontent.com/kubernetes/kubernetes/master/cluster/addons/metrics-server/"$file";
  done
}

function main() {
    install_metrics-server
}
main "$@"