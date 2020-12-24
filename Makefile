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

DEMO := hi
DEMO_BIN_PATH := build/$(TARGET)
DEMO_PATH := demo

# flags
CFLAGS += -Wall
CFLAGS += -Wpedantic
CFLAGS += -std=c99
CFLAGS += -I.

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
LDFLAGS += -framework UIKit
LDFLAGS += -framework CoreGraphics
LDFLAGS += -framework OpenGLES
LDFLAGS += -framework GLKit
LDFLAGS += -framework Metal
LDFLAGS += -framework MetalKit
LDFLAGS += -framework AudioToolbox
endif

ifeq ($(TARGET),iossim)
LDFLAGS += -framework Foundation
LDFLAGS += -framework UIKit
LDFLAGS += -framework CoreGraphics
LDFLAGS += -framework OpenGLES
LDFLAGS += -framework GLKit
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
ifeq ($(TARGET),web)
	cd $(DEMO_BIN_PATH); \
		PORT=8000 fserv
else ifeq ($(TARGET),iossim)
	mkdir -p $<.app
	cp $< $<.app/
	sed 's/{{name}}/$(DEMO)/' misc/ios.plist > $<.app/Info.plist
	xcrun simctl boot $(SIMULATOR) | true
	xcrun simctl install $(SIMULATOR) $<.app
	open -a Simulator --args -CurrentDeviceUDID $(SIMULATOR)
	xcrun simctl launch --console $(SIMULATOR) xyz.space55.$(DEMO)
else ifeq ($(TARGET),ios)
	mkdir -p $<.app
	cp $< $<.app/
	sed 's/{{name}}/$(DEMO)/' misc/ios.plist > $<.app/Info.plist
	cp $(PROFILE) $<.app/embedded.mobileprovision
	codesign -s "$(CODESIGN)" $<.app
	# does not work
	ios-deploy --debug --bundle $<.app
else
	rsync -a --delete $(DEMO_PATH)/res $(DEMO_BIN_PATH)/
	./$< $(ARGS)
endif

.PHONY: demo
demo: $(DEMO_BIN_PATH)/$(DEMO)

.PHONY: demos
demos: $(DEMO_TARGETS)
	rsync -a --delete $(DEMO_PATH)/res $(DEMO_BIN_PATH)/

$(DEMO_BIN_PATH)/%: $(DEMO_PATH)/%.c dirty.h
	@mkdir -p $(DEMO_BIN_PATH)
ifeq ($(TARGET),web)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@.js $<
	sed 's/{{name}}/$*/' misc/web.html > $(DEMO_BIN_PATH)/index.html
else
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<
endif

.PHONY: install
install:
	install dirty.h $(PREFIX)/include/

.PHONY: clean
clean:
	rm -rf build

