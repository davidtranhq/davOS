DIR := kernel
SUBMODULES := lib/kernelc lib/3rdparty

INCLUDE_DIRS += $(DIR)/include
OBJS += $(addprefix $(DIR)/, \
	kernel/frame_allocator.o \
	kernel/gdt.o \
	kernel/idt.o \
	kernel/IDTStructure.o \
	kernel/kernel.o \
	kernel/limine_features.o \
	kernel/load_ptbr.o \
	kernel/main.o \
	kernel/PageTree.o \
	kernel/PageTreeNode.o \
	kernel/reload_segment_registers.o \
	kernel/TableDescriptor.o \
	kernel/terminal.o \
	kernel/tests.o \
	kernel/vmm.o \
)

include $(patsubst %, $(DIR)/%/module.mk, $(SUBMODULES))