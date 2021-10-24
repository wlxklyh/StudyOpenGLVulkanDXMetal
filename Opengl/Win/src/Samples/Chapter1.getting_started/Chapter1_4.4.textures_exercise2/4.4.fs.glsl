#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
uniform sampler2D ourTexture0;
uniform sampler2D ourTexture1;

void main()
{
    vec4 color0 = texture(ourTexture0,TexCoord);
    vec4 color1 = texture(ourTexture1,(vec2(1.0,1.0)-TexCoord) * 2);
    vec4 retColor = mix(color0 , color1, 0.2);
    FragColor = retColor;
}