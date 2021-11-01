#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuceTex;
    sampler2D specularTex;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;
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
    vec4 specColor = texture(material.specularTex, TexCoords);
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
    vec3 specular = specColor.xyz * light.specular * spec;

    //spot
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // attenuation
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 result =  ambient;
    result =  ambient * attenuation + (diffuse + specular) * intensity * attenuation;

//    if(theta > light.cutOff){
//        result =  ambient + (diffuse + specular)*intensity;
//    } else
//    {
//        result = ambient;
//    }

    FragColor = vec4(result,1.0f);
}