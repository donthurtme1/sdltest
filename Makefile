make:
	sth -o shaders.h vertex.glsl fragment.glsl
	gcc -o sdl -g sdl.c matrix.c rotor.c -lSDL2 -lGL -lglut -lopenblas -lm
install: make
	cp -f sdl /usr/local/bin/sdl
uninstall:
	rm -f /usr/local/bin/sdl
