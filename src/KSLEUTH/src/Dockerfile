FROM centos:7
RUN yum -y install gcc automake autoconf libtool make
RUN yum -y install gcc-c++
RUN yum -y install epel-release && yum install -y https://centos7.iuscommunity.org/ius-release.rpm
ENV SPLIT_SCENARIO_FILE="split_scenario_file"
WORKDIR /SLEUTH
COPY ./ ./
RUN chmod +x /SLEUTH/KbsFramework/scenario_container_init.sh
CMD tail -f /var/log/cron.log
ENTRYPOINT /SLEUTH/KbsFramework/scenario_container_init.sh ${SPLIT_SCENARIO_FILE}
# ENTRYPOINT donot add ',' when give the parameter
