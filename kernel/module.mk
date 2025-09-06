DIR := kernel
SUBMODULES := lib/kpp

INCLUDE_DIRS += $(addprefix $(DIR)/, \
	include \
	include/arch/$(ARCH) \
)

OBJS += $(addprefix $(DIR)/, \
	src/APICManager.o \
	src/Frame.o \
	src/frame_allocator.o \
	src/gdt.o \
	src/idt.o \
	src/IDTStructure.o \
	src/kernel.o \
	src/limine_features.o \
	src/load_ptbr.o \
	src/LocalAPIC.o \
	src/main.o \
	src/paging.o \
	src/PageTree.o \
	src/PageTreeNode.o \
	src/processor.o \
	src/reload_segment_registers.o \
	src/TableDescriptor.o \
	src/Terminal.o \
	src/tests.o \
	src/thread.o \
	src/vmm.o \
	src/arch/$(ARCH)/cpu_context.o \
)

include $(patsubst %, $(DIR)/%/module.mk, $(SUBMODULES))
