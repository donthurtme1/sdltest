make:
	sth -o shaders.h vertex.glsl fragment.glsl
	gcc -o sdltest -g sdltest.c matrix.c -lSDL2 -lGL -lglut -lopenblas -lm
install: make
	cp -f sdltest /usr/local/bin/sdltest
uninstall:
	rm -f /usr/local/bin/sdltest
