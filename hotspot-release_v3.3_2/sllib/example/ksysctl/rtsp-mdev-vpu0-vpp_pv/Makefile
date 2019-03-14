
SLLIB_ROOT:=../../..
include ../../../rule/compile.mk
DEPSRCS := ../../../example/common/sl_msg.c ../../../example/common/handle.c
DEPLIBS := jpeg slmmutil slksysctl liveMedia  groupsock UsageEnvironment  BasicUsageEnvironment slrtsp slos pthread dl m
CFLAGS += -DCONFIG_SILAN_KSYSCTL

include ../../../rule/program.mk
