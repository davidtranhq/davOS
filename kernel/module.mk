DIR := kernel
SUBMODULES := lib/kernelc lib/3rdparty

INCLUDE_DIRS += $(DIR)/include
OBJS += $(addprefix $(DIR)/, \
	kernel/gdt.o \
	kernel/idt.o \
	kernel/IDTStructure.o \
	kernel/kernel.o \
	kernel/main.o \
	kernel/reload_segment_registers.o \
	kernel/TableDescriptor.o \
	kernel/terminal.o \
	kernel/tests.o \
)

include $(patsubst %, $(DIR)/%/module.mk, $(SUBMODULES))