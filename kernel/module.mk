DIR := kernel
# SUBMODULES := lib/kernelc

INCLUDE_DIRS += $(DIR)/include
OBJS += $(addprefix $(DIR)/, \
	kernel/kernel.o \
)

include $(patsubst %, $(DIR)/%/module.mk, $(SUBMODULES))