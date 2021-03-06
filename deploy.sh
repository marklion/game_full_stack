#!/bin/bash

GAME_DELIVER="game_deliver.tar.gz"
WECHAT_SECRET_INPUT=""
PORT=80
DATA_BASE="user_cash.db"

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
    valgrind /root/.ngrest/ngrest-build/deploy/bin/ngrestserver -s /lib &
    valgrind tcp_daemon &
    bash
}

start_docker_con() {
    local DATA_BASE_PATH=`realpath $DATA_BASE`
    local DATA_BASE_PATH=`dirname ${DATA_BASE_PATH}`
    local CON_ID=`docker create -ti -p 80:${PORT} -e WECHAT_SECRET="${WECHAT_SECRET_INPUT}"  -v ${DATA_BASE_PATH}:/database ${DOCKER_IMG_NAME} /root/deploy.sh`
    docker cp $0 ${CON_ID}:/root/
    docker cp ${GAME_DELIVER} ${CON_ID}:/root/
    docker start -ai ${CON_ID}
}

while getopts "D:p:w:d:" arg
do
    case $arg in
        D)
            GAME_DELIVER=${OPTARG}
            ;;
        p)
            PORT=${OPTARG}
            ;;
        w)
            WECHAT_SECRET_INPUT=${OPTARG}
            ;;
        d)
            DATA_BASE=${OPTARG}
            ;;
        *)
            echo "invalid args"
            ;;
    esac
done

if is_in_container 
then
    start_all_server
else
    get_docker_image
    start_docker_con
fi