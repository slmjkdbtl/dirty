# wengwengweng

ifeq ($(OS),Windows_NT)
PLATFORM := Windows
endif

ifeq ($(shell uname -s),Darwin)
PLATFORM := MacOS
endif

ifeq ($(shell uname -s),Linux)
PLATFORM := Linux
endif

TARGET := $(PLATFORM)

CC := cc
AR := ar

ifeq ($(TARGET),Web)
CC := emcc
AR := emar
endif

ifeq ($(PLATFORM),MacOS)
ifeq ($(TARGET),Windows)
CC := x86_64-w64-mingw32-gcc
AR := x86_64-w64-mingw32-ar
endif
ifeq ($(TARGET),Linux)
CC := x86_64-unknown-linux-gnu-gcc
AR := x86_64-unknown-linux-gnu-ar
endif
endif

SRC_PATH := src
OBJ_PATH := build/obj
BIN_PATH := build/bin
LIB_PATH := build/lib

C_FLAGS += -I lib/include

ifeq ($(TARGET),MacOS)
C_FLAGS += -ObjC
C_FLAGS += -D GL_SILENCE_DEPRECATION
endif

LD_FLAGS += -L lib
LD_FLAGS += -l iconv
LD_FLAGS += -l SDL2
LD_FLAGS += -l lua
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

ifeq ($(TARGET),Web)
LD_FLAGS += USE_SDL=2
endif

AR_FLAGS += -rc

LIB_TARGET := $(LIB_PATH)/dirty.a
BIN_TARGET := $(BIN_PATH)/dirty

SRC_FILES := $(wildcard $(SRC_PATH)/*.c)
OBJ_FILES := $(patsubst $(SRC_PATH)/%.c, $(OBJ_PATH)/%.o, $(SRC_FILES))

#  EXTLIBS := $(addprefix lib/,libSDL2.a liblua.a)

SDL2_VERSION := 2.0.12
LUA_VERSION := 5.4.0

.PHONY: bin
bin: $(BIN_TARGET)

.PHONY: lib
lib: $(LIB_TARGET)

$(BIN_TARGET): $(OBJ_FILES)
	$(CC) $(LD_FLAGS) $^ -o $@

$(LIB_TARGET): $(OBJ_FILES)
	$(AR) $(AR_FLAGS) $(LIB_TARGET) $^

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	$(CC) $(C_FLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJ_PATH)/*
	rm -rf $(BIN_PATH)/*
	rm -rf $(LIB_PATH)/*

.PHONY: run
run: $(BIN_TARGET)
	./$(BIN_TARGET) $(ARGS)

.PHONY: lua
lua:
	cd lib; \
		curl http://www.lua.org/ftp/lua-$(LUA_VERSION).tar.gz > lua-$(LUA_VERSION).tar.gz; \
		tar zxf lua-$(LUA_VERSION).tar.gz
	cd lib/lua-$(LUA_VERSION); \
		cp src/*.h ../include/lua/; \
		$(MAKE)
	cp -r lib/lua-$(LUA_VERSION)/src/liblua.a lib/liblua.a
	rm -rf lib/lua-$(LUA_VERSION)
	rm -rf lib/lua-$(LUA_VERSION).tar.gz

.PHONY: sdl2
sdl2:
	cd lib; \
		curl https://www.libsdl.org/release/SDL2-$(SDL2_VERSION).zip > SDL2-$(SDL2_VERSION).zip; \
		unzip -o SDL2-$(SDL2_VERSION).zip
	cd lib/SDL2-$(SDL2_VERSION); \
		cp include/*.h ../include/SDL2/; \
		./configure; \
		$(MAKE)
	cp -r lib/SDL2-$(SDL2_VERSION)/build/.libs/libSDL2.a lib/libSDL2.a
	rm -rf lib/SDL2-$(SDL2_VERSION)
	rm -rf lib/SDL2-$(SDL2_VERSION).zip

