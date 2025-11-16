CXX ?= g++
SDL2_CFLAGS := $(shell pkg-config --cflags sdl2 2>/dev/null)
SDL2_LIBS := $(shell pkg-config --libs sdl2 2>/dev/null)
ifeq ($(SDL2_CFLAGS),)
SDL2_CFLAGS := $(shell sdl2-config --cflags 2>/dev/null)
SDL2_LIBS := $(shell sdl2-config --libs 2>/dev/null)
endif
ifeq ($(SDL2_CFLAGS),)
$(error SDL2 development files not found. Please install libsdl2-dev)
endif
CXXFLAGS ?= -std=c++20 -Wall -Wextra -pedantic -O2 $(SDL2_CFLAGS)
LDFLAGS ?= $(SDL2_LIBS)

SRC_DIRS := src src/text sprites
SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
OBJS := $(SRCS:.cpp=.o)
TARGET := turbo_v12

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean

