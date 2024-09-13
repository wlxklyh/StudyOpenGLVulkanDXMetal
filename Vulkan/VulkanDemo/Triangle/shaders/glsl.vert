#version 450
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

layout(location = 2) out VS_OUT {
	vec3 color;
} vs_out;

void main() {
	vs_out.color = color;
	gl_Position = vec4(position,1.0f);
}