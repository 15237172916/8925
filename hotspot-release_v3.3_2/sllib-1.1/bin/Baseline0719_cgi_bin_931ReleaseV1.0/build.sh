#csky-abiv2-linux-gcc -o get_status.cgi get_status.c
#csky-abiv2-linux-gcc -o get_net.cgi get_net.c
#csky-abiv2-linux-gcc -o set_net.cgi set_net.c
#csky-abiv2-linux-gcc -o get_codec.cgi get_codec.c
#csky-abiv2-linux-gcc -o set_protocol.cgi set_protocol.c
#csky-abiv2-linux-gcc -o set_codec.cgi set_codec.c
#csky-abiv2-linux-gcc -o get_sys.cgi get_sys.c
#csky-abiv2-linux-gcc -o sys_reboot.cgi sys_reboot.c
#csky-abiv2-linux-gcc -o sys_default.cgi sys_default.c
#csky-abiv2-linux-gcc -o upload.cgi upload.c
#csky-abiv2-linux-gcc -o get_bar.cgi get_bar.c

#
csky-abiv2-linux-gcc -o set_txRename.cgi set_txRename.c
csky-abiv2-linux-gcc -o set_rxRename.cgi set_rxRename.c
csky-abiv2-linux-gcc -o set_multicast.cgi set_multicast.c

csky-abiv2-linux-gcc -o get_txRename.cgi get_txRename.c
csky-abiv2-linux-gcc -o get_rxRename.cgi get_rxRename.c
csky-abiv2-linux-gcc -o get_multicast.cgi get_multicast.c

csky-abiv2-linux-gcc -o sys_reboot.cgi sys_reboot.c
csky-abiv2-linux-gcc -o upload.cgi test.c
csky-abiv2-linux-gcc -o get_bar.cgi get_bar.c
csky-abiv2-linux-gcc -o get_sys.cgi get_sys.c
#csky-abiv2-linux-gcc -o set_net.cgi set_net.c

cp *.cgi ../web/cgi-bin
#cp *.cgi /home/eric/work/sllib/bin/9001html/htmlarm/cgi-bin
