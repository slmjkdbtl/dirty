# wengwengweng

run args="": build
	./dist/dirty {{args}}

build:
	cc \
		-I ext/sokol \
		-I ext/stb \
		-l SDL2 \
		-l lua \
		-ObjC \
		-D GL_SILENCE_DEPRECATION \
		-framework OpenGL \
		-framework CoreFoundation \
		-framework Cocoa \
		-o dist/dirty \
		src/*.c src/d/*.c

