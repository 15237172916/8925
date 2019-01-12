include ../rule/compile.mk

ifeq ($(HIDE),@)
	QUIET := -s
endif

SUB_DIRS = \
			gpio         \
			i2c          \
			ir           \
			led          \
			pwm          \
			rtc          \
			spi          \
			uart         \
			watchdog	\
			otp         \
			ksysctl      

.PHONY: all $(SUB_DIRS)
all: $(SUB_DIRS)

$(SUB_DIRS):
	@cd $@;$(MAKE) $(TARGET) $(QUIET)

clean:
	@$(MAKE) TARGET=clean $(QUIET)

install:
	@$(MAKE) TARGET=install $(QUIET)
