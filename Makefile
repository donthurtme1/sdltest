CFLAGS=-Wunused
LFLAGS=-Iinclude -lSDL2 -lGL -lglut -lopenblas -lm

make:
	sth -o include/shaders.h shader/pf_vertex.glsl shader/pf_fragment.glsl
	gcc -o SUPER_AsteroidFucker_69 -g src/main.c $(LFLAGS) $(CFLAGS)
install: make
	cp -f SUPER_AsteroidFucker_69 /usr/local/bin/SUPER_AsteroidFucker_69
uninstall:
	rm -f /usr/local/bin/SUPER_AsteroidFucker_69
