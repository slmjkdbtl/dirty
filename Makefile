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

ifeq ($(TARGET),web)
CC := emcc
endif

ifeq ($(TARGET),ios)
CC := xcrun -sdk iphoneos clang
endif

ifeq ($(TARGET),iossim)
CC := xcrun -sdk iphonesimulator clang
endif

DEMO := test
DEMO_BIN_PATH := build/bin/$(TARGET)
DEMO_PATH := demo

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

# files
DEMO_FILES := $(wildcard $(DEMO_PATH)/*.c)
DEMOS := $(patsubst $(DEMO_PATH)/%.c, %, $(DEMO_FILES))
DEMO_TARGETS := $(patsubst $(DEMO_PATH)/%.c, $(DEMO_BIN_PATH)/%, $(DEMO_FILES))

PREFIX := /usr/local

.PHONY: run
run: $(DEMO_BIN_PATH)/$(DEMO)
	rsync -a --delete $(DEMO_PATH)/res $(DEMO_BIN_PATH)/
	./$(DEMO_BIN_PATH)/$(DEMO) $(ARGS)

.PHONY: demo
demo: $(DEMO_BIN_PATH)/$(DEMO)

.PHONY: demos
demos: $(DEMO_TARGETS)
	rsync -a --delete $(DEMO_PATH)/res $(DEMO_BIN_PATH)/

$(DEMO_BIN_PATH)/%: $(DEMO_PATH)/%.c
	@mkdir -p $(DEMO_BIN_PATH)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -rf build

