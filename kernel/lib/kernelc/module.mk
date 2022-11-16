DIR := kernel/lib/kernelc
SUBMODULES :=

INCLUDE_DIRS += $(DIR)/include
LIBS += -lkernelc
LIB_DIRS += $(DIR)

OBJS_$(DIR) := $(addprefix $(DIR)/, \
	stdio/printf.o \
	stdio/putchar.o \
	stdio/puts.o \
	stdlib/abort.o \
	string/memcmp.o \
	string/memcpy.o \
	string/memmove.o \
	string/memset.o \
	string/strlen.o \
	builtin/__stack_chk_fail.o \
)

CLEAN += $(OBJS_$(DIR)) $(OBJS_$(DIR):.o=.d)

# the implementation of the stack protector must itself not use the stack protector
# (prevent infinite loop)
$(DIR)/builtin/__stack_chk_fail.o: CXXFLAGS += -fno-stack-protector

$(DIR)/libkernelc.a: $(OBJS_$(DIR))

include $(patsubst %, %/module.mk, $(SUBMODULES))