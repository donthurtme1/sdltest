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
struct __attribute__((aligned(8))) CameraData {
	float viewproj[16];
	float pos[3];
};

struct __attribute__((aligned(8))) PointLightData {
	float pos[3], colour[3];
	float falloff;
};

struct __attribute__((aligned(8))) MaterialData {
	float diff_colour[3], spec_colour[3];
	float roughness;
};

#endif
