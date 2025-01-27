#version 460

layout(location = 0) in vec4 pos;

layout(binding = 0) uniform Transform {
	mat4 mat_transform;
};

layout(binding = 1) uniform Camera {
	mat4 mat_viewproj;
};

void
main() {
	vec4 npos = mat_viewproj * pos;
	vec3 tpos = vec3(npos.x / npos.w, npos.y / npos.w, npos.z / npos.w);
	gl_Position = pos;
}
