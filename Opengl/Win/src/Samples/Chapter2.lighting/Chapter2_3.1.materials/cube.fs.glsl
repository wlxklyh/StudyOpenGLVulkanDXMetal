#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 Normal;
in vec3 FragPos;

uniform Material material;
uniform Light light;

void main()
{
    //ambient
    vec3 ambient = light.ambient * material.ambient;

    //diffuse
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * material.diffuse;

    //specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir,normal);
    float r_dot_v = dot(reflectDir,viewDir);
    float spec = pow(max(r_dot_v,0.0),material.shininess);
    vec3 specular = specularStrength * lightColor * spec;

    vec3 result = (ambient  + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}