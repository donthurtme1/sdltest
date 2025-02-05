#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>

/* Input */
extern void handle_events(void);
extern void handle_keydown(SDL_Event *event);
extern void handle_keyup(SDL_Event *event);
extern void handle_mouse(SDL_Event *event);

enum InputAction {
	FORWARD = 1,
	BACK = 2,
	LEFT = 4,
	RIGHT = 8,
	ROLL_LEFT = 16,
	ROLL_RIGHT = 32,
};

#endif
