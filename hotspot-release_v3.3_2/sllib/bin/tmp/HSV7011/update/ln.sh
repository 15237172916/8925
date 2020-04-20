#!/bin/sh
cd ./web

rm ./mime.types
ln -s /user/web/mime.types ./mime.types

#html
rm ./SystemE.html
ln -s /user/web/SystemE.html ./SystemE.html
rm ./System.html
ln -s /user/web/System.html ./System.html
rm ./SetNet.html
ln -s /user/web/SetNet.html ./SetNet.html
rm ./SetNetE.html
ln -s /user/web/SetNetE.html ./SetNetE.html
rm ./SetMedia0.html
ln -s /user/web/SetMedia0.html ./SetMedia0.html
rm ./SetMedia0E.html
ln -s /user/web/SetMedia0E.html ./SetMedia0E.html
rm ./indexE.html
ln -s /user/web/indexE.html ./indexE.html
rm ./index.html
ln -s /user/web/index.html ./index.html

#css
rm ./css/en -r
ln -s /user/web/css/en ./css/en
rm ./css/cn -r
ln -s /user/web/css/cn ./css/cn

#Js
rm ./Js/ajaxfileupload.js
ln -s /user/web/Js/ajaxfileupload.js ./Js/ajaxfileupload.js
rm ./Js/jquery-1.4.1.min.js
ln -s /user/web/Js/jquery-1.4.1.min.js ./Js/jquery-1.4.1.min.js
rm ./Js/platform.js
ln -s /user/web/Js/platform.js ./Js/platform.js
rm ./boa
ln -s /user/web/boa ./boa

#cgi
rm ./cgi-bin/upload_web.cgi
ln -s /user/web/cgi-bin/upload_web.cgi ./cgi-bin/upload_web.cgi
rm ./cgi-bin/upload_ko.cgi
ln -s /user/web/cgi-bin/upload_ko.cgi ./cgi-bin/upload_ko.cgi
rm ./cgi-bin/upload.cgi
ln -s /user/web/cgi-bin/upload.cgi ./cgi-bin/upload.cgi
rm ./cgi-bin/sys_default.cgi
ln -s /user/web/cgi-bin/sys_default.cgi ./cgi-bin/sys_default.cgi
rm ./cgi-bin/sys_reboot.cgi
ln -s /user/web/cgi-bin/sys_reboot.cgi ./cgi-bin/sys_reboot.cgi
rm ./cgi-bin/get_bar.cgi
ln -s /user/web/cgi-bin/get_bar.cgi ./cgi-bin/get_bar.cgi
rm ./cgi-bin/get_sys.cgi
ln -s /user/web/cgi-bin/get_sys.cgi ./cgi-bin/get_sys.cgi
rm ./cgi-bin/get_status.cgi
ln -s /user/web/cgi-bin/get_status.cgi ./cgi-bin/get_status.cgi
rm ./cgi-bin/set_protocol.cgi
ln -s /user/web/cgi-bin/set_protocol.cgi ./cgi-bin/set_protocol.cgi
rm ./cgi-bin/get_net.cgi
ln -s /user/web/cgi-bin/get_net.cgi ./cgi-bin/get_net.cgi
rm ./cgi-bin/set_net.cgi
ln -s /user/web/cgi-bin/set_net.cgi ./cgi-bin/set_net.cgi
rm ./cgi-bin/set_codec.cgi
ln -s /user/web/cgi-bin/set_codec.cgi ./cgi-bin/set_codec.cgi
rm ./cgi-bin/get_codec.cgi
ln -s /user/web/cgi-bin/get_codec.cgi ./cgi-bin/get_codec.cgi
#rm 
#ln -s /user/web/cgi-bin/ ./cgi-bin/

cd ..





