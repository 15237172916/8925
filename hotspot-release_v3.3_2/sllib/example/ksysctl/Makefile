include ../../rule/compile.mk

ifeq ($(HIDE),@)
	QUIET := -s
endif
SUB_DIRS =  viu-vpu0h264enc-mdev-rtsp \
            rtsp-mdev-vpu0-vpp_pv \
			control-box
.PHONY: all $(SUB_DIRS)
all: $(SUB_DIRS)

$(SUB_DIRS):
	@cd $@;$(MAKE) $(TARGET) $(QUIET)

clean:
	@$(MAKE) TARGET=clean $(QUIET)

install:
	@$(MAKE) TARGET=install $(QUIET)
