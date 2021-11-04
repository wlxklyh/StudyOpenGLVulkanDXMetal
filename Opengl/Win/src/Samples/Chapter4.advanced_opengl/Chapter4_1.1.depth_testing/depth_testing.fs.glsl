#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in float VS_Z;
in vec4 FragPos;

uniform sampler2D texture1;
uniform int DepthCase;
uniform float far;
uniform float near;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

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
    else if(DepthCase == 4){
        vec4 clipPosition = projection * view * FragPos;
        vec4 ndcPosition = clipPosition/clipPosition.w;
        FragColor = vec4(vec3(ndcPosition.z),1.0);
    }
    else if(DepthCase == 5){
        vec4 viewPosition = view * FragPos;
        FragColor = vec4(vec3(-viewPosition.z/far),1.0);
    }
    else if(DepthCase == 6)
    {
        FragColor = texture(texture1, TexCoords);
    }
    else
    {
        FragColor = texture(texture1, TexCoords);
    }
    int worldZ = int(FragPos.z);
    int worldX = int(FragPos.x);
    if(FragPos.z - worldZ < 0.05&& FragPos.z - worldZ >-0.05){
        FragColor = vec4(1.0,0.0,0.0,1.0);
    }
    if(FragPos.x - worldX < 0.05 && FragPos.x - worldX >-0.05){
        FragColor = vec4(1.0,0.0,0.0,1.0);
    }

}