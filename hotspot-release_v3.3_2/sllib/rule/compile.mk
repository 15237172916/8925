ifndef PLATFORM
PLATFORM := linux
endif

ifeq ($(PLATFORM),linux)
	CROSS_COMPILE := csky-abiv2-linux-
else
	test := $(error PLATFORM $(PALTFORM) not support.)
endif

CC = $(CROSS_COMPILE)gcc
CPP = $(CROSS_COMPILE)g++
AR = $(CROSS_COMPILE)ar
STRIP = $(CROSS_COMPILE)strip
MV := mv -f
RM := rm -f
CP := cp -rf
SED := sed
MKDIR := mkdir
HIDE := @
QUIET := -s

ifeq ($(SLLIB_ROOT), )
	SLLIB_ROOT:=../..
endif
SDK_ROOT:=$(shell cd $(PWD)/$(SLLIB_ROOT)/../; pwd)
SDK_ROOTFS:=$(SDK_ROOT)/rootfs
PREFIX:=$(SDK_ROOTFS)/usr
SLLIB_DIR := $(SDK_ROOT)/sllib
#CFLAGS += -EL -O2 -DSYSCTL
#LDFLAGS += -EL
CFLAGS += -EL -g -DSYSCTL -mcpu=ck810f -mhard-float
LDFLAGS += -EL -g -mcpu=ck810f -mhard-float
