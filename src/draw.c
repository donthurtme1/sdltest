#ifndef DRAW_C
#define DRAW_C

#include "types.h"

void draw_asteroids(struct IbufferArrayList *asteroid_list) {
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, ubuf_redmaterial);

	for (int i = 0;
			i < length_of(asteroid_list->array) && asteroid_list->array[i] > 0;
			i++) {
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, asteroi_list->array[i]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void *)0);
	}
}

#endif
