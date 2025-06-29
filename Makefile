.DEFAULT_GOAL := all

# name of the OS
NAME = davos
# kernel binary
BIN = $(NAME).elf
# bootable CD-ROM
ISO = $(NAME).iso
# compiler target
TARGET = x86_64-elf
# The architecture for which to compile this kernel for
ARCH = x86-64
# modules (sub-projects) contained in this project
MODULES = kernel

CXXFLAGS = -Wall -Werror -Wpedantic -g

########## NON-CONFIGURABLE STUFF BELOW ##########

# popoulate the following variables with the module.mk files from each module
OBJS :=
LIBS :=
INCLUDE_DIRS :=
LIB_DIRS :=
LIB_NAMES :=
CLEAN :=
include $(patsubst %, %/module.mk, $(MODULES))

CPPFLAGS += $(addprefix -I, $(INCLUDE_DIRS))

CXXFLAGS += \
	-std=c++20 \
	-ffreestanding \
	-fstack-protector \
	-fno-stack-check \
	-fno-lto \
	-fno-pie \
	-fno-pic \
	-fno-use-cxa-atexit \
	-fno-exceptions \
	-m64 \
	-march=$(ARCH) \
	-mabi=sysv \
	-mno-80387 \
	-mno-mmx \
	-mno-sse \
	-mno-sse2 \
	-mno-red-zone \
	-mcmodel=kernel \
	-MMD \
	-MP \

LDFLAGS += $(addprefix -L, $(LIB_DIRS))
LDFLAGS += $(addprefix -l, $(LIB_NAMES))
LDFLAGS += \
	-nostdlib \
	-static \
	-melf_x86_64 \
	-z max-page-size=0x1000 \
	-T kernel/arch/$(ARCH)/linker.ld \

HEADER_DEPS = $(OBJS:.o=.d)

CXX = $(TARGET)-g++
AR = $(TARGET)-ar
LD = $(TARGET)-ld

include phony.mk

all: $(BIN)

$(BIN): $(OBJS) $(LIBS)
	$(LD) $^ $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS) $(CPPFLAGS) -o $@

%.o: %.S
	$(CXX) -c $< $(CXXFLAGS) $(CPPFLAGS) -o $@

%.a:
	$(AR) rcs $@ $^

-include $(HEADER_DEPS) 
