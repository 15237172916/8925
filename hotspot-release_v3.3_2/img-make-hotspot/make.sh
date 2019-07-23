#!/bin/sh

rm server.img
rm client.img
#rm execute-server.jffs2
#rm execute-client.jffs2
#rm rootfs-server.sfs
#rm rootfs-client.sfs

rm rootfs/user -rf
cp user-server rootfs/user -rf
cp m0-send-ir.bin rootfs/lib/firmware/cm0.bin
mksquashfs rootfs rootfs-server.sfs

rm rootfs/user -rf
cp user-client rootfs/user -rf
cp m0-get-ir.bin rootfs/lib/firmware/cm0.bin
mksquashfs  rootfs rootfs-client.sfs 

rm rootfs/user -rf
cp user-control rootfs/user -rf
cp mo-get-ir.bin rootfs/lib/firmware/cm0.bin
mksquashfs rootfs rootfs-control.sfs


mkfs.jffs2 -n -s 512 -e 16KiB -d execute-server -o execute-server.jffs2
mkfs.jffs2 -n -s 512 -e 16KiB -d execute-client -o execute-client.jffs2
mkfs.jffs2 -n -s 512 -e 16KiB -d execute-control -o execute-control.jffs2

#server
dd if=/dev/zero of=server.img bs=1024 count=8192

dd if=boot.img of=server.img bs=1024 count=320
dd if=uImage of=server.img seek=384 bs=1024 count=2560
dd if=rootfs-server.sfs of=server.img seek=2944 bs=1024 count=2560
dd if=execute-server.jffs2 of=server.img seek=5504 bs=1024 count=2688

#dd if=reset.bin of=server.img bs=1024 count=4
#dd if=u-boot.bin of=server.img seek=128 bs=1024 count=192
#dd if=uImage of=server.img seek=384 bs=1024 count=2560
#dd if=rootfs-server.sfs of=server.img seek=2944 bs=1024 count=4224
#dd if=execute-server.jffs2 of=server.img seek=7168 bs=1024 count=1024

#client
dd if=/dev/zero of=client.img bs=1024 count=8192

dd if=boot.img of=client.img bs=1024 count=320
dd if=uImage of=client.img seek=384 bs=1024 count=2560
dd if=rootfs-client.sfs of=client.img seek=2944 bs=1024 count=2560
dd if=execute-client.jffs2 of=client.img seek=5504 bs=1024 count=2688

#control
dd if=/dev/zero of=control.img bs=1024 count=8192

dd if=boot.img of=control.img bs=1024 count=320
dd if=uImage of=control.img seek=384 bs=1024 count=2560
dd if=rootfs-control.sfs of=control.img seek=2944 bs=1024 count=2560
dd if=execute-control.jffs2 of=control.img seek=5504 bs=1024 count=2688

#dd if=reset.bin of=client.img bs=1024 count=4
#dd if=u-boot.bin of=client.img seek=128 bs=1024 count=192
#dd if=uImage of=client.img seek=384 bs=1024 count=2560
#dd if=rootfs-client.sfs of=client.img seek=2944 bs=1024 count=4224
#dd if=execute-client.jffs2 of=client.img seek=7168 bs=1024 count=1024

mv execute-server.jffs2 ./tmp
mv execute-client.jffs2 ./tmp
mv execute-control.jffs2 ./tmp

mv rootfs-server.sfs ./tmp
mv rootfs-client.sfs ./tmp
mv rootfs-control.sfs ./tmp

cp client.img /tftpboot/
cp server.img /tftpboot/
cp control.img /tftpboot/
