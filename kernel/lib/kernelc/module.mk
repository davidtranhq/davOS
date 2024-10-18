DIR := kernel/lib/kernelc
SUBMODULES :=
BIN_$(DIR) := kernelc

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
	string/strncmp.o \
	builtin/__stack_chk_fail.o \
)

BIN_PATH_$(DIR) := $(DIR)/lib$(BIN_$(DIR)).a

INCLUDE_DIRS += $(DIR)/include
LIBS += $(BIN_PATH_$(DIR))
LIB_DIRS += $(DIR)
CLEAN += $(BIN_PATH_$(DIR)) $(OBJS_$(DIR)) $(OBJS_$(DIR):.o=.d)

# the implementation of the stack protector must itself not use the stack protector
# (prevent infinite loop)
$(DIR)/builtin/__stack_chk_fail.o: CXXFLAGS += -fno-stack-protector

# dependencies for building the library binary
$(BIN_PATH_$(DIR)): $(OBJS_$(DIR))

include $(patsubst %, %/module.mk, $(SUBMODULES))
