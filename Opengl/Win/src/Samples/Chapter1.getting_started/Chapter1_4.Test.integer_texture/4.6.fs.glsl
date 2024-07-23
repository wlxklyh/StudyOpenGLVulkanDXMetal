#version 330 core
#define DEBUG
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
uniform sampler2D ourTexture0;
uniform float mixValue;
void main()
{
    vec4  color = texture(ourTexture0,TexCoord);
    ivec3  color_int = ivec3(color.r * 255.0, color.g * 255.0, color.b * 255.0);;
    FragColor = vec4(color_int.r/255.0,color_int.g/255.0,color_int.b/255.0,1);
}