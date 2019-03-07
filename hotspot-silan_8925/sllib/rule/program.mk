PROJECT_DIR := $(SLLIB_DIR)
ifndef sources
	sources := $(wildcard *.c *.S) $(DEPSRCS)
ifneq ($(HIDE),@)
	test := $(warning sources not defined, by default use sources = $(sources) instead.)
endif
endif

ifndef program
	program := $(notdir $(CURDIR))
ifneq ($(HIDE),@)
	test := $(warning program not defined, by default use program = $(library) instead.)
endif
endif

OBJS := $(subst .c,.o, $(filter %.c, $(sources))) \
		$(subst .S,.o, $(filter %.S, $(sources))) \
		$(sources:.c=.o)

DEPS :=  $(subst .c,.d, $(filter %.c, $(sources))) \
		 $(subst .S,.d, $(filter %.S, $(sources))) 

program_full := $(PROJECT_DIR)/bin/$(program)

include_dirs := . $(PROJECT_DIR)/include \
	$(PROJECT_DIR)/include/IL \
	$(PROJECT_DIR)/include/AL \
	$(PROJECT_DIR)/example/common/include \
	$(PROJECT_DIR)/3rd/include \
	$(PROJECT_DIR)/src/include \
	$(PROJECT_DIR)/3rd/libosip2-4.1.0/include \
	$(PROJECT_DIR)/3rd/libeXosip2-4.1.0/ \
	$(PROJECT_DIR)/3rd/libeXosip2-4.1.0/include \
	$(EXTINC)
library_dirs := $(PROJECT_DIR)/lib $(PROJECT_DIR)/3rd/lib $(PROJECT_DIR)/3rd/live555/lib $(PROJECT_DIR)/3rd/libosip2-4.1.0/lib $(PROJECT_DIR)/3rd/libeXosip2-4.1.0/lib 

#CFLAGS += -fPIC -Wall -mhard-float $(addprefix -I, $(include_dirs))
#LDFLAGS += -fPIC -Wall $(addprefix -L, $(library_dirs)) $(addprefix -l, $(DEPLIBS))  -Wl,-rpath-link=$(PROJECT_DIR)/lib 
CFLAGS += -fPIC -g -Wall -static -mhard-float $(addprefix -I, $(include_dirs))
LDFLAGS += -fPIC -g -Wall -static $(addprefix -L, $(library_dirs)) $(addprefix -l, $(DEPLIBS))  -Wl,-rpath-link=$(PROJECT_DIR)/lib 
#CFLAGS += -fPIC -Wall  -mhard-float $(addprefix -I, $(include_dirs))
#LDFLAGS += -fPIC -Wall $(addprefix -L, $(library_dirs)) $(addprefix -l, $(DEPLIBS))  -Wl,-rpath-link=$(PROJECT_DIR)/lib 
make-dir = $(if $(wildcard $1), , $(MKDIR) -p $1)

ifndef INSTALL_DIR
INSTALL_DIR = $(PREFIX)/bin
endif

%.o:%.c %.d
	$(CC) -c $(CFLAGS) `pwd`/$< -o $@
%.d:%.c
	$(CC)  $(CFLAGS) -MM -MD -c `pwd`/$< -o $@

.PHONY:program
program:$(program_full)
$(program_full):$(OBJS)
	$(HIDE)$(call make-dir, $(dir $@))
ifeq ($(CPP),$(CROSS_COMPILE)g++)
	$(HIDE)$(CPP) $(LDFLAGS) -Xlinker "-(" $(addprefix -l, $(DEPLIBS)) $^ -Xlinker "-)" -o $@ 
else
	$(HIDE)$(CC) $(LDFLAGS) -Xlinker "-(" $(addprefix -l, $(DEPLIBS)) $^ -Xlinker "-)" -o $@ 
endif

#$(HIDE)$(CC) $^ $(LDFLAGS) -o $@ 
#$(HIDE)$(CC) $(LDFLAGS) -o $@ $^

sinclude $(sources:.c=.d)

.PHONY:clean
clean:
	$(HIDE)$(RM) $(OBJS) $(program_full)
	find . -name '*.d' | xargs rm 

ifneq "$(MAKECMDGOALS)" "clean"
	-include $(DEPS)
endif

install:$(program_full)
	$(HIDE)$(call make-dir, $(INSTALL_DIR))
	$(HIDE)$(CP) $(program_full) $(INSTALL_DIR)/.
