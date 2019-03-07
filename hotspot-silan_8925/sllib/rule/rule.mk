PROJECT_DIR := $(SLLIB_DIR)

ifndef sources
	sources := $(wildcard *.c *.S)

ifneq ($(HIDE),@)
	test := $(warning sources not defined, by default use sources = $(sources) instead.)
endif

endif

ifndef library
	library := libsl$(notdir $(CURDIR))

ifneq ($(HIDE),@)
	test := $(warning library not defined, by default use library = $(library) instead.)
endif

endif

OBJS := $(subst .c,.o, $(filter %.c, $(sources))) \
		$(subst .S,.o, $(filter %.S, $(sources))) \
		$(sources:.c=.o)

DEPS :=  $(subst .c,.d, $(filter %.c, $(sources))) \
		 $(subst .S,.d, $(filter %.S, $(sources))) 

library_full := $(PROJECT_DIR)/lib/$(library)

include_dirs := $(PROJECT_DIR)/include $(PROJECT_DIR)/src/include $(PROJECT_DIR)/ksysctl/include . $(EXTINC) 
library_dirs := $(PROJECT_DIR)/lib

CFLAGS += -fPIC -g -mcpu=ck810f -mhard-float -Wall $(addprefix -I, $(include_dirs)) -MD
LDFLAGS += -fPIC -g -mcpu=ck810f -mhard-float  -Wall -shared $(addprefix -L, $(library_dirs)) $(addprefix -lsl, $(DEPLIBS))
ARFLAGS = -rc

make-dir = $(if $(wildcard $1), , $(MKDIR) -p $1)

ifndef INSTALL_DIR
INSTALL_DIR = $(PREFIX)/lib
endif

%.o:%.c %.d
	$(CC) -c $(CFLAGS) `pwd`/$< -o $@
%.d:%.c
	$(CC)  $(CFLAGS) -MM -MD -c `pwd`/$< -o $@


.PHONY:library
library:$(library_full)

$(library_full):$(OBJS)
	$(HIDE)$(call make-dir, $(dir $@))
	$(HIDE)$(CC) $(LDFLAGS) -o $@.so $^
	$(HIDE)$(AR) $(ARFLAGS) $@.a $^
	$(STRIP) -S $@.a 
	$(STRIP) -S $@.so 

sinclude $(sources:.c=.d)

.PHONY:clean
clean:
	$(HIDE)$(RM) $(OBJS) $(library_full).*
	find . -name '*.d' | xargs rm -f

ifneq "$(MAKECMDGOALS)" "clean"
	-include $(DEPS)
endif

install:$(library_full)
	$(HIDE)$(call make-dir, $(INSTALL_DIR))
	$(HIDE)$(CP) $(library_full).* $(INSTALL_DIR)/.
	$(HIDE)$(call make-dir, $(PREFIX)/include)
	$(HIDE)$(CP) $(PROJECT_DIR)/include/* $(PREFIX)/include/.

