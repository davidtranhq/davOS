CXX = i686-elf-g++
WARNINGS = -Wfatal-errors -Wall -Wextra -Wpedantic -Wconversion -Wshadow
CXX_FLAGS = -ffreestanding -O2 -fno-rtti -fno-exceptions -nostdlib

NAME = davos
BIN = $(NAME).bin
ISO = $(NAME).iso

# Put all auto generated stuff to this build dir.
BUILD_DIR = build
# output directory containing the ISO
ISO_DIR = iso
# directory containing test scripts
SCRIPT_DIR = scripts


# config folder holding configuration files for linker/GRUB
CONFIG = config
# List of all .cpp source files.
CPP = $(wildcard src/*.cpp)
# All .o files go to build dir.
OBJ = $(CPP:src/%.cpp=$(BUILD_DIR)/%.o)
# Gcc/Clang will create these .d files containing dependencies.
DEP = $(OBJ:%.o=%.d)
# Shows where to find the header files
INCLUDE = cxxshim/stage2/include
# Libraries to link
LIBRARY = -lgcc
LINKER =

all : iso check-multiboot

iso : $(BIN)
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(BIN) $(ISO_DIR)/boot/$(BIN)
	cp $(CONFIG)/grub.cfg $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) $(ISO_DIR)

$(BIN) : $(OBJ) $(BUILD_DIR)/boot.o
	$(CXX) -T $(CONFIG)/linker.ld $(CXX_FLAGS) $^ $(LIBRARY) $(LINKER) -o $@

# Include all .d files
-include $(DEP)

# Compile bootloader assembly file
$(BUILD_DIR)/boot.o : src/boot.s
	i686-elf-as $^ -o $@
	
	
# Build target for every single object file.
# The potential dependency on header files is covered
# by calling `-include $(DEP)`.
$(BUILD_DIR)/%.o : src/%.cpp
	mkdir -p $(BUILD_DIR)
# The -MMD flags additionaly creates a .d file with
# the same name as the .o file.
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -MMD -c $< -o $@

.PHONY : clean check-multiboot

clean :
	rm -rf $(BUILD_DIR) $(ISO_DIR)

# check if the compiled binary is multiboot 1 compliant
check-multiboot :
	$(SCRIPT_DIR)/check-multiboot.sh $(BIN)