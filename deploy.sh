#!/bin/bash

GAME_DELIVER=${1:-game_deliver.tar.gz}
PORT=${2:-80}
DOCKER_IMG_NAME="game_deploy:v1.0"
SRC_DIR=`dirname $(realpath $0)`
is_in_container() {
    cat /proc/1/cgroup | grep pids | grep docker 2>&1>/dev/null
}

make_docker_img_from_dockerfile() {
    docker build -t ${DOCKER_IMG_NAME} ${SRC_DIR}/depend_docker
}

get_docker_image() {
    docker images ${DOCKER_IMG_NAME} | grep game_deploy > /dev/null
    if [ $? != 0 ]
    then
        make_docker_img_from_dockerfile
    fi
}

start_all_server() {
    tar xf /root/game_deliver.tar.gz --skip-old-files -C /
    nginx -c /conf/nginx.conf
    /root/.ngrest/ngrest-build/deploy/bin/ngrestserver -s /lib &
    tcp_daemon
}

start_docker_con() {
    local CON_ID=`docker create -ti -p 80:${PORT} ${DOCKER_IMG_NAME} /root/deploy.sh`
    docker cp $0 ${CON_ID}:/root/
    docker cp ${GAME_DELIVER} ${CON_ID}:/root/
    docker start -ai ${CON_ID}
}

if is_in_container 
then
    start_all_server
else
    get_docker_image
    start_docker_con
fi