BIN := test_kernelcpp
CXX := g++
SRCS := main.cpp
OBJS := $(SRCS:.cpp=.o)

CXXFLAGS := \
	-Wall \
	-Werror \
	-Wpedantic \
	-O3 \
	-std=c++20 \

INCLUDE_DIRS := ../../../../external/gtest/googletest/include
CPPFLAGS += $(addprefix -I, $(INCLUDE_DIRS))

LIB_DIRS := ../../../../external/gtest/build/lib
LDFLAGS += $(addprefix -L, $(LIB_DIRS))

LIB_NAMES := gtest gtest_main
LDFLAGS += $(addprefix -l, $(LIB_NAMES))

all: $(BIN)

$(BIN): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(BIN)

.PHONY: all clean

