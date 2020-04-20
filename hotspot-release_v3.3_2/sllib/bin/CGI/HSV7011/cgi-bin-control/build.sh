rm *.cgi

#index.html
csky-abiv2-linux-gcc -o get_status.cgi get_status.c

#SetNet.html
csky-abiv2-linux-gcc -o get_net.cgi get_net_control.c
csky-abiv2-linux-gcc -o set_net.cgi set_net_control.c

#SetMedia.html
csky-abiv2-linux-gcc -o get_codec.cgi get_codec.c
csky-abiv2-linux-gcc -o set_protocol.cgi set_protocol.c
csky-abiv2-linux-gcc -o set_codec.cgi set_codec.c

#System.html
csky-abiv2-linux-gcc -o get_sys.cgi get_sys.c
csky-abiv2-linux-gcc -o sys_reboot.cgi sys_reboot.c
csky-abiv2-linux-gcc -o sys_default.cgi sys_default.c
csky-abiv2-linux-gcc -o get_bar.cgi get_bar.c
csky-abiv2-linux-gcc -o upload.cgi test.c

#scene
csky-abiv2-linux-gcc -o scene_apply.cgi scene_apply.c
csky-abiv2-linux-gcc -o scene_save.cgi scene_save.c
csky-abiv2-linux-gcc -o scene_rename.cgi scene_rename.c
#display information
csky-abiv2-linux-gcc -o infoDisplay.cgi infoDisplay.c

#upload
csky-abiv2-linux-gcc -o upload_web.cgi upload_web.c
csky-abiv2-linux-gcc -o upload_ko.cgi upload_ko.c

#main.html
csky-abiv2-linux-gcc -o onloading.cgi onloading.c
csky-abiv2-linux-gcc -o changeModeName.cgi changeModeName.c
csky-abiv2-linux-gcc -o get_RX_line.cgi get_RX_line.c

#change name
csky-abiv2-linux-gcc -o changeRxName.cgi changeRxName.c
csky-abiv2-linux-gcc -o changeTxName.cgi changeTxName.c
csky-abiv2-linux-gcc -o changeMulticast.cgi changeMulticast.c

#get name
csky-abiv2-linux-gcc -o get_txRename.cgi get_txRename.c
csky-abiv2-linux-gcc -o get_rxRename.cgi get_rxRename.c
csky-abiv2-linux-gcc -o get_multicast.cgi get_multicast.c

#mode
csky-abiv2-linux-gcc -o modeStateNow.cgi modeStateNow.c
csky-abiv2-linux-gcc -o saveCurrentMode.cgi saveCurrentMode.c

rm ../../../Web/HSV7011/web/cgi-bin/*.cgi
cp *.cgi ../../../Web/HSV7011/web/cgi-bin
#cp upload_web.cgi ../tmp/web/noup/cgi-bin -rf
#cp *.cgi /home/eric/work/sllib/bin/9001html/htmlarm/cgi-bin
