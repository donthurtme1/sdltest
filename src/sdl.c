#include <openblas/cblas.h>
#include <time.h>
#include <string.h>
#include "shaders.h"
#include "types.h"

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <SDL2/SDL.h>

#define IDENTITY_MATRIX { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }

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

static float ncube_vdata[] = {
    // Create back face
     0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f,
    // left face
    -0.5f,  0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, 1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f,
    // bottom face
     0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f,
    // front face
    -0.5f,  0.5f,  0.5f, 0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f, 0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f, 0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f, 0.0f,  0.0f,  1.0f,
    // right face
     0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f, 1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
    // top face
     0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f,
};
static int ncube_idata[] = {
     0,  3,  2,  2,  1,  0,  // Create back face
     4,  7,  5,  5,  7,  6,  // Create left face
     8,  11, 9, 9,  11, 10,  // Create bottom face
    12, 15, 13, 13, 15, 14,  // Create front face
    16, 19, 17, 17, 19, 18,  // Create right face
    20, 23, 21, 21, 23, 22   // Create top face
};

static float cube_vdata[] = {
	/* Position */       /* Colour */
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
	5, 1, 0, 0, 4, 5, // Bottom face
};

/* Global state */
int targetfps = 128;
int run = 1; // Continue game loop
float sens = 0.0002f; // Mouse sensitivity
struct InputSet input_state;

struct Cube cube;
struct Camera camera;

/* Function definitions */
#include "rotor.c"
#include "matrix.c"
#include "input.c"

static void pollevents(SDL_Event *event);
static void framestep(void);

void pollevents(SDL_Event *event) {
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

void framestep(void) {
	/* Cube */
	combine_rotor(cube.rotor, cube.rotor_delta, cube.rotor);
	for (int i = 0; i < 3; i++) {
		rotor_to_matrix(transform[i], cube.rotor);
		cblas_saxpy(3, 1.0f, (float [3]){ (i - 1) * 1.5f, (i - 1) * 1.5f, 3.0f }, 1, &transform[i][12], 1);
		glBindBuffer(GL_UNIFORM_BUFFER, ubuf_transform[i]);
		glBufferData(GL_UNIFORM_BUFFER, sizeof transform[i], transform[i], GL_STATIC_DRAW);
	}

	/* Camera transform */
	combine_rotor(camera.rotor_df, camera.rotor, camera.rotor);
	if (input_state.right != 0 || input_state.up != 0 || input_state.forward != 0) {
		cblas_scopy(3, (float [3]){ input_state.right, input_state.up, input_state.forward }, 1, camera.pos_df, 1);
		normalise_vec(camera.pos_df, 0.02f);
		apply_rotor(camera.rotor, camera.pos_df);
		cblas_saxpy(3, 1.0f, camera.pos_df, 1, camera.pos, 1);
	}

	/* Camera matrix */
	float camrotation[16], camtransform[16] = IDENTITY_MATRIX;
	rotor_to_matrix(camrotation, (float [4]){ camera.rotor[0], -camera.rotor[1], -camera.rotor[2], -camera.rotor[3] });
	cblas_saxpy(3, -1.0f, camera.pos, 1, &camtransform[12], 1);
	cblas_sgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, 4, 4, 4, 1.0f, camrotation, 4, camtransform, 4, 0.0f, view, 4);
	cblas_sgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, 4, 4, 4, 1.0f, project, 4, view, 4, 0.0f, viewproject, 4);
	glBindBuffer(GL_UNIFORM_BUFFER, ubuf_viewproject);
	glBufferData(GL_UNIFORM_BUFFER, sizeof viewproject, viewproject, GL_STATIC_DRAW);
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
	vshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vshader, 1, &vertex_glsl, NULL);
	glCompileShader(vshader);
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		int len = 0;
		char infolog[128];
		glGetShaderInfoLog(vshader, sizeof infolog, &len, infolog);
		fwrite(infolog, sizeof(char), len, stderr);
	}
	fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshader, 1, &fragment_glsl, NULL);
	glCompileShader(fshader);
	glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		int len = 0;
		char infolog[128];
		glGetShaderInfoLog(fshader, sizeof infolog, &len, infolog);
		fwrite(infolog, sizeof(char), len, stderr);
	}

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vshader);
	glAttachShader(shader_program, fshader);
	glLinkProgram(shader_program);
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		int len = 0;
		char infolog[128];
		glGetProgramInfoLog(shader_program, sizeof infolog, &len, infolog);
		fwrite(infolog, sizeof(char), len, stderr);
	}
	glUseProgram(shader_program);

	/* Create vertex array, vertex buffer and index buffer objects */
	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbuf);
	glGenBuffers(1, &ibuf);

	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof ncube_vdata, ncube_vdata, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof ncube_idata, ncube_idata, GL_STATIC_DRAW);
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
	project_matrix(project, 90.0f, (float)winsize.width / winsize.height,
			0.015625f, 2048.015625f);

	/* Cube transform UBOs */
	glGenBuffers(3, ubuf_transform);
	for (int i = 0; i < 3; i++) {
		glBindBuffer(GL_UNIFORM_BUFFER, ubuf_transform[i]);
		glBufferData(GL_UNIFORM_BUFFER, sizeof transform[i], transform[i],
				GL_STATIC_DRAW);
	}

	/* View-projection matrix UBO */
	GLuint ubuf_camera;
	struct CameraData cameradata;
	memcpy(&cameradata.mat_viewproj, &project, sizeof(float [16]));
	memcpy(&cameradata.cam_pos, &camera.pos, sizeof(float [3]));
	glGenBuffers(1, &ubuf_viewproject);
	glBindBuffer(GL_UNIFORM_BUFFER, ubuf_viewproject);
	glBufferData(GL_UNIFORM_BUFFER, sizeof project, project, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubuf_viewproject);

	/* Point light UBO */
	GLuint ubuf_pointlight;
	struct PointLightData plight = { { 10, 10, 10 }, { 1, 1, 1 }, 0.01f };
	glGenBuffers(1, &ubuf_pointlight);
	glBindBuffer(GL_UNIFORM_BUFFER, ubuf_pointlight);
	glBufferData(GL_UNIFORM_BUFFER, sizeof plight, &plight, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, ubuf_pointlight);

	/* Material UBO */
	GLuint ubuf_redmaterial;
	struct MaterialData redmaterial = { { 0.8, 0.2, 0.2 }, { 0.9, 0.9, 0.9 }, 15.0f };
	glGenBuffers(1, &ubuf_redmaterial);
	glBindBuffer(GL_UNIFORM_BUFFER, ubuf_redmaterial);
	glBufferData(GL_UNIFORM_BUFFER, sizeof redmaterial, &redmaterial, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, ubuf_redmaterial);

	/* Initialise rotors */
	cube.rotor[0] = cube.rotor_delta[0] = 1.0f;
	camera.rotor[0] = camera.rotor_df[0] = 1.0f;

	static SDL_Event event;
	static struct timespec monotime;
	clock_gettime(CLOCK_MONOTONIC, &monotime);
	while (run > 0) {
		/* Input */
		pollevents(&event);

		/* Game */
		framestep();

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
		monotime.tv_nsec += (1000000000 / targetfps);
		if (monotime.tv_nsec >= 1000000000) {
			monotime.tv_nsec -= 1000000000;
			monotime.tv_sec++;
		}
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &monotime, NULL);
	}

	return 0;
}
