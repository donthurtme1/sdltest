#ifndef INPUT_C
#define INPUT_C

#include <openblas/cblas.h>
#include <SDL2/SDL.h>
#include "rotor.c"
#include "types.h"

/* Global state */
extern int g_run;
extern struct Cube g_cube;
extern struct Camera g_camera;
extern struct InputSet g_inputstate;

void handle_keydown(SDL_Event *event) {
	float rotor[4] = { };
	if (event->key.repeat == 0) {
		switch (event->key.keysym.sym) {
			case SDLK_q:
				rotor[0] = 1.0f;
				rotor[1] += 1.0f;
				normalise_rotor_bivec(rotor, 0.02f);
				combine_rotor(rotor, g_camera.rotor_df, g_camera.rotor_df);
				break;
			case SDLK_e:
				rotor[0] = 1.0f;
				rotor[1] -= 1.0f;
				normalise_rotor_bivec(rotor, 0.02f);
				combine_rotor(rotor, g_camera.rotor_df, g_camera.rotor_df);
				break;
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
				g_inputstate.forward = 1;
				break;
			case SDLK_s:
				g_inputstate.forward = -1;
				break;
			case SDLK_d:
				g_inputstate.right = 1;
				break;
			case SDLK_a:
				g_inputstate.right = -1;
				break;
			case SDLK_SPACE:
				g_inputstate.up = 1;
				break;
			case SDLK_LSHIFT:
				g_inputstate.up = -1;
				break;
		}
	}
}

void handle_keyup(SDL_Event *event) {
	static float rotor[4];
	if (event->key.repeat == 0) {
		switch (event->key.keysym.sym) {
			case SDLK_q:
				g_camera.rotor_df[1] = 0.0f;
				normalise_rotor_bivec(g_camera.rotor_df, 0.02f);
				break;
			case SDLK_e:
				g_camera.rotor_df[1] = 0.0f;
				normalise_rotor_bivec(g_camera.rotor_df, 0.02f);
				break;
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
				if (g_inputstate.forward == 1)
					g_inputstate.forward = 0;
				break;
			case SDLK_s:
				if (g_inputstate.forward == -1)
					g_inputstate.forward = 0;
				break;
			case SDLK_d:
				if (g_inputstate.right == 1)
					g_inputstate.right = 0;
				break;
			case SDLK_a:
				if (g_inputstate.right == -1)
					g_inputstate.right = 0;
				break;
			case SDLK_SPACE:
				if (g_inputstate.up == 1)
					g_inputstate.up = 0;
				break;
			case SDLK_LSHIFT:
				if (g_inputstate.up == -1)
					g_inputstate.up = 0;
				break;
		}
	}
}

void handle_mouse(SDL_Event *event) {
	extern float g_sens;
	float rotor[4] = { };
	rotor[0] = 1.0f;
	rotor[2] += g_sens * event->motion.yrel;
	rotor[3] += g_sens * event->motion.xrel;
	combine_rotor(rotor, g_camera.rotor, g_camera.rotor);
}

#endif
