PKGCONF = `pkg-config --cflags --libs sdl2 gl glut openblas`
FLAGS = -Wunused -Iinclude -lm

make:
	gcc -o SUPER_AsteroidFucker_69 -g src/main.c $(PKGCONF) $(FLAGS)
install: make
	cp -f SUPER_AsteroidFucker_69 /usr/local/bin/SUPER_AsteroidFucker_69
uninstall:
	rm -f /usr/local/bin/SUPER_AsteroidFucker_69
