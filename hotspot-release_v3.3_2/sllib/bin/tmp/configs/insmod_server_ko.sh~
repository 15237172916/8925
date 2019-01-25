#! /bin/sh

#insmod slcommon.ko log_levels=1
if [ -f /tmp/update/ko/slcommon.ko ]; then
    echo "/tmp/update/ko/slcommon.ko OK"
    insmod /tmp/update/ko/slcommon.ko log_levels=1
else
    echo "/user/update/ko/slcommon.ko OK"
    insmod /user/update/ko/slcommon.ko log_levels=1
fi

#insmod slksysctl.ko
if [ -f /tmp/update/ko/slksysctl.ko ]; then
    echo "/tmp/update/ko/slksysctl.ko OK"
    insmod /tmp/update/ko/slksysctl.ko
else
    echo "/user/update/ko/slksysctl.ko OK"
    insmod /user/update/ko/slksysctl.ko
fi

#insmod slmemorydev.ko
if [ -f /tmp/update/ko/slmemorydev.ko ]; then
    echo "/tmp/update/ko/slmemorydev.ko OK"
    insmod /tmp/update/ko/slmemorydev.ko
else
    echo "/user/update/ko/slmemorydev.ko OK"
    insmod /user/update/ko/slmemorydev.ko
fi

#insmod slvpu.ko
if [ -f /tmp/update/ko/slvpu.ko ]; then
    echo "/tmp/update/ko/slvpu.ko OK"
    insmod /tmp/update/ko/slvpu.ko 
else
    echo "/user/update/ko/slvpu.ko OK"
    insmod /user/update/ko/slvpu.ko
fi

#insmod slvpp.ko
if [ -f /tmp/update/ko/slvpp.ko ]; then
    echo "/tmp/update/ko/slvpp.ko OK"
    insmod /tmp/update/ko/slvpp.ko
else
    echo "/user/update/ko/slvpp.ko OK"
    insmod /user/update/ko/slvpp.ko
fi

#insmod slviu.ko
if [ -f /tmp/update/ko/slviu.ko ]; then
    echo "/tmp/update/ko/slviu.ko OK"
    insmod /tmp/update/ko/slviu.ko
else
    echo "/user/update/ko/slviu.ko OK"
    insmod /user/update/ko/slviu.ko
fi

#insmod slaudio.ko
if [ -f /tmp/update/ko/slaudio.ko ]; then
    echo "/tmp/update/ko/slaudio.ko OK"
    insmod /tmp/update/ko/slaudio.ko
else
    echo "/user/update/ko/slaudio.ko OK"
    insmod /user/update/ko/slaudio.ko
fi

#insmod sldsp-ir.ko
if [ -f /tmp/update/ko/sldsp-ir.ko ]; then
    echo "/tmp/update/ko/sldsp-ir.ko OK"
    insmod /tmp/update/ko/sldsp-ir.ko
else
    echo "/user/update/ko/sldsp-ir.ko OK"
    insmod /user/update/ko/sldsp-ir.ko
fi



