#include <openblas/cblas.h>
#include "shaders.h"

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <SDL2/SDL.h>

static SDL_Window *window;
static SDL_GLContext *context;
static struct winsize {
	int width, height;
} winsize;
static GLuint vshader, fshader, shader_program, vao, vbuf, ibuf;

/* Matrix stuff */
static float transform[16], rotation[16];
static float view[16], project[16], viewproject[16];
static GLuint ubuf_transform, ubuf_project;

static float vdata[] = {
	 0.2f,  0.2f, 2.0f, 1.0f,
	-0.2f,  0.2f, 2.0f, 1.0f,
	-0.2f, -0.2f, 2.0f, 1.0f,
	 0.2f, -0.2f, 2.0f, 1.0f,
};
static float transformed_data[16];

static unsigned int idata[] = {
	0, 1, 2, 2, 3, 0
};

extern void debug_calcndc(float *m);
extern void printmatrix(char *s, float *mat, int m, int n);
extern void projectmat(float *m, float fov, float r, float near, float far);
extern void xrotation(float *m, float deg);
extern void yrotation(float *m, float deg);
extern void zrotation(float *m, float deg);

int
main(int argc, char *argv[]) {
	/* Initialise SDL2 and create SDL2 window and context */
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	window = SDL_CreateWindow("doom", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1920, 1080, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window == NULL) {
		fprintf(stderr, "Fucking no window\n");
		return 1;
	}
	SDL_SetWindowOpacity(window, 0.5f);
	context = SDL_GL_CreateContext(window);
	if (context == NULL) {
		fprintf(stderr, "Fucking no context\n");
		return 1;
	}

	/* Initialise glut library */
	glutInit(&argc, argv);
	SDL_GetWindowSize(window, &winsize.width, &winsize.height);
	glViewport(0, 0, winsize.width, winsize.height);
	glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);
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
	glBufferData(GL_ARRAY_BUFFER, sizeof vdata, vdata, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof idata, idata, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	/* Create transform matrix */
	cblas_saxpy(4, 1.0f, (float [4]){ 1.0f, 1.0f, 1.0f, 1.0f }, 1, project, 5);
	projectmat(project, 90.0f, (float)winsize.width / winsize.height, 0.1f, 1000.0f);
	printmatrix("projection", project, 4, 4);
	for (int i = 0; i < 4; i++) {
		cblas_sgemv(CblasColMajor, CblasNoTrans, 4, 4, 1.0f, project, 4, &vdata[4 * i], 1, 0.0f, &transformed_data[4 * i], 1);
	}
	printmatrix("transformed coords", transformed_data, 4, 4);
	debug_calcndc(transformed_data);

	/* Create uniform buffer objects */
	glGenBuffers(1, &ubuf_transform);
	glBindBuffer(GL_UNIFORM_BUFFER, ubuf_transform);
	glBufferData(GL_UNIFORM_BUFFER, sizeof transform, transform, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubuf_transform);
	glGenBuffers(1, &ubuf_project);
	glBindBuffer(GL_UNIFORM_BUFFER, ubuf_project);
	glBufferData(GL_UNIFORM_BUFFER, sizeof project, project, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubuf_project);

	static int run = 1;
	static SDL_Event event;
	while (run > 0) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					run = 0;
					break;
			}
		}
		SDL_GetWindowSize(window, &winsize.width, &winsize.height);
		glViewport(0, 0, winsize.width, winsize.height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)0);
		SDL_GL_SwapWindow(window);
	}

	return 0;
}
