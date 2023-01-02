DIR := kernel
SUBMODULES := lib/kernelc lib/3rdparty

INCLUDE_DIRS += $(DIR)/include
OBJS += $(addprefix $(DIR)/, \
	kernel/idt.o \
	kernel/IDT.o \
	kernel/TableDescriptor.o \
	kernel/kernel.o \
	kernel/main.o \
	kernel/terminal.o \
	kernel/tests.o \
)

include $(patsubst %, $(DIR)/%/module.mk, $(SUBMODULES))