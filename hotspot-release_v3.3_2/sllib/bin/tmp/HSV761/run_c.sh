#! /bin/sh
sleep 2;
ifconfig eth0 192.168.1.201 up netmask 255.255.255.0
echo 20971520 > /proc/sys/net/core/rmem_max
echo 20971520 > /proc/sys/net/core/wmem_max
echo 20971520 >/proc/sys/net/core/wmem_default
echo "20971520 20971520 20971520" >/proc/sys/net/ipv4/tcp_rmem
echo "20971520 20971520 20971520" >/proc/sys/net/ipv4/tcp_wmem
echo 500 > /proc/sys/net/core/netdev_max_backlog
echo 50 > /proc/sys/net/ipv4/tcp_retries2

# enable mic i2s in
/user/word.csky 0xbfba9010 0x07980120
# enable dma clk bit 9
/user/word.csky 0xbfba9000 0xef0d32df
/user/word.csky 0xbfba9004 0x802a0013

# enhance vpp access ddr ability
#default
/user/word.csky 0xbfbe4000 0x33011022
#later
#/user/word.csky 0xbfbe4000 0x11022044
#/user/word.csky 0xbfbe4004 0x60550330
#/user/word.csky 0xbfbe4008 0x6

#/user/word.csky 0xbfba9038  0x19836a0  
#/user/word.csky 0xbfba903c 0xfb27d0db 

# for usb
#/user/word.csky 0xbfba9054 0x4000000 

# up m0 speed
/user/word.csky 0xbfba902c 0x10019

#echo "blk_pd mpu 0" > /proc/slpm
#echo "blk_pd gpu 0" > /proc/slpm
#echo "blk_pd vpu1 0" > /proc/slpm
#echo "blk_pd dsp0 0" > /proc/slpm
#echo "blk_pd dsp1 0" > /proc/slpm
#echo "blk_pd fdip 0" > /proc/slpm
echo "blk_pd isp 0" > /proc/slpm
#echo "blk_pd vpre2h264 0" > /proc/slpm

#add route list
route add default gw 192.168.1.1
route add -net 224.0.0.0 netmask 240.0.0.0 dev eth0
#mount -o nolock 192.168.1.2:/home/n55/yuliubing /tmp
#cd /tmp/2016/hdmi/sllib/ksysctl/ko/&&./insmod-server.sh
sleep 1;

#cd /tmp
#vpp color more natural
#/user/word.csky 0xbfa49060 0x52f597f
#trigger m0
cat /dev/silan-cm0
/user/word.csky 0xbfba903c 0xffffffff


#cd /user/update/ko/&&./insmod-server.sh
#insmod ko

#insmod slcommon.ko log_levels=1
if [ -f /tmp/update/ko/slcommon.ko ]; then
    echo "/tmp/update/ko/slcommon.ko OK"
    insmod /tmp/update/ko/slcommon.ko log_levels=1
else
    echo "/user/update/ko/slcommon.ko OK"
    insmod /user/update/ko/slcommon.ko log_levels=1
fi

#insmod slksysctl.ko
if [ -f /tmp/update/ko/slksysctl.ko ]; then
    echo "/tmp/update/ko/slksysctl.ko OK"
    insmod /tmp/update/ko/slksysctl.ko
else
    echo "/user/update/ko/slksysctl.ko OK"
    insmod /user/update/ko/slksysctl.ko
fi

#insmod slmemorydev.ko
if [ -f /tmp/update/ko/slmemorydev.ko ]; then
    echo "/tmp/update/ko/slmemorydev.ko OK"
    insmod /tmp/update/ko/slmemorydev.ko
else
    echo "/user/update/ko/slmemorydev.ko OK"
    insmod /user/update/ko/slmemorydev.ko
fi

#insmod slvpu.ko
if [ -f /tmp/update/ko/slvpu.ko ]; then
    echo "/tmp/update/ko/slvpu.ko OK"
    insmod /tmp/update/ko/slvpu.ko 
else
    echo "/user/update/ko/slvpu.ko OK"
    insmod /user/update/ko/slvpu.ko
fi

#insmod slvpp.ko
if [ -f /tmp/update/ko/slvpp.ko ]; then
    echo "/tmp/update/ko/slvpp.ko OK"
    insmod /tmp/update/ko/slvpp.ko
else
    echo "/user/update/ko/slvpp.ko OK"
    insmod /user/update/ko/slvpp.ko
fi

#insmod slviu.ko
#if [ -f /tmp/update/ko/slviu.ko ]; then
#    echo "/tmp/update/ko/slviu.ko OK"
#    insmod /tmp/update/ko/slviu.ko
#else
#    echo "/user/update/ko/slviu.ko OK"
#    insmod /user/update/ko/slviu.ko
#fi

#insmod slaudio.ko
if [ -f /tmp/update/ko/slaudio.ko ]; then
    echo "/tmp/update/ko/slaudio.ko OK"
    insmod /tmp/update/ko/slaudio.ko
else
    echo "/user/update/ko/slaudio.ko OK"
    insmod /user/update/ko/slaudio.ko
fi

#insmod sldsp-ir.ko
if [ -f /tmp/update/ko/sldsp-ir.ko ]; then
    echo "/tmp/update/ko/sldsp-ir.ko OK"
    insmod /tmp/update/ko/sldsp-ir.ko
else
    echo "/user/update/ko/sldsp-ir.ko OK"
    insmod /user/update/ko/sldsp-ir.ko
fi


#boa
if [ -f /tmp/update/web/boa ]; then
    echo "/tmp/update/web/boa OK"
    cd /tmp/update/web/&&./boa &
else
    echo "user/web/boa OK"
    cd /user/web/&&./boa &
fi

#bin
#./tmp/configs/ip.conf
cd /tmp/&&./rtsp-mdev-vpu0-vpp_pv &
cd /tmp/&&./UpGradeClient &

#mount -o nolock 192.168.1.253:/home/ubuntu/Work/8925/test/hotspot-release_v3.3_2/sllib/bin /mnt
#mount -o nolock 192.168.1.2:/tftpboot /mnt
#cd /mnt/&&./rtsp-mdev-vpu0-vpp_pv&
#cd /mnt/web/&&./boa

