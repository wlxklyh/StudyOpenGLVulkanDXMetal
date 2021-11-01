#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuceTex;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main()
{
    vec4 diffuseColor = texture(material.diffuceTex,TexCoords);
    //ambient
    vec3 ambient = light.ambient * diffuseColor.xyz;

    //diffuse
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * diffuseColor.xyz;

    //specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir,normal);
    float r_dot_v = dot(reflectDir,viewDir);
    float spec = pow(max(r_dot_v,0.0),material.shininess);
    vec3 specular = material.specular * light.specular * spec;
    vec3 result = ambient  + diffuse + specular;
    FragColor = vec4(diffuse,1.0f);
}