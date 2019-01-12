#!/bin/bash 
cd ./sllib/&&./cp.sh
cd ..
cd ./img-make-hotspot/ &&./make.sh
cd ..
cd ./img-make-hotspot_old/ &&./make.sh
cp ./client.img /tftpboot
cd ..
cd ./img-make-hotspot/&&cp ./server.img /tftpboot
cd ..
cd ./img-make-hotspot_old/&&cp ./client.img /tftpboot
cd ..
cd ./img-make-hotspot/&&cp ./server.img /tftpboot


echo "OK"
