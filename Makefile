# wengwengweng

# setting default targets
ifeq ($(OS),Windows_NT)
TARGET := windows
endif

ifeq ($(shell uname -s),Darwin)
TARGET := macos
endif

ifeq ($(shell uname -s),Linux)
TARGET := linux
endif

# programs / paths
CC := clang
AR := ar

ifeq ($(TARGET),web)
CC := emcc
endif

ifeq ($(TARGET),web)
AR := emar
endif

ifeq ($(TARGET),ios)
CC := xcrun -sdk iphoneos clang
endif

DEMO := tri

SRC_PATH := src
LUA_SRC_PATH := $(SRC_PATH)/lua
OBJ_PATH := build/obj/$(TARGET)
BIN_PATH := build/bin/$(TARGET)
DEMO_BIN_PATH := build/bin/$(TARGET)/demo
LIB_PATH := build/lib/$(TARGET)
INC_PATH := inc
DEMO_PATH := demo
EXT_INC_PATH := ext/inc
EXT_LIB_PATH := ext/lib
LIB_TARGET := $(LIB_PATH)/libdirty.a
BIN_TARGET := $(BIN_PATH)/dirty

# flags
C_FLAGS += -I $(INC_PATH)
C_FLAGS += -I $(EXT_INC_PATH)
C_FLAGS += -Wall
C_FLAGS += -Wpedantic
C_FLAGS += -std=c99
C_FLAGS += -D SOKOL_IMPL
C_FLAGS += -D SOKOL_GLCORE33
C_FLAGS += -D SOKOL_NO_ENTRY

ifeq ($(TARGET),macos)
C_FLAGS += -ObjC
endif

ifeq ($(TARGET),ios)
C_FLAGS += -arch armv7
C_FLAGS += -arch arm64
endif

LD_FLAGS += -L $(EXT_LIB_PATH)

ifeq ($(TARGET),macos)
LD_FLAGS += -framework Cocoa
LD_FLAGS += -framework QuartzCore
LD_FLAGS += -framework OpenGL
LD_FLAGS += -framework AudioToolbox
endif

ifdef RELEASE
C_FLAGS += -O3
LD_FLAGS += -O3
endif

AR_FLAGS += -rcs

# files
SRC_FILES := $(wildcard $(SRC_PATH)/*.c)
OBJ_FILES := $(patsubst $(SRC_PATH)/%.c, $(OBJ_PATH)/%.o, $(SRC_FILES))
LUA_SRC_FILES := $(wildcard $(LUA_SRC_PATH)/*.c)

DEMO_FILES := $(wildcard $(DEMO_PATH)/*.c)
DEMOS := $(patsubst $(DEMO_PATH)/%.c, %, $(DEMO_FILES))
DEMO_TARGETS := $(patsubst $(DEMO_PATH)/%.c, $(DEMO_BIN_PATH)/%, $(DEMO_FILES))

INSTALL_TOP := /usr/local
INSTALL_BIN := $(INSTALL_TOP)/bin
INSTALL_INC := $(INSTALL_TOP)/include
INSTALL_LIB := $(INSTALL_TOP)/lib

.PHONY: lib
lib: $(LIB_TARGET)

.PHONY: lua
lua: $(BIN_TARGET)

.PHONY: demos
demos: $(DEMO_TARGETS)
	rsync -a --delete $(DEMO_PATH)/res $(DEMO_BIN_PATH)/

.PHONY: run
run: $(DEMO_BIN_PATH)/$(DEMO)
	rsync -a --delete $(DEMO_PATH)/res $(DEMO_BIN_PATH)/
	./$(DEMO_BIN_PATH)/$(DEMO) $(ARGS)

.PHONY: run-lua
run-lua: $(BIN_TARGET) $(DEMO_PATH)/$(DEMO).lua
	./$(BIN_TARGET) $(DEMO_PATH)/$(DEMO).lua

$(DEMO_BIN_PATH)/%: $(DEMO_PATH)/%.c $(LIB_TARGET)
	@mkdir -p $(DEMO_BIN_PATH)
	$(CC) $(C_FLAGS) -L $(LIB_PATH) -ldirty $(LD_FLAGS) -o $@ $^

$(BIN_TARGET): $(LUA_SRC_FILES) $(LIB_TARGET)
	@mkdir -p $(BIN_PATH)
	$(CC) $(C_FLAGS) -L $(LIB_PATH) -ldirty -llua $(LD_FLAGS) -o $@ $^

$(LIB_TARGET): $(OBJ_FILES)
	@mkdir -p $(LIB_PATH)
	$(AR) $(AR_FLAGS) $(LIB_TARGET) $^

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	@mkdir -p $(OBJ_PATH)
	$(CC) $(C_FLAGS) -c $< -o $@

.PHONY: res
res:
	rm -rf src/res
	sh scripts/cres.sh res src/res

.PHONY: install
install: $(LIB_TARGET)
	cp $(LIB_TARGET) $(INSTALL_LIB)
	cp -r $(INC_PATH)/* $(INSTALL_INC)

.PHONY: install-lua
install-lua: $(BIN_TARGET)
	install $(BIN_TARGET) $(INSTALL_BIN)

.PHONY: clean
clean:
	rm -rf build

