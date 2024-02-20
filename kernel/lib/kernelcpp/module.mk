DIR := kernel/lib/kernelcpp
SUBMODULES :=
BIN_$(DIR) := kernelcpp

OBJS_$(DIR) := $(addprefix $(DIR)/, \
)

BIN_PATH_$(DIR) := $(DIR)/lib$(BIN_$(DIR)).a

INCLUDE_DIRS += $(DIR)/include
LIBS += $(BIN_PATH_$(DIR))
LIB_DIRS += $(DIR)
CLEAN += $(BIN_PATH_$(DIR)) $(OBJS_$(DIR)) $(OBJS_$(DIR):.o=.d)

# dependencies for building the library binary
$(BIN_PATH_$(DIR)): $(OBJS_$(DIR))

include $(patsubst %, %/module.mk, $(SUBMODULES))