#! /bin/bash

source /etc/profile

[ -f /run/desco_first ] && sleep 1s
touch /run/desco_first

exec nohup /root/desco/desco < /dev/null &> /var/log/desco.log
