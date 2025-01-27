#ifndef SHADERS_H
#define SHADERS_H

const char *const vertex_glsl = "\
#version 460\n\
\n\
layout(location = 0) in vec4 pos;\n\
\n\
layout(binding = 0) uniform Transform {\n\
	mat4 mat_transform;\n\
};\n\
\n\
layout(binding = 1) uniform Camera {\n\
	mat4 mat_viewproj;\n\
};\n\
\n\
void\n\
main() {\n\
	vec4 npos = mat_viewproj * pos;\n\
	vec3 tpos = vec3(npos.x / npos.w, npos.y / npos.w, npos.z / npos.w);\n\
	gl_Position = pos;\n\
}\n\
";

const char *const fragment_glsl = "\
#version 460 core\n\
\n\
out vec4 fragcolour;\n\
\n\
void\n\
main() {\n\
	fragcolour = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n\
}\n\
";

#endif