#ifndef INCLUDE_SHADERS_H
#define INCLUDE_SHADERS_H

const char *const shader_pf_vertex_glsl = "\
#version 460\n\
\n\
/* Structures */\n\
struct CameraStruct {\n\
	mat4 mViewProj;\n\
	vec3 pos;\n\
};\n\
\n\
/* Uniforms */\n\
layout(binding = 0) uniform Transform {\n\
	mat4 mTransform;\n\
};\n\
layout(binding = 1) uniform Camera {\n\
	CameraStruct cam;\n\
};\n\
\n\
/* I/O variables */\n\
layout(location = 0) in vec3 in_pos;\n\
layout(location = 1) in vec3 in_normal;\n\
\n\
layout(location = 0) smooth out vec3 out_tpos; /* transformed pos */\n\
layout(location = 1) smooth out vec3 out_tnormal; /* transformed normal */\n\
\n\
void main() {\n\
	/* Output transformed vertex position */\n\
	vec4 tpos = cam.mViewProj * (mTransform * vec4(in_pos, 1.0f));\n\
	gl_Position = tpos;\n\
	out_tpos = tpos.xyz / tpos.w;\n\
\n\
	/* Output transformed normal vector */\n\
	out_tnormal = normalize((mTransform * vec4(in_normal, 0.0f)).xyz);\n\
}\n\
";

const char *const shader_pf_fragment_glsl = "\
#version 460\n\
\n\
/* Structures */\n\
struct CameraStruct {\n\
	mat4 mViewProj;\n\
	vec3 pos;\n\
};\n\
struct PointLightStruct {\n\
	vec3 pos;\n\
	vec3 colour; /* intensity */\n\
	float falloff;\n\
};\n\
struct MaterialStruct {\n\
	vec3 diffuse;\n\
	vec3 specular;\n\
	float roughness;\n\
};\n\
\n\
/* Uniforms */\n\
layout(binding = 0) uniform Transform {\n\
	mat4 mTransform;\n\
};\n\
layout(binding = 1) uniform Camera {\n\
	CameraStruct cam;\n\
};\n\
layout(std140, binding = 2) uniform PointLightData {\n\
	PointLightStruct pl;\n\
};\n\
layout(std140, binding = 3) uniform MaterialData {\n\
	MaterialStruct mtr;\n\
};\n\
\n\
/* I/O variables */\n\
layout(location = 0) in vec3 in_tpos; /* transformed vertex position */\n\
layout(location = 1) in vec3 in_tnormal; /* transformed normal */\n\
out vec4 out_colour;\n\
\n\
/* Functions */\n\
vec3 blinnphong(in vec3 n, in vec3 lightdir, in vec3 viewdir, in vec3 light_irradiance, in vec3 diffuse_colour, in vec3 specular_colour, in float roughness) {\n\
	vec3 H = normalize(viewdir + lightdir); /* half vector */\n\
	vec3 spec = pow(max(dot(n, H), 0.0f), roughness) * specular_colour;\n\
\n\
	vec3 view_colour = diffuse_colour * spec;\n\
	view_colour *= max(dot(n, lightdir), 0.0f);\n\
	view_colour *= light_irradiance;\n\
	return view_colour + (diffuse_colour * vec3(0.3f));\n\
}\n\
\n\
vec3 lightfalloff(in vec3 colour, in float falloff, in vec3 light_pos, in vec3 surface_pos) {\n\
	float r = distance(light_pos, surface_pos);\n\
	return colour / (r * r * falloff);\n\
}\n\
\n\
void\n\
main() {\n\
	/* Transform the normal */\n\
	//vec3 t_normal = normalize((mTransform * vec4(in_tnormal, 0.0f)).xyz);\n\
	//vec3 unit_pos = in_tpos.xyz;\n\
	vec3 view_dir =  normalize(cam.pos - in_tpos);\n\
	vec3 light_dir = normalize(pl.pos - in_tpos);\n\
\n\
	/* Calculate falloff */\n\
	vec3 light_irradiance = lightfalloff(pl.colour, pl.falloff, pl.pos, in_tpos);\n\
	vec3 colour = blinnphong(in_tnormal, light_dir, view_dir, light_irradiance, mtr.diffuse, mtr.specular, mtr.roughness);\n\
	out_colour = vec4(colour, 1.0f);\n\
}\n\
";

#endif