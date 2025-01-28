#ifndef SHADERS_H
#define SHADERS_H

const char *const vertex_glsl = "\
#version 460\n\
\n\
layout(location = 0) in vec3 pos;\n\
layout(location = 1) in vec3 colour;\n\
\n\
layout(location = 1) out vec4 vert_colour;\n\
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
	vert_colour = vec4(colour, 1.0f);\n\
	gl_Position = mat_viewproj * (mat_transform * vec4(pos, 1.0f));\n\
}\n\
";

const char *const fragment_glsl = "\
#version 460\n\
\n\
layout(location = 1) in vec4 colour;\n\
\n\
out vec4 fragcolour;\n\
\n\
void\n\
main() {\n\
	fragcolour = colour;\n\
}\n\
";

#endif