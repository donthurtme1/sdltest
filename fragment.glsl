#version 460

layout(location = 1) in vec4 colour;

out vec4 fragcolour;

void
main() {
	fragcolour = colour;
}
