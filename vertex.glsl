#version 460

/* In variables */
layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_normal;

/* Uniforms */
layout(binding = 0) uniform Transform {
	mat4 mat_transform;
};

layout(binding = 1) uniform Camera {
	mat4 mat_viewproj;
	vec3 cam_pos;
};

layout(std140, binding = 2) uniform PointLightData {
	vec3 pl_pos;
	vec3 pl_intensity;
	float pl_falloff;
};

layout(std140, binding = 3) uniform MaterialData {
	vec3 diff_colour;
	vec3 spec_colour;
	float roughness;
};

/* Out variables */
layout(location = 0) smooth out vec3 out_vcolour;

/* Functions */
vec3 blinnphong(in vec3 in_normal, in vec3 lightdir, in vec3 viewdir, in vec3
		light_irradiance, in vec3 diffuse_colour, in vec3 specular_colour, in
		float roughness) {
	vec3 H = normalize(viewdir + lightdir);
	vec3 S = pow(max(dot(in_normal, H), 0.0f), roughness) * specular_colour;

	vec3 view_colour = diffuse_colour + S;
	view_colour *= max(dot(in_normal, lightdir), 0.0f);
	view_colour *= light_irradiance;
	return view_colour;
}

vec3 lightfalloff(in vec3 intensity, in float falloff, in vec3 light_pos, in vec3
		surface_pos) {
	float r = distance(light_pos, surface_pos);
	return intensity / (falloff * r * r);
}

void main() {
	vec4 t_pos = mat_viewproj * (mat_transform * vec4(in_pos, 1.0f));
	gl_Position = t_pos;

	/* Transform the normal */
	vec3 t_normal = (mat_transform * vec4(in_normal, 0.0f)).xyz;
	vec3 unit_pos = t_pos.xyz / t_pos.w;
	vec3 view_dir =  normalize(cam_pos - in_pos);
	vec3 light_dir = normalize(pl_pos - in_pos);

	/* Calculate falloff */
	vec3 light_irradiance = lightfalloff(pl_intensity, pl_falloff, pl_pos, in_pos);
	out_vcolour = blinnphong(t_normal, light_dir, view_dir, light_irradiance,
			diff_colour, spec_colour, roughness);
	//out_vcolour = spec_colour;
}
