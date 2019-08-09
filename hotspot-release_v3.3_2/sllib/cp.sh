#!/bin/bash
cd ./example/ksysctl/ 
make
cd ../../bin

csky-abiv2-linux-strip -S viu-vpu0h264enc-mdev-rtsp
csky-abiv2-linux-strip -S rtsp-mdev-vpu0-vpp_pv 
csky-abiv2-linux-strip -S control-box
csky-abiv2-linux-strip -S UpGradeClient
csky-abiv2-linux-strip -S UpGradeServer

#update
cp ./rtsp-mdev-vpu0-vpp_pv ../../img-make-hotspot/execute-client/ 
cp ./viu-vpu0h264enc-mdev-rtsp ../../img-make-hotspot/execute-server/
cp ./control-box ../../img-make-hotspot/execute-control/
cp ./UpGradeClient ../../img-make-hotspot/execute-client/
cp ./UpGradeClient ../../img-make-hotspot/execute-server/
cp ./UpGradeServer ../../img-make-hotspot/execute-server/
cp ./tmp/configs ../../img-make-hotspot/execute-server/ -r
cp ./tmp/configs ../../img-make-hotspot/execute-control/ -r


#user
rm ../../img-make-hotspot/user-server/web -rf #server: rm web
cp ./web ../../img-make-hotspot/user-server/ -r #server:add user/web

rm ./tmp/update/web -rf #server: rm tmp/update/web
cp ./web ./tmp/update/ -rf #server: add tmp/update/web
cd ./tmp/update&&./ln.sh
cd ../../

#server: /user
rm ../../img-make-hotspot/execute-server/update/* -r
cp ./tmp/update/web ../../img-make-hotspot/execute-server/update/ -rf
cp ./tmp/run_s.sh ../../img-make-hotspot/user-server/run.sh #server: user/run.sh

#client: /user
rm ../../img-make-hotspot/user-client/web -rf #client: rm web
cp ./web_client ../../img-make-hotspot/user-client/web -r #client: user/web
cp ./tmp/run_c.sh ../../img-make-hotspot/user-client/run.sh #client: user/run.sh

#control: /user
rm ../../img-make-hotspot/user-control/web -rf #control: rm web
cp ./web_control ../../img-make-hotspot/user-control/web -r #control: user/web
cp ./tmp/run_control.sh ../../img-make-hotspot/user-control/run.sh #control: user/run.sh

#ko
cd ../
rm ../img-make-hotspot/user-server/update/ko -r
rm ../img-make-hotspot/user-client/update/ko -r
rm ../img-make-hotspot/user-control/update/ko -r

cp ./ko/ ../img-make-hotspot/user-client/update/ -rf
cp ./ko/ ../img-make-hotspot/user-server/update/ -rf
cp ./ko/ ../img-make-hotspot/user-control/update/ -rf



#cp ./rtsp-mdev-vpu0-vpp_pv ../../img-make-hotspot_old/execute-client/
#cp ./viu-vpu0h264enc-mdev-rtsp ../../img-make-hotspot_old/execute-server/

echo OK 

