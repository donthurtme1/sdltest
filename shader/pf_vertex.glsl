#version 460

/* Structures */
struct CameraStruct {
	mat4 mViewProj;
	vec3 pos;
};

/* Uniforms */
layout(binding = 0) uniform Transform {
	mat4 mTransform;
};
layout(binding = 1) uniform Camera {
	CameraStruct cam;
};

/* I/O variables */
layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_normal;

layout(location = 0) smooth out vec3 out_tpos; /* transformed pos */
layout(location = 1) smooth out vec3 out_tnormal; /* transformed normal */

void main() {
	/* Output transformed vertex position */
	vec4 tpos = cam.mViewProj * (mTransform * vec4(in_pos, 1.0f));
	gl_Position = tpos;
	out_tpos = tpos.xyz / tpos.w;

	/* Output transformed normal vector */
	out_tnormal = normalize((mTransform * vec4(in_normal, 0.0f)).xyz);
}
