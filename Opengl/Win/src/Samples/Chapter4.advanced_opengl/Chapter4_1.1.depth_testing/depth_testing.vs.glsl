#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
out vec4 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int DepthCase;

void main()
{
    TexCoords = aTexCoords;    
    gl_Position = projection * view * model * vec4(aPos, 1.0);// -Z Z  W = Z  (1) Z/W => -1 1
    FragPos = vec4(model * vec4(aPos,1.0));
    FragPos.w = 1.0;
}