DIR := kernel
SUBMODULES := lib/kpp

INCLUDE_DIRS += $(DIR)/include
OBJS += $(addprefix $(DIR)/, \
	kernel/APICManager.o \
	kernel/Frame.o \
	kernel/frame_allocator.o \
	kernel/gdt.o \
	kernel/idt.o \
	kernel/IDTStructure.o \
	kernel/kernel.o \
	kernel/limine_features.o \
	kernel/load_ptbr.o \
	kernel/LocalAPIC.o \
	kernel/main.o \
	kernel/paging.o \
	kernel/PageTree.o \
	kernel/PageTreeNode.o \
	kernel/processor.o \
	kernel/reload_segment_registers.o \
	kernel/TableDescriptor.o \
	kernel/Terminal.o \
	kernel/tests.o \
	kernel/vmm.o \
)

include $(patsubst %, $(DIR)/%/module.mk, $(SUBMODULES))
