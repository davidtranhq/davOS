.DEFAULT_GOAL := all

# name of the OS
NAME = davos
# kernel binary
BIN = $(NAME).elf
# bootable CD-ROM
ISO = $(NAME).iso
# compiler target
TARGET = x86_64-elf
# directory containing architecture specific source files (kernel/<ARCH_DIR>)
ARCH_DIR = x86-64
# modules (sub-projects) contained in this project
MODULES = kernel

# popoulate the following variables with the module.mk files from each module
OBJS :=
LIBS :=
INCLUDE_DIRS :=
LIB_DIRS :=
CLEAN :=
include $(patsubst %, %/module.mk, $(MODULES))

CPPFLAGS += $(addprefix -I, $(INCLUDE_DIRS))

CXXFLAGS += \
	-std=c++17 \
	-ffreestanding \
	-fstack-protector \
	-fno-stack-check \
	-fno-lto \
	-fno-pie \
	-fno-pic \
	-m64 \
	-march=x86-64 \
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
LDFLAGS += \
	-nostdlib \
	-static \
	-melf_x86_64 \
	-z max-page-size=0x1000 \
	-T kernel/arch/$(ARCH_DIR)/linker.ld \

HEADER_DEPS = $(OBJS:.o=.d)

CXX = $(TARGET)-g++
AR = $(TARGET)-ar
LD = $(TARGET)-ld

include phony.mk

all: $(BIN)

$(BIN): $(OBJS)
	$(LD) $^ $(LDFLAGS) $(LIBS) -o $@

%.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS) $(CPPFLAGS) -o $@

%.a:
	$(AR) rcs $@ $^

-include $(HEADER_DEPS) 