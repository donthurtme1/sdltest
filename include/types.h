#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>
#include <stdint.h>

#define align(a) __attribute__((aligned(a)))

#define list_container(lp, type) (type *)(lp - offsetof(type, link))
#define list_for_each(pos, type, head) \
	for (type *pos = list_container(head->next, type); \
		&pos->link != head; \
		pos = list_container(pos->link.next, type))
struct ListHead {
	struct ListHead *next;
	struct ListHead *prev;
};

#define length_of(a) (sizeof(a) / sizeof(a[0]))
struct IbufferArrayList {
	struct IbufferArrayList *next;
	struct IbufferArrayList *prev;
	uint32_t array[60]; /* List of GL index buffer objects */
} align(16);



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



/* Game */
struct Player {
	align(8) float pos[3], velocity[3], accel[3];
	float health;
};

struct Asteroid {
	align(8) float pos[3], velocity[3];
	align(8) float rotor[4], rotor_d[4];
};

/* Structs for passing data to the vertex shader */
struct CameraData {
	align(16) float mat[16];
	align(16) float pos[3];
};

struct PointLightData {
	align(16) float pos[3];
	align(16) float colour[3];
	float falloff;
};

struct MaterialData {
	align(16) float diff_colour[3];
	align(16) float spec_colour[3];
	align(16) float roughness;
};

#endif
