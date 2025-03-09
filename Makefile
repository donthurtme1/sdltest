make:
	sth -o include/shaders.h vertex.glsl fragment.glsl
	gcc -o sdl -g src/sdl.c -Iinclude -lSDL2 -lGL -lglut -lopenblas -lm
install: make
	cp -f sdl /usr/local/bin/sdl
uninstall:
	rm -f /usr/local/bin/sdl
