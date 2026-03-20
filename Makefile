CXX     := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude -Ithird_party
LDFLAGS  := -lglfw -lGL -ldl

SRC_DIRS := src/engine src/renderer src/world src/game src/ui
SRCS     := src/main.cpp \
            $(wildcard src/engine/*.cpp) \
            $(wildcard src/renderer/*.cpp) \
            $(wildcard src/world/*.cpp) \
            $(wildcard src/game/*.cpp) \
            $(wildcard src/ui/*.cpp) \
            third_party/glad/glad.c

BUILD_DIR := build
OBJS      := $(patsubst %.cpp, $(BUILD_DIR)/%.o, \
             $(patsubst %.c,   $(BUILD_DIR)/%.o, $(SRCS)))

TARGET := ZombieHorde

.PHONY: all debug release clean

all: debug

debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

release: CXXFLAGS += -O2 -DNDEBUG
release: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	gcc -Ithird_party -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
