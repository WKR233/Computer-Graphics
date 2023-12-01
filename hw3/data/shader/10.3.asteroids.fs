#version 330 core
out vec4 FragColor;

in vec3 LightColor;
in vec3 ObjectColor;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightpos;

void main()
{
    float AmbientStrength=0.1;
    vec3 norm=normalize(Normal);
    vec3 lightDir=normalize(lightpos-FragPos);
    float Diff=max(dot(norm,lightDir),0.0);
    vec3 Diffuse=Diff*LightColor;
    vec3 Ambient=AmbientStrength*LightColor;
    vec3 ParticleColor=(Ambient+Diff)*ObjectColor;
    FragColor=vec4(ParticleColor,1.0f);
}