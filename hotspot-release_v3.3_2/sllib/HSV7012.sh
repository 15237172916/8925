#!/bin/bash
cd ./example/ksysctl/ 
make clean
make MAKENAME=HSV7012
cd ../../bin

csky-abiv2-linux-strip -S viu-vpu0h264enc-mdev-rtsp
csky-abiv2-linux-strip -S rtsp-mdev-vpu0-vpp_pv 
csky-abiv2-linux-strip -S UpGradeClient
csky-abiv2-linux-strip -S UpGradeServer

#update
echo "update"
cp ./rtsp-mdev-vpu0-vpp_pv ../../img-make-hotspot/execute-client/ 
cp ./viu-vpu0h264enc-mdev-rtsp ../../img-make-hotspot/execute-server/
cp ./UpGradeClient ../../img-make-hotspot/execute-client/
cp ./UpGradeClient ../../img-make-hotspot/execute-server/
cp ./UpGradeServer ../../img-make-hotspot/execute-server/
cp ./tmp/configs ../../img-make-hotspot/execute-server/ -r

#TX user
echo "TX user"
rm ../../img-make-hotspot/user-server/web -rf #server: rm web
cp ./Web/HSV7012/web ../../img-make-hotspot/user-server/ -r #server:add user/web
cp ./tmp/HSV7012/run_s.sh ../../img-make-hotspot/user-server/run.sh #server: user/run.sh

echo "RX user"
rm ../../img-make-hotspot/user-client/web -rf #client: rm web
cp ./Web/HSV7012/web_client ../../img-make-hotspot/user-client/web -r #client:add user/web
cp ./tmp/HSV7012/run_c.sh ../../img-make-hotspot/user-client/run.sh #client: user/run.sh

echo "cp TX web to /tmp/update/"
rm ./tmp/HSV7012/update/web -rf #server: rm tmp/update/web
cp ./Web/HSV7012/web ./tmp/HSV7012/update/ -rf #server: add tmp/update/web
cd ./tmp/HSV7012/update&&./ln.sh
cd ../../../

rm ../../img-make-hotspot/execute-server/update/* -r
cp ./tmp/HSV7012/update/web ../../img-make-hotspot/execute-server/update/ -rf

#ko
echo "cp ko"
cd ../
rm ../img-make-hotspot/user-server/update/ko -r
rm ../img-make-hotspot/user-client/update/ko -r
cp ./ko/ ../img-make-hotspot/user-client/update/ -rf
cp ./ko/ ../img-make-hotspot/user-server/update/ -rf

cd ./bin/CGI/HSV7012/cgi-bin-control && ./build.sh && cd ../../../../
#cp ./rtsp-mdev-vpu0-vpp_pv ../../img-make-hotspot_old/execute-client/
#cp ./viu-vpu0h264enc-mdev-rtsp ../../img-make-hotspot_old/execute-server/

echo "cp tftpboot"
sudo cp ./bin/rtsp-mdev-vpu0-vpp_pv /tftpboot
sudo cp ./bin/viu-vpu0h264enc-mdev-rtsp /tftpboot
sudo cp ./bin/Web/HSV7012/web /tftpboot -r

echo "cp mnt/nfs/tftpboot"
sudo cp ./bin/rtsp-mdev-vpu0-vpp_pv /mnt/nfs/tftpboot
sudo cp ./bin/viu-vpu0h264enc-mdev-rtsp /mnt/nfs/tftpboot
sudo cp ./bin/Web/HSV7012/web /mnt/nfs/tftpboot -r

echo OK 

