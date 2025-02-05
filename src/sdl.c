#include <openblas/cblas.h>
#include <time.h>
#include <string.h>
#include "rotor.h"
#include "matrix.h"
#include "input.h"
#include "shaders.h"

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <SDL2/SDL.h>

#define IDENTITY_MATRIX { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }

/* Stores other player's data */
struct Player {
	float pos[3], vel[3], accel[3], jerk[3];
	float rotor[4], rotor_delta[4];
	short rtr_2df[4], rtr_3df[4]; // Used for prediction to mitigate lag
};

/* SDL stuff */
static SDL_Window *window;
static SDL_GLContext *context;
static struct winsize {
	int width, height;
} winsize;
static GLuint vshader, fshader, shader_program, vao, vbuf, ibuf;

/* Matrix stuff */
static float transform[3][16];
static float view[16], project[16], viewproject[16];
static GLuint ubuf_transform[3], ubuf_viewproject;

static float square_vdata[] = {
	 0.5f,  0.5f, 0.0f,
	-0.5f,  0.5f, 0.0f,
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
};
static unsigned int square_idata[] = {
	0, 1, 2, 2, 3, 0
};

static float cube_vdata[] = {
	-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
};
static unsigned int cube_idata[] = {
	0, 1, 2, 2, 3, 0, // Front face 
	6, 5, 4, 4, 7, 6, // Back face
	1, 5, 6, 6, 2, 1, // Right face
	4, 0, 3, 3, 7, 4, // Left face
	3, 2, 6, 6, 7, 3, // Top face
	0, 4, 5, 5, 1, 0  // Bottom face
};

/* Global state */
int fps = 128;
int run = 1;

struct Cube {
	float rotor[4], rotor_delta[4];
} cube;
struct Camera {
	float pos[3], pos_df[3];
	float rotor[4], rotor_df[4];
} camera;

enum InputAction input_state;

/* Function definitions */
static void PollEvents(SDL_Event *event);
static void Framestep(void);

void
PollEvents(SDL_Event *event) {
	while (SDL_PollEvent(event)) {
		switch (event->type) {
			case SDL_KEYDOWN:
				handle_keydown(event);
				break;
			case SDL_KEYUP:
				handle_keyup(event);
				break;
			case SDL_MOUSEMOTION:
				handle_mouse(event);
				break;
			case SDL_QUIT:
				run = 0;
				break;
		}
	}
}

void
Framestep(void) {
	/* Cube */
	combine_rotor(cube.rotor_delta, cube.rotor, cube.rotor);
	for (int i = 0; i < 3; i++) {
		rotor_to_matrix(transform[i], cube.rotor);
		cblas_saxpy(3, 1.0f, (float [3]){ (i - 1) * 1.5f, (i - 1) * 1.5f, 3.0f }, 1, &transform[i][12], 1);
		glBindBuffer(GL_UNIFORM_BUFFER, ubuf_transform[i]);
		glBufferData(GL_UNIFORM_BUFFER, sizeof transform[i], transform[i], GL_STATIC_DRAW);
	}

	/* Camera transform */
	combine_rotor(camera.rotor_df, camera.rotor, camera.rotor);
	if (input_state & FORWARD || input_state & BACK) {
		cblas_scopy(3, (float [3]){ 0, 0, 1 }, 1, camera.pos_df, 1);
		normalise_vec(camera.pos_df, 0.02f);
		apply_rotor(camera.rotor, camera.pos_df);
	} else {
		memset(camera.pos_df, 0, sizeof camera.pos_df);
	}
	cblas_saxpy(3, 1.0f, camera.pos_df, 1, camera.pos, 1);

	/* Camera matrix */
	float camrotation[16], camtransform[16] = IDENTITY_MATRIX;
	rotor_to_matrix(camrotation, camera.rotor);
	cblas_saxpy(3, -1.0f, camera.pos, 1, &camtransform[12], 1);
	cblas_sgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, 4, 4, 4, 1.0f, camrotation, 4, camtransform, 4, 0.0f, view, 4);
	cblas_sgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, 4, 4, 4, 1.0f, project, 4, view, 4, 0.0f, viewproject, 4);
	glBindBuffer(GL_UNIFORM_BUFFER, ubuf_viewproject);
	glBufferData(GL_UNIFORM_BUFFER, sizeof viewproject, viewproject, GL_STATIC_DRAW);
}

int
main(int argc, char *argv[]) {
	/* Initialise SDL2 and create SDL2 window and context */
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	window = SDL_CreateWindow("doom", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1920, 1080, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_GRABBED);
	SDL_SetWindowOpacity(window, 0.5f);
	context = SDL_GL_CreateContext(window);
	SDL_SetRelativeMouseMode(SDL_TRUE);

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
	vshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vshader, 1, &vertex_glsl, NULL);
	glCompileShader(vshader);
	fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshader, 1, &fragment_glsl, NULL);
	glCompileShader(fshader);

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vshader);
	glAttachShader(shader_program, fshader);
	glLinkProgram(shader_program);
	glUseProgram(shader_program);

	/* Create vertex array, vertex buffer and index buffer objects */
	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbuf);
	glGenBuffers(1, &ibuf);

	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof cube_vdata, cube_vdata, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof cube_idata, cube_idata, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)12);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	/* Create matrices */
	for (int i = 0; i < 3; i++) {
		cblas_saxpy(4, 1.0f, (float [4]){ 1.0f, 1.0f, 1.0f, 1.0f }, 1, transform[i], 5);
		cblas_saxpy(3, 1.0f, (float [3]){ (i - 1) * 1.5f, (i - 1) * 1.5f, 3.0f }, 1, &transform[i][12], 1);
	}
	cblas_saxpy(4, 1.0f, (float [4]){ 1.0f, 1.0f, 1.0f, 1.0f }, 1, project, 5);
	project_matrix(project, 90.0f, (float)winsize.width / winsize.height, 0.015625f, 2048.015625f);

	/* Create uniform buffer objects */
	glGenBuffers(3, ubuf_transform);
	for (int i = 0; i < 3; i++) {
		glBindBuffer(GL_UNIFORM_BUFFER, ubuf_transform[i]);
		glBufferData(GL_UNIFORM_BUFFER, sizeof transform[i], transform[i], GL_STATIC_DRAW);
	}
	glGenBuffers(1, &ubuf_viewproject);
	glBindBuffer(GL_UNIFORM_BUFFER, ubuf_viewproject);
	glBufferData(GL_UNIFORM_BUFFER, sizeof project, project, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubuf_viewproject);

	/* Initialise rotors */
	cube.rotor[0] = cube.rotor_delta[0] = 1.0f;
	camera.rotor[0] = camera.rotor_df[0] = 1.0f;

	static SDL_Event event;
	static struct timespec monotime;
	clock_gettime(CLOCK_MONOTONIC, &monotime);
	while (run > 0) {
		PollEvents(&event);

		/* Update */
		Framestep();

		/* Render */
		SDL_GetWindowSize(window, &winsize.width, &winsize.height);
		glViewport(0, 0, winsize.width, winsize.height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for (int i = 0; i < 3; i++) {
			glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubuf_transform[i]);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void *)0);
		}
		SDL_GL_SwapWindow(window);

		/* Frame advance */
		monotime.tv_nsec += (1000000000 / fps);
		if (monotime.tv_nsec >= 1000000000) {
			monotime.tv_nsec -= 1000000000;
			monotime.tv_sec++;
		}
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &monotime, NULL);
	}

	return 0;
}
