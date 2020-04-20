#!/bin/bash
cp client.img /tftpboot
cp server.img /tftpboot
sudo cp client.img /mnt/nfs/tftpboot
sudo cp server.img /mnt/nfs/tftpboot


echo "OK"
