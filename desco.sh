#! /bin/bash

source /etc/profile

sleep 3s

exec nohup /root/desco/desco < /dev/null &> /var/log/desco.log


