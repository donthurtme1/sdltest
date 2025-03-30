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

/* SDL variables */
static SDL_Window *window;
static SDL_GLContext *context;
static struct winsize {
	int width, height;
} winsize;

/* GL variables */
static struct {
	GLuint vshader, fshader, program, vao, vbuf, ibuf;
	float transform[3][16];
	float view[16], project[16], viewproject[16];
	GLuint ubuf_transform[3], ubuf_viewproject;
	GLuint ubuf_camera;
} gl;

/* Global state */
int g_targetfps = 128;
float g_sens = 0.0002f; // Mouse sensitivity

struct InputSet g_inputstate;
struct Cube g_cube;
struct Camera g_camera;
struct CameraData g_cameradata;

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
				handle_keydown(event, &g_inputstate);
				break;
			case SDL_KEYUP:
				handle_keyup(event, &g_inputstate);
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
	combine_rotor(g_cube.rotor, g_cube.rotor_delta, g_cube.rotor);
	for (int i = 0; i < 3; i++) {
		rotor_to_matrix(gl.transform[i], g_cube.rotor);
		cblas_saxpy(3, 1.0f, (float [3]){ (i - 1) * 1.5f, (i - 1) * 1.5f, 3.0f }, 1, &gl.transform[i][12], 1);
		glBindBuffer(GL_UNIFORM_BUFFER, gl.ubuf_transform[i]);
		glBufferData(GL_UNIFORM_BUFFER, sizeof gl.transform[i], gl.transform[i], GL_STATIC_DRAW);
	}

	/* Camera transform */
	//combine_rotor(g_camera.rotor_df, g_camera.rotor, g_camera.rotor);

	/* Camera position */
	float camera_dpos[3];
	cblas_scopy(3, (float [3]){ g_inputstate.right, g_inputstate.up, g_inputstate.forward }, 1, camera_dpos, 1);
	normalise_vec(camera_dpos, 0.02f);
	apply_rotor(g_camera.rotor, camera_dpos);
	cblas_saxpy(3, 1.0f, camera_dpos, 1, g_camera.pos, 1);

	/* Camera matrix */
	float camrotation[16], camtransform[16] = IDENTITY_MATRIX;
	rotor_to_matrix(camrotation, (float [4]){ g_camera.rotor[0], -g_camera.rotor[1], -g_camera.rotor[2], -g_camera.rotor[3] });
	cblas_saxpy(3, -1.0f, g_camera.pos, 1, &camtransform[12], 1);
	cblas_sgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, 4, 4, 4, 1.0f, camrotation, 4, camtransform, 4, 0.0f, gl.view, 4);
	cblas_sgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, 4, 4, 4, 1.0f, gl.project, 4, gl.view, 4, 0.0f, gl.viewproject, 4);

	/* Update camera uniform buffer */
	memcpy(g_cameradata.mat, gl.viewproject, sizeof(float [16]));
	memcpy(g_cameradata.pos, g_camera.pos, sizeof(float [3]));
	glBindBuffer(GL_UNIFORM_BUFFER, gl.ubuf_camera);
	glBufferData(GL_UNIFORM_BUFFER, sizeof g_cameradata, &g_cameradata, GL_STATIC_DRAW);
}

int main(int argc, char *argv[]) {
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
	glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);

	/* Load, compile and link shaders */
	int success;
	gl.vshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(gl.vshader, 1, &perfragshader_vertex_glsl, NULL);
	glCompileShader(gl.vshader);
	glGetShaderiv(gl.vshader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		int len = 0;
		char infolog[128];
		glGetShaderInfoLog(gl.vshader, sizeof infolog, &len, infolog);
		fwrite(infolog, sizeof(char), len, stderr);
	}
	gl.fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(gl.fshader, 1, &perfragshader_fragment_glsl, NULL);
	glCompileShader(gl.fshader);
	glGetShaderiv(gl.fshader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		int len = 0;
		char infolog[128];
		glGetShaderInfoLog(gl.fshader, sizeof infolog, &len, infolog);
		fwrite(infolog, sizeof(char), len, stderr);
	}

	gl.program = glCreateProgram();
	glAttachShader(gl.program, gl.vshader);
	glAttachShader(gl.program, gl.fshader);
	glLinkProgram(gl.program);
	glGetProgramiv(gl.program, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		int len = 0;
		char infolog[128];
		glGetProgramInfoLog(gl.program, sizeof infolog, &len, infolog);
		fwrite(infolog, sizeof(char), len, stderr);
	}
	glUseProgram(gl.program);

	/* Create vertex array, vertex buffer and index buffer objects */
	glCreateVertexArrays(1, &gl.vao);
	glBindVertexArray(gl.vao);
	glGenBuffers(1, &gl.vbuf);
	glGenBuffers(1, &gl.ibuf);

	glBindBuffer(GL_ARRAY_BUFFER, gl.vbuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof ncube_vdata, ncube_vdata, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl.ibuf);
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
	project_matrix(gl.project, 90.0f, (float)winsize.width / winsize.height,
			0.015625f, 2048.015625f);

	/* Cube transform UBOs */
	glGenBuffers(3, gl.ubuf_transform);
	for (int i = 0; i < 3; i++) {
		glBindBuffer(GL_UNIFORM_BUFFER, gl.ubuf_transform[i]);
		glBufferData(GL_UNIFORM_BUFFER, sizeof gl.transform[i], gl.transform[i],
				GL_STATIC_DRAW);
	}

	/* Camera view-projection matrix and position UBO */
	memcpy(g_cameradata.mat, gl.viewproject, sizeof(float [16]));
	memcpy(g_cameradata.pos, g_camera.pos, sizeof(float [3]));
	glGenBuffers(1, &gl.ubuf_camera);
	glBindBuffer(GL_UNIFORM_BUFFER, gl.ubuf_camera);
	glBufferData(GL_UNIFORM_BUFFER, sizeof g_cameradata, &g_cameradata, GL_DYNAMIC_DRAW);
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
	GLuint ubuf_redmaterial;
	struct MaterialData redmaterial = {
		.diff_colour = { 1.0f, 0.2f, 0.2f },
		.spec_colour = { 0.8f, 0.8f, 0.8f },
		.roughness = 15.0f
	};
	glGenBuffers(1, &ubuf_redmaterial);
	glBindBuffer(GL_UNIFORM_BUFFER, ubuf_redmaterial);
	glBufferData(GL_UNIFORM_BUFFER, sizeof redmaterial, &redmaterial, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, ubuf_redmaterial);

	/* Initialise rotors */
	g_cube.rotor[0] = g_cube.rotor_delta[0] = 1.0f;
	g_camera.rotor[0] = 1.0f;

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
		monotime.tv_nsec += (1000000000 / g_targetfps);
		if (monotime.tv_nsec >= 1000000000) {
			monotime.tv_nsec -= 1000000000;
			monotime.tv_sec++;
		}
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &monotime, NULL);
	}

	return 0;
}
