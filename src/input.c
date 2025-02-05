#include <openblas/cblas.h>
#include <SDL2/SDL.h>
#include "rotor.h"
#include "input.h"

/* Global state */
extern int run;
extern struct Cube {
	float rotor[4], rotor_delta[4];
} cube;
extern struct Camera {
	float pos[3], pos_df[3];
	float rotor[4], rotor_df[4];
} camera;
extern enum InputAction input_state;

void
handle_keydown(SDL_Event *event) {
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
				cube.rotor_delta[2] -= 0.05f;
				normalise_rotor_bivec(cube.rotor_delta, 0.05f);
				break;
			case SDLK_DOWN:
				cube.rotor_delta[2] += 0.05f;
				normalise_rotor_bivec(cube.rotor_delta, 0.05f);
				break;
			case SDLK_LEFT:
				cube.rotor_delta[3] -= 0.05f;
				normalise_rotor_bivec(cube.rotor_delta, 0.05f);
				break;
			case SDLK_RIGHT:
				cube.rotor_delta[3] += 0.05f;
				normalise_rotor_bivec(cube.rotor_delta, 0.05f);
				break;
			case SDLK_w:
				input_state |= FORWARD;
				break;
			case SDLK_s:
				input_state |= BACK;
				break;
			case SDLK_d:
				input_state |= LEFT;
				break;
			case SDLK_a:
				input_state |= RIGHT;
				break;
		}
	}
}

void
handle_keyup(SDL_Event *event) {
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
				input_state &= ~FORWARD;
				break;
			case SDLK_s:
				input_state &= ~BACK;
				break;
			case SDLK_d:
				input_state &= ~RIGHT;
				break;
			case SDLK_a:
				input_state &= ~LEFT;
				break;
		}
	}
}

void
handle_mouse(SDL_Event *event) {
	float rotor[4] = { };
	rotor[0] = 1.0f;
	rotor[2] += 0.0002f * event->motion.yrel;
	rotor[3] += 0.0002f * event->motion.xrel;
	combine_rotor(rotor, camera.rotor, camera.rotor);
}
