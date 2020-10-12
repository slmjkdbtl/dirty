# wengwengweng

ifeq ($(OS),Windows_NT)
TARGET := windows
endif

ifeq ($(shell uname -s),Darwin)
TARGET := macos
endif

ifeq ($(shell uname -s),Linux)
TARGET := linux
endif

CC := cc
AR := ar

DEMO := test

SRC_PATH := src
OBJ_PATH := build/obj
BIN_PATH := build/bin
EXT_INC_PATH := ext/inc
EXT_LIB_PATH := ext/lib
DIRTY_PATH := ..
BIN_TARGET := $(BIN_PATH)/dirty

C_FLAGS += -I $(EXT_INC_PATH)
C_FLAGS += -I $(DIRTY_PATH)/inc
C_FLAGS += -Wall
C_FLAGS += -std=c99

ifeq ($(MODE),release)
C_FLAGS += -O3
endif

ifeq ($(TARGET),macos)
C_FLAGS += -ObjC
endif

LD_FLAGS += -L $(EXT_LIB_PATH)
LD_FLAGS += -L $(DIRTY_PATH)/build/lib/$(TARGET)
LD_FLAGS += -L $(DIRTY_PATH)/ext/lib/$(TARGET)
LD_FLAGS += -l iconv
LD_FLAGS += -l SDL2
LD_FLAGS += -l lua
LD_FLAGS += -l dirty

ifeq ($(TARGET),macos)
LD_FLAGS += -framework CoreFoundation
LD_FLAGS += -framework OpenGL
LD_FLAGS += -framework Cocoa
LD_FLAGS += -framework Carbon
LD_FLAGS += -framework IOKit
LD_FLAGS += -framework CoreVideo
LD_FLAGS += -framework CoreAudio
LD_FLAGS += -framework AudioToolbox
LD_FLAGS += -framework Metal
LD_FLAGS += -framework ForceFeedback
endif

AR_FLAGS += -rc

SRC_FILES := $(wildcard $(SRC_PATH)/*.c)
OBJ_FILES := $(patsubst $(SRC_PATH)/%.c, $(OBJ_PATH)/%.o, $(SRC_FILES))

.PHONY: bin
bin: $(BIN_TARGET)

$(BIN_TARGET): $(OBJ_FILES)
	@mkdir -p $(BIN_PATH)
	$(CC) $(LD_FLAGS) $^ -o $@

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	@mkdir -p $(OBJ_PATH)
	$(CC) $(C_FLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf build

.PHONY: run
run: $(BIN_TARGET)
	./$(BIN_TARGET) demo

.PHONY: update
update:
	git submodule foreach git pull origin master

