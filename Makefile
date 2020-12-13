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
RANLIB := ranlib

ifeq ($(TARGET),web)
CC := emcc
AR := emar
RANLIB := emranlib
endif

ifeq ($(TARGET),ios)
CC := xcrun -sdk iphoneos clang
AR := xcrun -sdk iphoneos ar
RANLIB := xcrun -sdk iphoneos ranlib
endif

ifeq ($(TARGET),iossim)
CC := xcrun -sdk iphonesimulator clang
AR := xcrun -sdk iphonesimulator ar
RANLIB := xcrun -sdk iphonesimulator ranlib
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
EXT_LIB_PATH := ext/lib/$(TARGET)
LIB_TARGET := $(LIB_PATH)/libdirty.a
BIN_TARGET := $(BIN_PATH)/dirty

# flags
CFLAGS += -I $(INC_PATH)
CFLAGS += -I $(EXT_INC_PATH)
CFLAGS += -Wall
CFLAGS += -Wpedantic
CFLAGS += -std=c99

ifeq ($(TARGET),macos)
CFLAGS += -ObjC
endif

ifeq ($(TARGET),ios)
CFLAGS += -ObjC
CFLAGS += -arch armv7
CFLAGS += -arch arm64
endif

ifeq ($(TARGET),iossim)
CFLAGS += -ObjC
CFLAGS += -arch x86_64
endif

ifdef RELEASE
CFLAGS += -O3
endif

LDFLAGS += -L $(EXT_LIB_PATH)

ifeq ($(TARGET),macos)
LDFLAGS += -framework Cocoa
LDFLAGS += -framework OpenGL
LDFLAGS += -framework Metal
LDFLAGS += -framework MetalKit
LDFLAGS += -framework AudioToolbox
endif

ifeq ($(TARGET),ios)
LDFLAGS += -framework Foundation
LDFLAGS += -framework CoreGraphics
LDFLAGS += -framework UIKit
LDFLAGS += -framework Metal
LDFLAGS += -framework MetalKit
LDFLAGS += -framework AudioToolbox
endif

ifeq ($(TARGET),iossim)
LDFLAGS += -framework Foundation
LDFLAGS += -framework CoreGraphics
LDFLAGS += -framework UIKit
LDFLAGS += -framework Metal
LDFLAGS += -framework MetalKit
LDFLAGS += -framework AudioToolbox
endif

ifeq ($(TARGET),linux)
LDFLAGS += -lx11
LDFLAGS += -lGL
endif

ifeq ($(TARGET),web)
LDFLAGS += --preload-file demo/res@res
endif

ARFLAGS := rc

# files
SRC_FILES := $(wildcard $(SRC_PATH)/*.c)
OBJ_FILES := $(patsubst $(SRC_PATH)/%.c, $(OBJ_PATH)/%.o, $(SRC_FILES))
LUA_SRC_FILES := $(wildcard $(LUA_SRC_PATH)/*.c)

DEMO_FILES := $(wildcard $(DEMO_PATH)/*.c)
DEMOS := $(patsubst $(DEMO_PATH)/%.c, %, $(DEMO_FILES))
DEMO_TARGETS := $(patsubst $(DEMO_PATH)/%.c, $(DEMO_BIN_PATH)/%, $(DEMO_FILES))

PREFIX := /usr/local

.PHONY: lib
lib: $(LIB_TARGET)

.PHONY: lua
lua: $(BIN_TARGET)

.PHONY: run
run: $(DEMO_BIN_PATH)/$(DEMO)
	rsync -a --delete $(DEMO_PATH)/res $(DEMO_BIN_PATH)/
	./$(DEMO_BIN_PATH)/$(DEMO) $(ARGS)

.PHONY: run-lua
run-lua: $(BIN_TARGET) $(DEMO_PATH)/$(DEMO).lua
	./$(BIN_TARGET) $(DEMO_PATH)/$(DEMO).lua

.PHONY: demo
demo: $(DEMO_BIN_PATH)/$(DEMO)

.PHONY: demos
demos: $(DEMO_TARGETS)
	rsync -a --delete $(DEMO_PATH)/res $(DEMO_BIN_PATH)/

# TODO: $^ also includes $(LIB_TARGET)
$(DEMO_BIN_PATH)/%: $(DEMO_PATH)/%.c $(LIB_TARGET)
	@mkdir -p $(DEMO_BIN_PATH)
	$(CC) $(CFLAGS) -L $(LIB_PATH) -ldirty $(LDFLAGS) -o $@ $^

$(BIN_TARGET): $(LUA_SRC_FILES) $(LIB_TARGET)
	@mkdir -p $(BIN_PATH)
	$(CC) $(CFLAGS) -L $(LIB_PATH) -ldirty -llua $(LDFLAGS) -o $@ $^

$(LIB_TARGET): $(OBJ_FILES)
	@mkdir -p $(LIB_PATH)
	$(AR) $(ARFLAGS) $(LIB_TARGET) $^
	$(RANLIB) $(LIB_TARGET)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	@mkdir -p $(OBJ_PATH)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: res
res:
	rm -rf src/res
	sh scripts/cres.sh res src/res
	sokol-shdc --input res/quad.glsl --output src/res/quad.h --slang glsl330:glsl300es:hlsl5:metal_macos:metal_ios:metal_sim

.PHONY: install
install: $(LIB_TARGET)
	cp $(LIB_TARGET) $(PREFIX)/lib
	cp -r $(INC_PATH)/* $(PREFIX)/include

.PHONY: install-lua
install-lua: $(BIN_TARGET)
	install $(BIN_TARGET) $(PREFIX)/bin

.PHONY: clean
clean:
	rm -rf build

