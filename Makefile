CFLAGS=-Wunused
LFLAGS=-Iinclude -lSDL2 -lGL -lglut -lopenblas -lm

make:
	gcc -o SUPER_AsteroidFucker_69 -g src/main.c $(LFLAGS) $(CFLAGS)
install: make
	cp -f SUPER_AsteroidFucker_69 /usr/local/bin/SUPER_AsteroidFucker_69
uninstall:
	rm -f /usr/local/bin/SUPER_AsteroidFucker_69
