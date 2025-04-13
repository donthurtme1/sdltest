#include <openblas/cblas.h>
#include <time.h>
#include <string.h>
#include "shaders.h"
#include "types.h"
#include "geometry.h"

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <SDL2/SDL.h>

#define IDENTITY_MATRIX { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }
#define die(str) { perror(str); exit(1); }

/* GL variables */
static struct {
	GLuint vshader, fshader, shader_program, vertex_arrayobj, vertex_bufobj, index_bufobj;
	float transform[3][16];
	float view[16], project[16], viewproject[16];
	GLuint ubuf_transform[3], ubuf_viewproject;
	GLuint ubuf_camera;
} gl;

static struct IbufferArrayList ibuf_array_list;

/* Global state */
struct GlobalState {
	int targetfps;
	float sens; /* Mouse sensitivity */

	struct InputSet inputstate;
	struct Cube cube;
	struct Camera camera;
	struct CameraData cameradata;
} global;

/* Game state */
struct GameState {
	struct Player player;
	struct Asteroid *asteroids; /* Loaded asteroids */
} game_state;

/* Function definitions */
#include "rotor.c"
#include "matrix.c"
#include "input.c"

static void pollevents(SDL_Event *event);
static void framestep(void);

/* This function is a possible cancelation point */
void pollevents(SDL_Event *event) {
	while (SDL_PollEvent(event)) {
		switch (event->type) {
			case SDL_KEYDOWN:
				handle_keydown(event, &global.inputstate);
				break;
			case SDL_KEYUP:
				handle_keyup(event, &global.inputstate);
				break;
			case SDL_MOUSEMOTION:
				handle_mouse(event);
				break;
			case SDL_QUIT:
				exit(0); /* Force quit game */
		}
	}
}

void framestep(void) {
	/* Cube */
	combine_rotor(global.cube.rotor, global.cube.rotor_delta, global.cube.rotor);
	for (int i = 0; i < 3; i++) {
		rotor_to_matrix(gl.transform[i], global.cube.rotor);
		cblas_saxpy(3, 1.0f, (float [3]){ (i - 1) * 1.5f, (i - 1) * 1.5f, 3.0f }, 1, &gl.transform[i][12], 1);
		glBindBuffer(GL_UNIFORM_BUFFER, gl.ubuf_transform[i]);
		glBufferData(GL_UNIFORM_BUFFER, sizeof gl.transform[i], gl.transform[i], GL_STATIC_DRAW);
	}

	/* Camera position */
	float camera_dpos[3];
	cblas_scopy(3, (float [3]){ global.inputstate.right, global.inputstate.up, global.inputstate.forward }, 1, camera_dpos, 1);
	normalise_vec(camera_dpos, 0.02f);
	apply_rotor(global.camera.rotor, camera_dpos);
	cblas_saxpy(3, 1.0f, camera_dpos, 1, global.camera.pos, 1);

	/* Camera matrix */
	float camrotation[16], camtransform[16] = IDENTITY_MATRIX;
	rotor_to_matrix(camrotation, (float [4]){ global.camera.rotor[0], -global.camera.rotor[1], -global.camera.rotor[2], -global.camera.rotor[3] });
	cblas_saxpy(3, -1.0f, global.camera.pos, 1, &camtransform[12], 1);
	cblas_sgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, 4, 4, 4, 1.0f, camrotation, 4, camtransform, 4, 0.0f, gl.view, 4);
	cblas_sgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, 4, 4, 4, 1.0f, gl.project, 4, gl.view, 4, 0.0f, gl.viewproject, 4);

	/* Update camera uniform buffer */
	memcpy(global.cameradata.mat, gl.viewproject, sizeof(float [16]));
	memcpy(global.cameradata.pos, global.camera.pos, sizeof(float [3]));
	glBindBuffer(GL_UNIFORM_BUFFER, gl.ubuf_camera);
	glBufferData(GL_UNIFORM_BUFFER, sizeof global.cameradata, &global.cameradata, GL_STATIC_DRAW);
}

int main(int argc, char *argv[]) {
	/* SDL variables */
	static SDL_Window *window;
	static SDL_GLContext *context;
	static struct winsize {
		int width, height;
	} winsize;

	global.targetfps = 128;
	global.sens = 0.0002f;

	/* Initialise SDL2 and create SDL2 window and context */
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	window = SDL_CreateWindow("doom", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1920, 1080, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_GRABBED);
	context = SDL_GL_CreateContext(window);
	SDL_SetWindowOpacity(window, 0.5f);
	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_ShowCursor(SDL_DISABLE);

	/* Initialise glut library */
	glutInit(&argc, argv);
	SDL_GetWindowSize(window, &winsize.width, &winsize.height);
	glViewport(0, 0, winsize.width, winsize.height);
	glClearColor(0.0f, 0.0f, 0.02f, 1.0f);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);

	/* Load, compile and link shaders */
	int success;
	gl.vshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(gl.vshader, 1, &shader_pf_vertex_glsl, NULL);
	glCompileShader(gl.vshader);
	glGetShaderiv(gl.vshader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		int len = 0;
		char infolog[128];
		glGetShaderInfoLog(gl.vshader, sizeof infolog, &len, infolog);
		fwrite(infolog, sizeof(char), len, stderr);
	}
	gl.fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(gl.fshader, 1, &shader_pf_fragment_glsl, NULL);
	glCompileShader(gl.fshader);
	glGetShaderiv(gl.fshader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		int len = 0;
		char infolog[128];
		glGetShaderInfoLog(gl.fshader, sizeof infolog, &len, infolog);
		fwrite(infolog, sizeof(char), len, stderr);
	}

	gl.shader_program = glCreateProgram();
	glAttachShader(gl.shader_program, gl.vshader);
	glAttachShader(gl.shader_program, gl.fshader);
	glLinkProgram(gl.shader_program);
	glGetProgramiv(gl.shader_program, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		int len = 0;
		char infolog[128];
		glGetProgramInfoLog(gl.shader_program, sizeof infolog, &len, infolog);
		fwrite(infolog, sizeof(char), len, stderr);
	}
	glUseProgram(gl.shader_program);

	/* Create vertex array, vertex buffer and index buffer objects */
	int normal_bufobj;

	glCreateVertexArrays(1, &gl.vertex_arrayobj);
	glBindVertexArray(gl.vertex_arrayobj);
	glGenBuffers(1, &gl.vertex_bufobj);
	glGenBuffers(1, &gl.index_bufobj);

	glBindBuffer(GL_ARRAY_BUFFER, gl.vertex_bufobj);
	glBufferData(GL_ARRAY_BUFFER, sizeof ncube_vdata, ncube_vdata, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl.index_bufobj);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof ncube_idata, ncube_idata, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)12);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	/* Create matrices */
	for (int i = 0; i < 3; i++) {
		cblas_saxpy(4, 1.0f, (float [4]){ 1.0f, 1.0f, 1.0f, 1.0f }, 1, gl.transform[i], 5);
		cblas_saxpy(3, 1.0f, (float [3]){ (i - 1) * 1.5f, (i - 1) * 1.5f, 3.0f }, 1, &gl.transform[i][12], 1);
	}
	cblas_saxpy(4, 1.0f, (float [4]){ 1.0f, 1.0f, 1.0f, 1.0f }, 1, gl.project, 5);
	project_matrix(gl.project, 90.0f, (float)winsize.width / winsize.height, 0.015625f, 2048.015625f);

	/* Cube transform UBOs */
	glGenBuffers(3, gl.ubuf_transform);
	for (int i = 0; i < 3; i++) {
		glBindBuffer(GL_UNIFORM_BUFFER, gl.ubuf_transform[i]);
		glBufferData(GL_UNIFORM_BUFFER, sizeof gl.transform[i], gl.transform[i], GL_STATIC_DRAW);
	}

	/* Camera view-projection matrix and position UBO */
	memcpy(global.cameradata.mat, gl.viewproject, sizeof(float [16]));
	memcpy(global.cameradata.pos, global.camera.pos, sizeof(float [3]));
	glGenBuffers(1, &gl.ubuf_camera);
	glBindBuffer(GL_UNIFORM_BUFFER, gl.ubuf_camera);
	glBufferData(GL_UNIFORM_BUFFER, sizeof global.cameradata, &global.cameradata, GL_DYNAMIC_DRAW);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, gl.ubuf_camera);

	/* Point light UBO */
	GLuint ubuf_pointlight;
	struct PointLightData plight = {
		.pos = { 6.0f, 6.0f, -6.0f },
		.colour = { 1.0f, 1.0f, 1.0f },
		.falloff = 0.01f
	};
	glGenBuffers(1, &ubuf_pointlight);
	glBindBuffer(GL_UNIFORM_BUFFER, ubuf_pointlight);
	glBufferData(GL_UNIFORM_BUFFER, sizeof plight, &plight, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, ubuf_pointlight);

	/* Material UBO */
	GLuint ubuf_asteroidmaterial;
	struct MaterialData redmaterial = {
		.diff_colour = { 0.5f, 0.5f, 0.5f },
		.spec_colour = { 0.8f, 0.8f, 0.8f },
		.roughness = 15.0f
	};
	glGenBuffers(1, &ubuf_asteroidmaterial);
	glBindBuffer(GL_UNIFORM_BUFFER, ubuf_asteroidmaterial);
	glBufferData(GL_UNIFORM_BUFFER, sizeof redmaterial, &redmaterial, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, ubuf_asteroidmaterial);

	/* Initialise rotors */
	global.cube.rotor[0] = global.cube.rotor_delta[0] = 1.0f;
	global.camera.rotor[0] = 1.0f;

	/* Set game clock */
	struct timespec monotime;
	clock_gettime(CLOCK_MONOTONIC, &monotime);
	int run = 1;
	while (run > 0) {
		/* Input */
		static SDL_Event event;
		pollevents(&event);

		/* Game */
		framestep();

		/* Render */
		SDL_GetWindowSize(window, &winsize.width, &winsize.height);
		glViewport(0, 0, winsize.width, winsize.height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for (int i = 0; i < 3; i++) {
			glBindBufferBase(GL_UNIFORM_BUFFER, 0, gl.ubuf_transform[i]);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void *)0);
		}
		SDL_GL_SwapWindow(window);

		/* Frame advance */
		monotime.tv_nsec += (1000000000 / global.targetfps);
		if (monotime.tv_nsec >= 1000000000) {
			monotime.tv_nsec -= 1000000000;
			monotime.tv_sec++;
		}
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &monotime, NULL);
	}

	return 0;
}
