#version 430
out vec4 FragColor;
in vec3 ourColor;
layout(early_fragment_tests) in;

uniform sampler2D ourTexture;
uniform int MaxFragmentCount;


layout(binding = 0, offset = 0) uniform atomic_uint counter;

void main()
{
    atomicCounterIncrement(counter);
    FragColor = vec4(ourColor, 1.0);
}