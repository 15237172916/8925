#! /bin/sh

#ifconfig eth0 192.168.1.3 netmask 255.255.255.0
mount -t jffs2 /dev/mtdblock4 /tmp

#start http server

#mount -o nolock 192.168.1.2:/home/n55/yuliubing /tmp
#cd /tmp/2016/hdmi/sllib/ksysctl/ko/&&./insmod-server.sh
#cd ../../bin
#./viu-vpu0h264enc-mdev-rtsp &

#cd /ko/&&./insmod-server.sh
#cd /bin
#./viu-vpu0h264enc-mdev-rtsp &

cd /tmp
if [ -f run.sh ]; then
    echo "tmp/run.sh OK"
    ./run.sh &
else
    echo "user/run.sh OK"
    cd /user
    ./run.sh &
fi
