#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;
uniform int DepthCase;
uniform float far;
uniform float near;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}
void main()
{
    if(DepthCase == 2)
    {
        FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
    }
    else if(DepthCase == 3)
    {
        float depth = LinearizeDepth(gl_FragCoord.z) / far; // 为了演示除以 far
        FragColor = vec4(vec3(depth), 1.0);
    }
    else
    {
        FragColor = texture(texture1, TexCoords);
    }
}