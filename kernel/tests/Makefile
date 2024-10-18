BIN := test_kernel
CXX := clang++
GTEST_PATH := ../../external/gtest
SRCS := \
	test_Terminal.cpp \
	../kernel/Terminal.cpp \
	../kernel/limine_features.cpp \

OBJS := $(SRCS:.cpp=.o)

CXXFLAGS := \
	-Wall \
	-Werror \
	-Wpedantic \
	-std=c++20 \

INCLUDE_DIRS := \
	$(GTEST_PATH)/googletest/include \
	../include \
	../lib/kernelcpp/include

CPPFLAGS += $(addprefix -I, $(INCLUDE_DIRS))

LIB_DIRS := $(GTEST_PATH)/build/lib
LDFLAGS += $(addprefix -L, $(LIB_DIRS))

LIB_NAMES := gtest gtest_main
LDFLAGS += $(addprefix -l, $(LIB_NAMES))

release: CXXFLAGS += -O3
release: $(BIN)

debug: CXXFLAGS += -g
debug: $(BIN)

$(BIN): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(BIN)

gtest:
	rm -rf $(GTEST_PATH)/build
	mkdir -p $(GTEST_PATH)/build
	cmake -B $(GTEST_PATH)/build -S $(GTEST_PATH)
	cd $(GTEST_PATH)/build
	make -C $(GTEST_PATH)/build


.PHONY: all clean gtest
