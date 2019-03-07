#! /bin/sh
ifconfig eth0 192.168.1.3 up netmask 255.255.255.0
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

echo "blk_pd mpu 0" > /proc/slpm
echo "blk_pd gpu 0" > /proc/slpm
echo "blk_pd vpu1 0" > /proc/slpm
echo "blk_pd dsp0 0" > /proc/slpm
echo "blk_pd dsp1 0" > /proc/slpm
echo "blk_pd fdip 0" > /proc/slpm
echo "blk_pd isp 0" > /proc/slpm
echo "blk_pd vpre2h264 0" > /proc/slpm

route add default gw 192.168.1.1
route add -net 224.0.0.0 netmask 240.0.0.0 dev eth0
#mount -o nolock 192.168.1.2:/home/n55/yuliubing /tmp
#cd /tmp/2016/hdmi/sllib/ksysctl/ko/&&./insmod-server.sh
cd /user/update/ko/&&./insmod-server.sh
cd /user/web/&&./boa &
cd /tmp
#cd /tmp/2016/hdmi/sllib/bin
./viu-vpu0h264enc-mdev-rtsp &
#vpp color more natural
#/user/word.csky 0xbfa49060 0x52f597f
#trigger m0
cat /dev/silan-cm0
/user/word.csky 0xbfba903c 0xffffffff

#mount -o nolock 192.168.1.2:/home/y/work/8925Docs/8925_test/hotspot-release_v1.8/sllib/bin /mnt
#cd /mnt/&&./viu-vpu0h264enc-mdev-rtsp &
