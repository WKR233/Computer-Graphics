#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceMatrix;

out vec3 LightColor;
out vec3 ObjectColor;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 ocolor;
uniform vec3 lcolor;

void main()
{
    ObjectColor=ocolor;
    LightColor=lcolor;
    Normal=mat3(transpose(inverse(aInstanceMatrix)))*aNormal;
    gl_Position=projection*view*aInstanceMatrix*vec4(aPos, 1.0f); 
}