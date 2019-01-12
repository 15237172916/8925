#! /bin/sh
ifconfig eth0 192.168.1.6 up netmask 255.255.255.0
echo 20971520 > /proc/sys/net/core/rmem_max
echo 20971520 > /proc/sys/net/core/wmem_max
echo 20971520 >/proc/sys/net/core/wmem_default
echo "20971520 20971520 20971520" >/proc/sys/net/ipv4/tcp_rmem
echo "20971520 20971520 20971520" >/proc/sys/net/ipv4/tcp_wmem
echo 500 > /proc/sys/net/core/netdev_max_backlog
echo 50 > /proc/sys/net/ipv4/tcp_retries2

# enable dma clk bit 9
/user/word.csky 0xbfba9000 0xef0d32df
/user/word.csky 0xbfba9004 0x802a0013
# enhance vpp access ddr ability
/user/word.csky 0xbfbe4000 0x33011022

echo "blk_pd mpu 0" > /proc/slpm
echo "blk_pd gpu 0" > /proc/slpm
echo "blk_pd vpu1 0" > /proc/slpm
echo "blk_pd dsp0 0" > /proc/slpm
echo "blk_pd dsp1 0" > /proc/slpm
echo "blk_pd fdip 0" > /proc/slpm
echo "blk_pd isp 0" > /proc/slpm
echo "blk_pd vpre2h264 0" > /proc/slpm

route add -net 224.0.0.0 netmask 240.0.0.0 dev eth0
route add default gw 192.168.1.1
#mount -o nolock 192.168.1.2:/home/n55/yuliubing /var
#cd /var/2017/sllib/ksysctl/ko/&&./insmod.sh
#ln -s /user/usr /usr
cd /user/update/ko/&&./insmod.sh
cd /user/web/&&./boa &
#cd /var/2017/sllib/bin
cd /tmp
./rtsp-mdev-vpu0-vpp_pv &
#vpp color more natural
#/user/word.csky 0xbfa49060 0x52f597f

#trigger m0
cat /dev/silan-cm0
/user/word.csky 0xbfba903c 0xffffffff

#mount -o nolock 192.168.1.2:/home/y/work/8925Docs/8925_test/Baseline_8925/900/test/hotspot-release_v1.7_3/sllib/bin /mnt
#cd /mnt/&&./rtsp-mdev-vpu0-vpp_pv &
