#version 460

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 colour;

layout(location = 1) out vec4 vert_colour;

layout(binding = 0) uniform Transform {
	mat4 mat_transform;
};

layout(binding = 1) uniform Camera {
	mat4 mat_viewproj;
};

void
main() {
	vert_colour = vec4(colour, 1.0f);
	gl_Position = mat_viewproj * (mat_transform * vec4(pos, 1.0f));
}
