# wengwengweng

ifeq ($(OS),Windows_NT)
HOST := Windows
endif

ifeq ($(shell uname -s),Darwin)
HOST := MacOS
endif

ifeq ($(shell uname -s),Linux)
HOST := Linux
endif

TARGET := $(HOST)

CC := cc
AR := ar

ifeq ($(TARGET),Web)
CC := emcc
AR := emar
endif

ifeq ($(HOST),MacOS)
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
RES_PATH := src/res
OBJ_PATH := build/obj
BIN_PATH := build/bin
LIB_PATH := build/lib
EXT_INC_PATH := ext/inc
EXT_LIB_PATH := ext/libs
LIB_TARGET := build/lib/dirty.a
BIN_TARGET := build/bin/dirty

C_FLAGS += -I $(EXT_INC_PATH)

ifeq ($(TARGET),MacOS)
C_FLAGS += -ObjC
C_FLAGS += -D GL_SILENCE_DEPRECATION
endif

LD_FLAGS += -L $(EXT_LIB_PATH)
LD_FLAGS += -l iconv
LD_FLAGS += -l SDL2
LD_FLAGS += -l lua

ifeq ($(TARGET),MacOS)
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

ifeq ($(TARGET),Web)
LD_FLAGS += USE_SDL=2
endif

AR_FLAGS += -rc

RES_FILES := $(wildcard $(RES_PATH)/*.png)
RES_H_FILES := $(patsubst $(RES_PATH)/%, $(RES_PATH)/%.h, $(RES_FILES))
SRC_FILES := $(wildcard $(SRC_PATH)/*.c)
OBJ_FILES := $(patsubst $(SRC_PATH)/%.c, $(OBJ_PATH)/%.o, $(SRC_FILES))

#  EXTLIBS := $(addprefix ext/lib/,libSDL2.a liblua.a)

SDL2_VERSION := 2.0.12
LUA_VERSION := 5.4.0

.PHONY: bin
bin: $(BIN_TARGET)

.PHONY: lib
lib: $(LIB_TARGET)

.PHONY: res
res: $(RES_H_FILES)

$(BIN_TARGET): $(OBJ_FILES)
	$(CC) $(LD_FLAGS) $^ -o $@

$(LIB_TARGET): $(OBJ_FILES)
	$(AR) $(AR_FLAGS) $(LIB_TARGET) $^

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	$(CC) $(C_FLAGS) -c $< -o $@

$(RES_PATH)/%.h: $(RES_PATH)/%
	cd $(RES_PATH); \
		xxd -i $(notdir $^) > $(notdir $@)

.PHONY: clean
clean:
	rm -rf $(OBJ_PATH)/*
	rm -rf $(BIN_PATH)/*
	rm -rf $(LIB_PATH)/*
	rm -rf $(RES_PATH)/*.h

.PHONY: run
run: $(BIN_TARGET)
	./$(BIN_TARGET) $(ARGS)

.PHONY: lua
lua:
	mkdir -p $(EXT_INC_PATH)/lua
	cd ext; \
		curl http://www.lua.org/ftp/lua-$(LUA_VERSION).tar.gz > lua-$(LUA_VERSION).tar.gz; \
		tar zxf lua-$(LUA_VERSION).tar.gz
	cd ext/lua-$(LUA_VERSION); \
		cp src/*.h ../inc/lua/; \
		$(MAKE)
	cp -r ext/lua-$(LUA_VERSION)/src/liblua.a $(EXT_LIB_PATH)/liblua.a
	rm -rf ext/lua-$(LUA_VERSION)
	rm -rf ext/lua-$(LUA_VERSION).tar.gz

.PHONY: sdl2
sdl2:
	mkdir -p $(EXT_INC_PATH)/SDL2
	cd ext; \
		curl https://www.libsdl.org/release/SDL2-$(SDL2_VERSION).zip > SDL2-$(SDL2_VERSION).zip; \
		unzip -o SDL2-$(SDL2_VERSION).zip
	cd ext/SDL2-$(SDL2_VERSION); \
		cp include/*.h ../inc/SDL2/; \
		./configure; \
		$(MAKE)
	cp -r ext/SDL2-$(SDL2_VERSION)/build/.libs/libSDL2.a $(EXT_LIB_PATH)/libSDL2.a
	rm -rf ext/SDL2-$(SDL2_VERSION)
	rm -rf ext/SDL2-$(SDL2_VERSION).zip

.PHONY: update
update:
	git submodule foreach git pull origin master

