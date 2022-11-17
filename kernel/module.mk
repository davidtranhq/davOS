DIR := kernel
SUBMODULES := lib/kernelc

INCLUDE_DIRS += $(DIR)/include
OBJS += $(addprefix $(DIR)/, \
	kernel/kernel.o \
	kernel/main.o \
	kernel/terminal.o \
)

include $(patsubst %, $(DIR)/%/module.mk, $(SUBMODULES))