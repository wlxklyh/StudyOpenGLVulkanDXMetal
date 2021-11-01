#version 330 core
out vec4 FragColor;

#define NR_POINT_LIGHTS 4

struct Material {
    sampler2D diffuceTex;
    sampler2D specularTex;
    float shininess;
};

struct PointLight {
    bool use;
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    //距离衰减
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    bool use;
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    //角度衰减
    float cutOff;
    float outerCutOff;
    //距离衰减
    float constant;
    float linear;
    float quadratic;
};

struct DirLight {
    bool use;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform Material material;
uniform vec3 viewPos;

uniform DirLight dirLights[NR_POINT_LIGHTS];
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLights[NR_POINT_LIGHTS];

vec3 CalcDirLight(DirLight light)
{
    if(light.use == false){
        return vec3(0.0f);
    }
    vec3 result;
    vec4 diffuseColor = texture(material.diffuceTex,TexCoords);
    vec4 specColor = texture(material.specularTex, TexCoords);
    //环境光
    vec3 ambient = light.ambient * diffuseColor.xyz;
    //漫反射
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(light.direction);
    float n_dot_l = max(dot(normal,-lightDir),0.0);
    vec3 diffuse = light.diffuse * diffuseColor.xyz * n_dot_l;
    //高光
    vec3 viewDir = normalize(viewPos-FragPos);
    vec3 reflectDir = reflect(-lightDir,normal);
    float r_dot_v = max(dot(viewDir,reflectDir),0);
    float spec = pow(r_dot_v,material.shininess);
    vec3 specular = light.specular * specColor.xyz * spec;

    result =  ambient + diffuse + specular;
    return result;
}

vec3 CalcPointLight(PointLight light)
{
    if(light.use == false){
        return vec3(0.0f);
    }
    vec3 result;
    vec4 diffuseColor = texture(material.diffuceTex,TexCoords);
    vec4 specColor = texture(material.specularTex, TexCoords);
    //环境光
    vec3 ambient = light.ambient * diffuseColor.xyz;
    //漫反射
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(FragPos - light.position);
    float n_dot_l = max(dot(normal,-lightDir),0.0);
    vec3 diffuse = light.diffuse * diffuseColor.xyz * n_dot_l;
    //高光
    vec3 viewDir = normalize(viewPos-FragPos);
    vec3 reflectDir = reflect(-lightDir,normal);
    float r_dot_v = max(dot(viewDir,reflectDir),0);
    float spec = pow(r_dot_v,material.shininess);
    vec3 specular = light.specular * specColor.xyz * spec;
    //衰减
    float distance = length(light.position - FragPos);
    float attenuation = 1 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    result =  (ambient + diffuse + specular)*attenuation;
    return result;
}

vec3 CalcSpotLight(SpotLight light)
{
    if(light.use == false){
        return vec3(0.0f);
    }
    vec3 result;
    vec4 diffuseColor = texture(material.diffuceTex,TexCoords);
    vec4 specColor = texture(material.specularTex, TexCoords);
    //环境光
    vec3 ambient = light.ambient * diffuseColor.xyz;
    //漫反射
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(FragPos - light.position);
    float n_dot_l = max(dot(normal,-lightDir),0.0);
    vec3 diffuse = light.diffuse * diffuseColor.xyz * n_dot_l;
    //高光
    vec3 viewDir = normalize(viewPos-FragPos);
    vec3 reflectDir = reflect(-lightDir,normal);
    float r_dot_v = max(dot(viewDir,reflectDir),0);
    float spec = pow(r_dot_v,material.shininess);
    vec3 specular = light.specular * specColor.xyz * spec;
    //衰减
    float distance = length(light.position - FragPos);
    float attenuation = 1 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    //soft spot
    float spotsoftDist = light.cutOff - light.outerCutOff;
    float thelta = dot(lightDir,normalize(light.direction));
    float intensity = clamp((thelta-light.outerCutOff)/spotsoftDist,0,1);
    result =  (ambient + diffuse * intensity + specular * intensity)*attenuation;

    return result;
}

void main()
{
    vec3 result;
    for(int i = 0;i < NR_POINT_LIGHTS;i++)
    {
        result += CalcPointLight(pointLights[i]);
        result += CalcDirLight(dirLights[i]);
        result += CalcSpotLight(spotLights[i]);
    }

    FragColor = vec4(result,1.0f);
}