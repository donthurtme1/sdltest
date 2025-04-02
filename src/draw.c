#ifndef DRAW_C
#define DRAW_C

#include "types.h"

void draw_asteroids(struct IbufferArrayList *asteroid_list) {
	for (int i = 0;
			i < length_of(asteroid_list->array) &&
			asteroid_list->array[i] > 0;
			i++) {
	}
}

#endif
