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
CC := cc

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
BIN_PATH := build/$(TARGET)
DEMO_PATH := demo

# flags
CFLAGS += -Wall
CFLAGS += -Werror
CFLAGS += -pedantic
CFLAGS += -std=c99
CFLAGS += -I.
CFLAGS += -Iext
CFLAGS += -Wno-unused-function
CFLAGS += -Wno-unused-variable

ifdef DEBUG
CFLAGS += -O0 -g
else
CFLAGS += -O2
endif

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

ifeq ($(TARGET),web)
CFLAGS += -s ALLOW_MEMORY_GROWTH=1
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

ifndef DEBUG
LDFLAGS += -flto
endif

# files
DEMO_FILES := $(wildcard $(DEMO_PATH)/*.c)
DEMOS := $(patsubst $(DEMO_PATH)/%.c, %, $(DEMO_FILES))
DEMO_TARGETS := $(patsubst $(DEMO_PATH)/%.c, $(BIN_PATH)/%, $(DEMO_FILES))

PREFIX := /usr/local

.PHONY: default
default: run

.PHONY: run
run: $(BIN_PATH)/$(DEMO)
ifeq ($(TARGET),web)
	cd $(BIN_PATH); \
		PORT=8000 fserv
else ifeq ($(TARGET),iossim)
	$(MAKE) bundle
	xcrun simctl boot $(SIMULATOR)
	xcrun simctl install $(SIMULATOR) $<.app
	open -a Simulator --args -CurrentDeviceUDID $(SIMULATOR)
	xcrun simctl launch --console $(SIMULATOR) xyz.space55.$(DEMO)
else ifeq ($(TARGET),ios)
	$(MAKE) bundle
# 	ios-deploy --debug --bundle $<.app
else
	./$< $(ARGS)
endif

.PHONY: debug
debug: $(BIN_PATH)/$(DEMO)
	lldb $<

.PHONY: demo
demo: $(BIN_PATH)/$(DEMO)

.PHONY: demos
demos: $(DEMO_TARGETS)

.PHONY: bundle
bundle: $(BIN_PATH)/$(DEMO)
ifeq ($(TARGET),macos)
	mkdir -p $<.app/Contents/MacOS
	mkdir -p $<.app/Contents/Resources
	cp $< $<.app/Contents/MacOS
	cp -R $(DEMO_PATH)/res $<.app/Contents/Resources
	sed 's/{{name}}/$(DEMO)/' misc/macos.plist > $<.app/Contents/Info.plist
else ifeq ($(TARGET),iossim)
	mkdir -p $<.app
	cp $< $<.app/
	cp -r $(BIN_PATH)/res $<.app/
	sed 's/{{name}}/$(DEMO)/' misc/ios.plist > $<.app/Info.plist
else ifeq ($(TARGET),ios)
	mkdir -p $<.app
	cp $< $<.app/
	cp -r $(BIN_PATH)/res $<.app/
	sed 's/{{name}}/$(DEMO)/' misc/ios.plist > $<.app/Info.plist
# 	cp $(PROFILE) $<.app/embedded.mobileprovision
# 	codesign -s "$(CODESIGN)" $<.app
endif

$(BIN_PATH)/%: $(DEMO_PATH)/%.c *.h
	@mkdir -p $(BIN_PATH)
ifeq ($(TARGET),web)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@.js $<
	sed 's/{{name}}/$*/' misc/web.html > $(BIN_PATH)/index.html
else
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<
endif
	rsync -a --delete $(DEMO_PATH)/res $(BIN_PATH)/

$(BIN_PATH)/dirty: dirty.c *.h
	@mkdir -p $(BIN_PATH)
	cc $(CFLAGS) $(LDFLAGS) $< -o $@

.PHONY: runscript
runscript: $(BIN_PATH)/dirty
	$< $(ARGS)

.PHONY: install
install: $(BIN_PATH)/dirty
	install $< /usr/local/bin/dirty

.PHONY: clean
clean:
	rm -rf build
