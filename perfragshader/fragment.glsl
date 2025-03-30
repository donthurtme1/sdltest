#version 460

/* Structures */
struct CameraStruct {
	mat4 mViewProj;
	vec3 pos;
};
struct PointLightStruct {
	vec3 pos;
	vec3 colour; /* intensity */
	float falloff;
};
struct MaterialStruct {
	vec3 diffuse;
	vec3 specular;
	float roughness;
};

/* Uniforms */
layout(binding = 0) uniform Transform {
	mat4 mTransform;
};
layout(binding = 1) uniform Camera {
	CameraStruct cam;
};
layout(std140, binding = 2) uniform PointLightData {
	PointLightStruct pl;
};
layout(std140, binding = 3) uniform MaterialData {
	MaterialStruct mtr;
};

/* I/O variables */
layout(location = 0) in vec3 in_tpos; /* transformed vertex position */
layout(location = 1) in vec3 in_tnormal; /* transformed normal */
out vec4 out_colour;

/* Functions */
vec3 blinnphong(in vec3 n, in vec3 lightdir, in vec3 viewdir, in vec3 light_irradiance, in vec3 diffuse_colour, in vec3 specular_colour, in float roughness) {
	vec3 H = normalize(viewdir + lightdir); /* half vector */
	vec3 spec = pow(max(dot(n, H), 0.0f), roughness) * specular_colour;

	vec3 view_colour = diffuse_colour * spec;
	view_colour *= max(dot(n, lightdir), 0.0f);
	view_colour *= light_irradiance;
	return view_colour + (diffuse_colour * vec3(0.3f));
}

vec3 lightfalloff(in vec3 colour, in float falloff, in vec3 light_pos, in vec3 surface_pos) {
	float r = distance(light_pos, surface_pos);
	return colour / (r * r * falloff);
}

void
main() {
	/* Transform the normal */
	//vec3 t_normal = normalize((mTransform * vec4(in_tnormal, 0.0f)).xyz);
	//vec3 unit_pos = in_tpos.xyz;
	vec3 view_dir =  normalize(cam.pos - in_tpos);
	vec3 light_dir = normalize(pl.pos - in_tpos);

	/* Calculate falloff */
	vec3 light_irradiance = lightfalloff(pl.colour, pl.falloff, pl.pos, in_tpos);
	vec3 colour = blinnphong(in_tnormal, light_dir, view_dir, light_irradiance, mtr.diffuse, mtr.specular, mtr.roughness);
	out_colour = vec4(colour, 1.0f);
}
