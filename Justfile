# wengwengweng

run args="": build
	./dist/dirty {{args}}

build:
	cc \
		-I ext/sokol \
		-I ext/stb \
		-l SDL2 \
		-l lua \
		-framework OpenGL \
		-o dist/dirty \
		src/*.c src/d/*.c

