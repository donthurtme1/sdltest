#ifndef TYPES_H
#define TYPES_H

/* Struct containing which buttons are being pressed */
struct InputSet {
	int right, up, forward;
};

/* Contains rotation and change in rotation of a cube */
struct Cube {
	float rotor[4];
	float rotor_delta[4];
};

/* Camera position and rotation */
struct Camera {
	float pos[3];
	float rotor[4];
};

/* Structs for passing data to the vertex shader */
#define GL_ALIGN __attribute__((aligned(16)))

struct CameraData {
	GL_ALIGN float mat[16];
	GL_ALIGN float pos[3];
};

struct PointLightData {
	GL_ALIGN float pos[3];
	GL_ALIGN float colour[3];
	float falloff;
};

struct MaterialData {
	GL_ALIGN float diff_colour[3];
	GL_ALIGN float spec_colour[3];
	GL_ALIGN float roughness;
};

#endif
