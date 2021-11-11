
function install_dashboard(){
  echo "=================================install_dashboard======================================="
  # to ge the the current script dir:
  # https://blog.csdn.net/somezz/article/details/81168443
  script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
  kubectl apply -f https://raw.githubusercontent.com/kubernetes/dashboard/v2.0.0-beta6/aio/deploy/recommended.yaml && \
  kubectl apply -f "$script_dir/dashboard_adminuser.yaml" && \
  kubectl -n kubernetes-dashboard describe secret "$(kubectl -n kubernetes-dashboard get secret | grep admin-user | awk '{print $1}')" && \
  # if port 8001 had been use:
  # https://www.cnblogs.com/hindy/p/7249234.html
  echo "visit dashboard from this url: http://localhost:8001/api/v1/namespaces/kubernetes-dashboard/services/https:kubernetes-dashboard:/proxy/."
  kubectl proxy && \
  echo "=================================dashboard_finish!======================================="



}
function main() {
    install_dashboard
}
main "$@"