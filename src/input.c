#ifndef INPUT_C
#define INPUT_C

#include <openblas/cblas.h>
#include <SDL2/SDL.h>
#include "rotor.c"
#include "types.h"

/* Global state */
extern int run;
extern struct Cube cube;
extern struct Camera camera;
extern struct InputSet input_state;

void handle_keydown(SDL_Event *event) {
	float rotor[4] = { };
	if (event->key.repeat == 0) {
		switch (event->key.keysym.sym) {
			case SDLK_q:
				rotor[0] = 1.0f;
				rotor[1] += 1.0f;
				normalise_rotor_bivec(rotor, 0.02f);
				combine_rotor(rotor, camera.rotor_df, camera.rotor_df);
				break;
			case SDLK_e:
				rotor[0] = 1.0f;
				rotor[1] -= 1.0f;
				normalise_rotor_bivec(rotor, 0.02f);
				combine_rotor(rotor, camera.rotor_df, camera.rotor_df);
				break;
			case SDLK_UP:
				cube.rotor_delta[2] += 0.05f;
				normalise_rotor_bivec(cube.rotor_delta, 0.05f);
				break;
			case SDLK_DOWN:
				cube.rotor_delta[2] -= 0.05f;
				normalise_rotor_bivec(cube.rotor_delta, 0.05f);
				break;
			case SDLK_LEFT:
				cube.rotor_delta[3] += 0.05f;
				normalise_rotor_bivec(cube.rotor_delta, 0.05f);
				break;
			case SDLK_RIGHT:
				cube.rotor_delta[3] -= 0.05f;
				normalise_rotor_bivec(cube.rotor_delta, 0.05f);
				break;
			case SDLK_w:
				input_state.forward = 1;
				break;
			case SDLK_s:
				input_state.forward = -1;
				break;
			case SDLK_d:
				input_state.right = 1;
				break;
			case SDLK_a:
				input_state.right = -1;
				break;
			case SDLK_SPACE:
				input_state.up = 1;
				break;
			case SDLK_LSHIFT:
				input_state.up = -1;
				break;
		}
	}
}

void handle_keyup(SDL_Event *event) {
	static float rotor[4];
	if (event->key.repeat == 0) {
		switch (event->key.keysym.sym) {
			case SDLK_q:
				camera.rotor_df[1] = 0.0f;
				normalise_rotor_bivec(camera.rotor_df, 0.02f);
				break;
			case SDLK_e:
				camera.rotor_df[1] = 0.0f;
				normalise_rotor_bivec(camera.rotor_df, 0.02f);
				break;
			case SDLK_UP:
				cube.rotor_delta[2] = 0.0f;
				normalise_rotor_bivec(cube.rotor_delta, 0.05f);
				break;
			case SDLK_DOWN:
				cube.rotor_delta[2] = 0.0f;
				normalise_rotor_bivec(cube.rotor_delta, 0.05f);
				break;
			case SDLK_LEFT:
				cube.rotor_delta[3] = 0.0f;
				normalise_rotor_bivec(cube.rotor_delta, 0.05f);
				break;
			case SDLK_RIGHT:
				cube.rotor_delta[3] = 0.0f;
				normalise_rotor_bivec(cube.rotor_delta, 0.05f);
				break;
			case SDLK_w:
				if (input_state.forward == 1)
					input_state.forward = 0;
				break;
			case SDLK_s:
				if (input_state.forward == -1)
					input_state.forward = 0;
				break;
			case SDLK_d:
				if (input_state.right == 1)
					input_state.right = 0;
				break;
			case SDLK_a:
				if (input_state.right == -1)
					input_state.right = 0;
				break;
			case SDLK_SPACE:
				if (input_state.up == 1)
					input_state.up = 0;
				break;
			case SDLK_LSHIFT:
				if (input_state.up == -1)
					input_state.up = 0;
				break;
		}
	}
}

void handle_mouse(SDL_Event *event) {
	extern float sens;
	float rotor[4] = { };
	rotor[0] = 1.0f;
	rotor[2] += sens * event->motion.yrel;
	rotor[3] += sens * event->motion.xrel;
	combine_rotor(rotor, camera.rotor, camera.rotor);
}

#endif
