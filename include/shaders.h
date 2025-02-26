#ifndef INCLUDE_SHADERS_H
#define INCLUDE_SHADERS_H

const char *const vertex_glsl = "\
#version 460\n\
\n\
/* Types */\n\
struct PointLight {\n\
	vec3 pos;\n\
	vec3 intensity;\n\
	float falloff;\n\
};\n\
\n\
/* In variables */\n\
layout(location = 0) in vec3 in_pos;\n\
layout(location = 1) in vec3 in_normal;\n\
\n\
/* Uniforms */\n\
layout(binding = 0) uniform Transform {\n\
	mat4 mat_transform;\n\
};\n\
\n\
layout(binding = 1) uniform Camera {\n\
	mat4 mat_viewproj;\n\
	vec3 cam_pos;\n\
};\n\
\n\
layout(std140, binding = 2) uniform PointLightData {\n\
	PointLight pointlights;\n\
};\n\
\n\
layout(std140, binding = 3) uniform MaterialData {\n\
	vec3 diffuse_colour;\n\
	vec3 specular_colour;\n\
	float roughness;\n\
};\n\
\n\
/* Out variables*/\n\
layout(location = 0) smooth out vec3 out_vcolour;\n\
\n\
/* Functions */\n\
vec3 blinnphong(in vec3 in_normal, in vec3 lightdir, in vec3 viewdir, in vec3 light_irradiance, in vec3 diffuse_colour, in vec3 specular_colour, in float roughness) {\n\
	vec3 H = normalize(viewdir + lightdir);\n\
	vec3 S = pow(max(dot(in_normal, H), 0.0f), roughness) * specular_colour;\n\
\n\
	vec3 view_colour = diffuse_colour + S;\n\
	view_colour *= max(dot(in_normal, lightdir), 0.0f);\n\
	view_colour *= light_irradiance;\n\
	return view_colour;\n\
}\n\
\n\
vec3 lightfalloff(in vec3 intensity, in float falloff, in vec3 light_position, in vec3 surface_position) {\n\
	float r = distance(light_position, surface_position);\n\
	return intensity / (falloff * r * r);\n\
}\n\
\n\
void\n\
main() {\n\
	vec4 t_pos = mat_viewproj * (mat_transform * vec4(in_pos, 1.0f));\n\
	gl_Position = t_pos;\n\
\n\
	/* Transform the normal */\n\
	vec3 t_normal = (mat_transform * vec4(in_normal, 0.0f)).xyz;\n\
	vec3 unit_pos = t_pos.xyz / t_pos.w;\n\
	vec3 view_direction =  normalize(cam_pos - in_pos);\n\
	vec3 light_direction = normalize(pointlights.pos - in_pos);\n\
\n\
	/* Calculate falloff */\n\
	vec3 light_irradiance =  lightfalloff(pointlights.intensity, pointlights.falloff, pointlights.pos, in_pos);\n\
	out_vcolour = blinnphong(t_normal, light_direction, view_direction, light_irradiance, diffuse_colour, specular_colour, roughness);\n\
}\n\
";

const char *const fragment_glsl = "\
#version 460\n\
\n\
layout(location = 0) in vec3 colour;\n\
\n\
out vec4 fragcolour;\n\
\n\
void\n\
main() {\n\
	fragcolour = vec4(colour, 1.0f);\n\
}\n\
";

#endif