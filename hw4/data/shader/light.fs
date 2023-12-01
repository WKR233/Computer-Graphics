#version 330 core
out vec4 FragColor;

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    float shininess;
};

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in mat3 TBN;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    vec3 ambient=light.ambient*texture(material.diffuse,TexCoords).rgb;

    vec3 norm=texture(material.normal,TexCoords).rgb;
    norm=normalize(norm*2.0-1.0);
    //use TBN
    norm=normalize(TBN*norm);
 
    vec3 lightDir=normalize(light.position-FragPos);
    float diff=max(dot(norm,lightDir),0.0);
    vec3 diffuse=light.diffuse*diff*texture(material.diffuse,TexCoords).rgb;

    vec3 viewDir=normalize(viewPos-FragPos);
    vec3 reflectDir=reflect(-lightDir,norm);
    vec3 halfwayDir=normalize(lightDir+viewDir);
    float spec=pow(max(dot(norm,halfwayDir),0.0),material.shininess);
    vec3 specular=light.specular*spec*texture(material.specular,TexCoords).rgb;

    vec3 result=ambient+diffuse+specular;
    FragColor=vec4(result,1.0);
}