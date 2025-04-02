#ifndef INPUT_C
#define INPUT_C

#include <openblas/cblas.h>
#include <SDL2/SDL.h>
#include "rotor.c"
#include "types.h"

/* Global state */

/* Change the InputSet pointed to by `inputstate` according to
 * which keys have been pressed. */
void handle_keydown(SDL_Event *event, struct InputSet *inputstate) {
	extern struct Cube g_cube;
	extern struct Camera g_camera;

	if (event->key.repeat == 0) {
		switch (event->key.keysym.sym) {
			case SDLK_UP:
				g_cube.rotor_delta[2] += 0.05f;
				normalise_rotor_bivec(g_cube.rotor_delta, 0.05f);
				break;
			case SDLK_DOWN:
				g_cube.rotor_delta[2] -= 0.05f;
				normalise_rotor_bivec(g_cube.rotor_delta, 0.05f);
				break;
			case SDLK_LEFT:
				g_cube.rotor_delta[3] += 0.05f;
				normalise_rotor_bivec(g_cube.rotor_delta, 0.05f);
				break;
			case SDLK_RIGHT:
				g_cube.rotor_delta[3] -= 0.05f;
				normalise_rotor_bivec(g_cube.rotor_delta, 0.05f);
				break;
			case SDLK_w:
				inputstate->forward = 1;
				break;
			case SDLK_s:
				inputstate->forward = -1;
				break;
			case SDLK_d:
				inputstate->right = 1;
				break;
			case SDLK_a:
				inputstate->right = -1;
				break;
			case SDLK_SPACE:
				inputstate->up = 1;
				break;
			case SDLK_LSHIFT:
				inputstate->up = -1;
				break;
		}
	}
}

void handle_keyup(SDL_Event *event, struct InputSet *inputstate) {
	extern struct Cube g_cube;

	if (event->key.repeat == 0) {
		switch (event->key.keysym.sym) {
			case SDLK_UP:
				g_cube.rotor_delta[2] = 0.0f;
				normalise_rotor_bivec(g_cube.rotor_delta, 0.05f);
				break;
			case SDLK_DOWN:
				g_cube.rotor_delta[2] = 0.0f;
				normalise_rotor_bivec(g_cube.rotor_delta, 0.05f);
				break;
			case SDLK_LEFT:
				g_cube.rotor_delta[3] = 0.0f;
				normalise_rotor_bivec(g_cube.rotor_delta, 0.05f);
				break;
			case SDLK_RIGHT:
				g_cube.rotor_delta[3] = 0.0f;
				normalise_rotor_bivec(g_cube.rotor_delta, 0.05f);
				break;
			case SDLK_w:
				if (inputstate->forward > 0)
					inputstate->forward = 0;
				break;
			case SDLK_s:
				if (inputstate->forward < 0)
					inputstate->forward = 0;
				break;
			case SDLK_d:
				if (inputstate->right > 0)
					inputstate->right = 0;
				break;
			case SDLK_a:
				if (inputstate->right < 0)
					inputstate->right = 0;
				break;
			case SDLK_SPACE:
				if (inputstate->up > 0)
					inputstate->up = 0;
				break;                
			case SDLK_LSHIFT:         
				if (inputstate->up < 0)
					inputstate->up = 0;
				break;
		}
	}
}

void handle_mouse(SDL_Event *event) {
	extern float g_sens;
	extern struct Camera g_camera;

	float rotor[4] = { };
	rotor[0] = 1.0f;
	rotor[2] += g_sens * event->motion.yrel;
	rotor[3] += g_sens * event->motion.xrel;
	combine_rotor(rotor, g_camera.rotor, g_camera.rotor);
}

#endif
