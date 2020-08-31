# wengwengweng

CC := cc
AR := ar rcu
LIB_SRC := src/d
BIN_SRC := src
OBJ_PATH := build/obj
BIN_PATH := build/bin
LIB_PATH := build/lib
C_FLAGS += -ObjC
C_FLAGS += -D GL_SILENCE_DEPRECATION
LD_FLAGS += -l SDL2
LD_FLAGS += -l lua
LD_FLAGS += -framework OpenGL
LD_FLAGS += -framework CoreFoundation
LD_FLAGS += -framework Cocoa

LIB_TARGET := $(LIB_PATH)/dirty.a
BIN_TARGET := $(BIN_PATH)/dirty

LIB_SRC_FILES := $(wildcard $(LIB_SRC)/*.c)
LIB_OBJ_FILES := $(patsubst $(LIB_SRC)/%.c, $(OBJ_PATH)/%.o, $(LIB_SRC_FILES))
BIN_SRC_FILES := $(wildcard $(BIN_SRC)/*.c) $(LIB_SRC_FILES)
BIN_OBJ_FILES := $(patsubst $(BIN_SRC)/%.c, $(OBJ_PATH)/%.o, $(wildcard $(BIN_SRC)/*.c)) $(LIB_OBJ_FILES)

EXTLIBS := $(addprefix lib/,libSDL2.a liblua.a)

SDL2_VERSION := 2.0.12
LUA_VERSION := 5.4.0

.PHONY: bin
bin: $(BIN_TARGET)

.PHONY: lib
lib: $(LIB_TARGET)

$(BIN_TARGET): $(BIN_OBJ_FILES)
	$(CC) $(LD_FLAGS) $^ -o $@

$(LIB_TARGET): $(LIB_OBJ_FILES)
	$(AR) $(LIB_TARGET) $^

$(OBJ_PATH)/%.o: $(BIN_SRC)/%.c
	$(CC) $(C_FLAGS) -c $< -o $@

$(OBJ_PATH)/%.o: $(LIB_SRC)/%.c
	$(CC) $(C_FLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJ_PATH)/*
	rm -rf $(BIN_PATH)/*
	rm -rf $(LIB_PATH)/*

.PHONY: run
run: $(BIN_TARGET)
	./$(BIN_TARGET) $(EXAMPLE)

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
	rm -rf lib/SDL2-$(SDL2_VERSION)
	rm -rf lib/SDL2-$(SDL2_VERSION).zip

