function install_helm(){
  script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
wget https://get.helm.sh/helm-v3.1.0-linux-amd64.tar.gz
  tar xf helm-v3.1.0-linux-amd64.tar.gz
  mv "$script_dir"/linux-amd64/helm /usr/bin
  rm -rf helm-v3.0.3-linux-amd64.tar.gz
  rm -rf linux-amd64
  helm repo add stable https://kubernetes-charts.storage.googleapis.com/
  helm repo update
  #helm install stable/kubernetes-dashboard --generate-name

}

function main() {
    install_helm
}
main "$@"


