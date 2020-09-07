# wengwengweng

ifeq ($(OS),Windows_NT)
TARGET := Windows
endif

ifeq ($(shell uname -s),Darwin)
TARGET := MacOS
endif

ifeq ($(shell uname -s),Linux)
TARGET := Linux
endif

CC := clang
AR := ar

EXAMPLE := tri

SRC_PATH := src
RES_PATH := src/res
OBJ_PATH := build/obj
BIN_PATH := build/bin
LIB_PATH := build/lib
INC_PATH := include
EXAMPLE_PATH := examples
EXT_INC_PATH := ext/include
EXT_LIB_PATH := ext/lib
LIB_TARGET := $(LIB_PATH)/libdirty.a

C_FLAGS += -I $(INC_PATH)
C_FLAGS += -I $(EXT_INC_PATH)
C_FLAGS += -Wall
C_FLAGS += -Wpedantic
C_FLAGS += -std=c99

ifeq ($(MODE),release)
C_FLAGS += -O3
endif

ifeq ($(TARGET),MacOS)
C_FLAGS += -ObjC
endif

LD_FLAGS += -L $(EXT_LIB_PATH)
LD_FLAGS += -l iconv
LD_FLAGS += -l SDL2

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

AR_FLAGS += -rc

RES_FILES := $(wildcard $(RES_PATH)/*.png)
RES_H_FILES := $(patsubst $(RES_PATH)/%, $(RES_PATH)/%.h, $(RES_FILES))

SRC_FILES := $(wildcard $(SRC_PATH)/*.c)
OBJ_FILES := $(patsubst $(SRC_PATH)/%.c, $(OBJ_PATH)/%.o, $(SRC_FILES))

EXAMPLE_FILES := $(wildcard $(EXAMPLE_PATH)/*.c)
EXAMPLES := $(patsubst $(EXAMPLE_PATH)/%.c, %, $(EXAMPLE_FILES))
EXAMPLE_TARGETS := $(patsubst $(EXAMPLE_PATH)/%.c, $(BIN_PATH)/%, $(EXAMPLE_FILES))

SDL2_VERSION := 2.0.12

.PHONY: lib
lib: $(LIB_TARGET)

.PHONY: res
res: $(RES_H_FILES)

.PHONY: examples
examples: $(EXAMPLE_TARGETS)
	cp -r $(EXAMPLE_PATH)/res $(BIN_PATH)/

.PHONY: run
run: $(BIN_PATH)/$(EXAMPLE)
	cp -r $(EXAMPLE_PATH)/res $(BIN_PATH)/
	cd $(BIN_PATH); \
		./$(EXAMPLE) $(ARGS)

$(BIN_PATH)/%: $(EXAMPLE_PATH)/%.c $(LIB_TARGET)
	@mkdir -p $(BIN_PATH)
	$(CC) $(C_FLAGS) -L $(LIB_PATH) -ldirty $(LD_FLAGS) -o $@ $^

$(LIB_TARGET): $(OBJ_FILES)
	@mkdir -p $(LIB_PATH)
	$(AR) $(AR_FLAGS) $(LIB_TARGET) $^

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	@mkdir -p $(OBJ_PATH)
	$(CC) $(C_FLAGS) -c $< -o $@

$(RES_PATH)/%.h: $(RES_PATH)/%
	cd $(RES_PATH); \
		xxd -i $(notdir $^) > $(notdir $@)

.PHONY: clean
clean:
	rm -rf build

.PHONY: sdl2
sdl2:
	mkdir -p $(EXT_INC_PATH)/SDL2
	cd ext; \
		curl https://www.libsdl.org/release/SDL2-$(SDL2_VERSION).zip > SDL2-$(SDL2_VERSION).zip; \
		unzip -o SDL2-$(SDL2_VERSION).zip
	cd ext/SDL2-$(SDL2_VERSION); \
		cp include/*.h ../include/SDL2/; \
		./configure; \
		$(MAKE)
	cp -r ext/SDL2-$(SDL2_VERSION)/build/.libs/libSDL2.a $(EXT_LIB_PATH)/libSDL2.a
	rm -rf ext/SDL2-$(SDL2_VERSION)
	rm -rf ext/SDL2-$(SDL2_VERSION).zip

.PHONY: init
init:
	git submodule update --init --recursive

.PHONY: update
update:
	git submodule foreach git pull origin master

