A summary of how the build system for this project works.

The reasons I don't use recursive make are in Miller's 2008 paper [Recursive Make Considered Harmful](https://aegis.sourceforge.net/auug97.pdf).

The "main" Makefile is in the root called `Makefile`. To build the entire project, it recursively includes makefiles for each of the modules in the subdirectories specified by the `MODULES` variable. The makefile for each module is described in a `module.mk` file.

The contents of a `module.mk` file are as below:

```makefile
# The current directory (where the module.mk resides)
DIR := kernel/lib/kernelc
# Paths to submodules (subdirectories containing their own module.mk files)
SUBMODULES :=
# The name of the submodule binary
BIN_$(DIR) := kernelc

# Object files 
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

# Output path for this binary
BIN_PATH_$(DIR) := $(DIR)/lib$(BIN_$(DIR)).a

# These are variables initialized by the root Makefile, which we modify here. This is how the root Makefile
# "collects" all the dependencies needed to build the entire project.
INCLUDE_DIRS += $(DIR)/include
LIBS += $(BIN_PATH_$(DIR))
LIB_DIRS += $(DIR)
CLEAN += $(BIN_PATH_$(DIR)) $(OBJS_$(DIR)) $(OBJS_$(DIR):.o=.d)

# Custom flags for any of the object files in this submodule
$(DIR)/builtin/__stack_chk_fail.o: CXXFLAGS += -fno-stack-protector

# Listing the object files as a dependency for this submodule
$(BIN_PATH_$(DIR)): $(OBJS_$(DIR))

# Recurse into the other submodules
include $(patsubst %, %/module.mk, $(SUBMODULES))
```
