#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>

/* Input */
extern void handle_events(void);
extern void handle_keydown(SDL_Event *event);
extern void handle_keyup(SDL_Event *event);
extern void handle_mouse(SDL_Event *event);

#endif
