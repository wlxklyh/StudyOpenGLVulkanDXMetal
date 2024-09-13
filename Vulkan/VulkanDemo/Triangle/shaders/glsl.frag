#version 450
layout(location = 2) in VS_OUT {
	vec3 color;
} fs_in;

layout (location = 0) out vec4 color;

void main() {
	color = vec4(1.0f,0.0f,0.0f,1.0f);
}