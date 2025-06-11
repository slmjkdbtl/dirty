# finding host and default targets
ifeq ($(OS),Windows_NT)
HOST := windows
endif

ifeq ($(shell uname -s),Darwin)
HOST := macos
endif

ifeq ($(shell uname -s),Linux)
HOST := linux
endif

TARGET := $(HOST)

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

ifeq ($(TARGET),windows)
CC := x86_64-w64-mingw32-gcc
endif

DEMO := hi
BUILD_PATH := build/$(TARGET)
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
CFLAGS += -Wno-missing-braces
CFLAGS += -Wno-unused-but-set-variable
CFLAGS += -Wno-tautological-compare

ifdef DEBUG
CFLAGS += -O0 -g
else
CFLAGS += -O2
endif

ifeq ($(TARGET),macos)
CFLAGS += -ObjC
CFLAGS += -arch arm64
CFLAGS += -arch x86_64
endif

ifeq ($(TARGET),ios)
CFLAGS += -ObjC
CFLAGS += -arch armv7
CFLAGS += -arch arm64
endif

ifeq ($(TARGET),iossim)
CFLAGS += -ObjC
endif

ifeq ($(TARGET),web)
CFLAGS += -s "ALLOW_MEMORY_GROWTH=1"
CFLAGS += -s "EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap']"
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

ifeq ($(TARGET),windows)
LDFLAGS += -mwindows
LDFLAGS += -lole32
LDFLAGS += -lavrt
LDFLAGS += -lwinmm
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
DEMO_TARGETS := $(patsubst $(DEMO_PATH)/%.c, $(BUILD_PATH)/%, $(DEMO_FILES))
PREFIX := /usr/local

LUA_PATH := ext/lua
LUA_SRC := $(wildcard $(LUA_PATH)/src/*.c) ext/stb_vorbis.c d_lua.c
LUA_OBJ := $(patsubst %.c, $(BUILD_PATH)/obj/%.o, $(LUA_SRC))
LUA_TARGET := $(BUILD_PATH)/dlua
LUA_CFLAGS += -I$(LUA_PATH)/src
LUA_CFLAGS += -Wno-gnu-label-as-value
LUA_CFLAGS += -Wno-incompatible-pointer-types-discards-qualifiers

.PHONY: run
run: $(BUILD_PATH)/$(DEMO)
ifeq ($(TARGET),web)
	cd $(BUILD_PATH); \
		python3 -m http.server 8000
else ifeq ($(TARGET),iossim)
	@if [ -z "$(SIMULATOR)" ]; then \
		echo "SIMULATOR not set, run 'xcrun simctl list' and select a device"; \
		exit 1; \
	fi
	$(MAKE) bundle
	xcrun simctl boot $(SIMULATOR)
	xcrun simctl install booted $<.app
	open -a Simulator --args -CurrentDeviceUDID $(SIMULATOR)
	xcrun simctl launch --console-pty booted xyz.space55.$(DEMO)
else ifeq ($(TARGET),ios)
	$(MAKE) bundle
	ios-deploy --debug --bundle $<.app
else ifeq ($(TARGET),windows)
ifeq ($(HOST), windows)
	cd $(BUILD_PATH); \
		$(DEMO).exe $(ARGS)
else
	cd $(BUILD_PATH); \
		wine $(DEMO).exe $(ARGS)
endif
else
	cd $(BUILD_PATH); \
		./$(DEMO) $(ARGS)
endif

.PHONY: debug
debug: $(BUILD_PATH)/$(DEMO)
	lldb -- $< $(ARGS)

.PHONY: demo
demo: $(BUILD_PATH)/$(DEMO)

.PHONY: demos
demos: $(DEMO_TARGETS)

.PHONY: bundle
bundle: $(BUILD_PATH)/$(DEMO)
ifeq ($(TARGET),macos)
	mkdir -p $<.app/Contents/MacOS
	mkdir -p $<.app/Contents/Resources
	cp $< $<.app/Contents/MacOS
	cp -R $(DEMO_PATH)/res $<.app/Contents/Resources
	sed 's/{{name}}/$(DEMO)/' misc/macos.plist > $<.app/Contents/Info.plist
else ifeq ($(TARGET),iossim)
	mkdir -p $<.app
	cp $< $<.app/
	cp -r $(BUILD_PATH)/res $<.app/
	sed 's/{{name}}/$(DEMO)/' misc/ios.plist > $<.app/Info.plist
else ifeq ($(TARGET),ios)
	mkdir -p $<.app
	cp $< $<.app/
	cp -r $(BUILD_PATH)/res $<.app/
	sed 's/{{name}}/$(DEMO)/' misc/ios.plist > $<.app/Info.plist
	@if [ -z "$(PROVISION)" ] || [ -z "$(CODESIGN)" ]; then \
		echo "PROVISION and CODESIGN required"; \
		exit 1; \
	fi
	cp $(PROVISION) $<.app/embedded.mobileprovision
	codesign -s "$(CODESIGN)" --deep --force $<.app
endif

$(BUILD_PATH)/%: $(DEMO_PATH)/%.c *.h
	@mkdir -p $(BUILD_PATH)
ifeq ($(TARGET),web)
	$(CC) $(CFLAGS) -o $@.js $< ext/stb_vorbis.c $(LDFLAGS)
	sed 's/{{name}}/$*/' misc/web.html > $(BUILD_PATH)/index.html
else
	$(CC) $(CFLAGS) -o $@ $< ext/stb_vorbis.c $(LDFLAGS)
endif
	rsync -a --delete $(DEMO_PATH)/res $(BUILD_PATH)/

d_lua_lib.h: lib.lua
	xxd -i $< > $@

$(LUA_TARGET): $(LUA_SRC) d_lua_lib.h *.h
	@mkdir -p $(BUILD_PATH)
	$(CC) $(CFLAGS) $(LUA_CFLAGS) $(LDFLAGS) $(LUA_SRC) -o $@

.PHONY: runlua
runlua: $(LUA_TARGET)
	$< $(ARGS)

.PHONY: install
installlua: $(LUA_TARGET)
	install $< $(PREFIX)/bin/dlua

$(BUILD_PATH)/dirty: dirty.c *.h
	@mkdir -p $(BUILD_PATH)
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

.PHONY: runscript
runscript: $(BUILD_PATH)/dirty
	$< $(ARGS)

.PHONY: debugscript
debugscript: $(BUILD_PATH)/dirty
	lldb -- $< $(ARGS)

.PHONY: install
install: $(BUILD_PATH)/dirty
	install $< $(PREFIX)/bin/dirty

.PHONY: clean
clean:
	rm -rf build
