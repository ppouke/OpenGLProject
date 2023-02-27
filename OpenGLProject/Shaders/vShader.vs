#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

layout (std140, binding = 0) uniform Matrices {
    mat4 projection;
    mat4 view;
};

uniform mat4 model;

out vec2 texCoord;
out vec3 Normal;
out vec3 FragPos;

void main()
{
    gl_Position = projection * view * model * vec4(aPos,1.0f);
    texCoord = aTexCoord;
    Normal = mat3(transpose(inverse(model))) * aNormal; //Note: costly to calc inverse
   
    FragPos = vec3(model * vec4(aPos, 1.0f));
}