#!/bin/sh
insmod slcommon.ko log_levels=1
insmod slksysctl.ko
insmod slmemorydev.ko
insmod slvpu.ko
insmod slvpp.ko
insmod slviu.ko
insmod slaudio.ko
insmod sldsp-ir.ko
#insmod slnulldev.ko
#insmod silan_usb_host.ko

