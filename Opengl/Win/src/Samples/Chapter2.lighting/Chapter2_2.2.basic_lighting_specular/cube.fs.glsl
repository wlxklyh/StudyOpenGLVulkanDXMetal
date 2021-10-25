#version 330 core

in vec3 Normal;
in vec3 FragPos;
out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    //ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    //diffuse
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    //specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 reflectDir = reflect(-lightDir,normal);
    float r_dot_v = dot(reflectDir,viewDir);

    vec3 h = (lightDir + viewDir) / 2;
    float n_dot_h = dot(h , normal);

    float spec = pow(max(r_dot_v,0.0),16);
//    float spec = pow(max(n_dot_h,0.0),32);

    vec3 specular = specularStrength * lightColor * spec;

    vec3 result = (ambient  + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}