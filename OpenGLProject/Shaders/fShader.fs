#version 330 core
out vec4 FragColor;

in vec3 vertexPos;
in vec2 texCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
vec2 altTexCoord = vec2(texCoord.x, texCoord.y) * 2;
FragColor = mix(texture(texture1, texCoord), texture(texture2, altTexCoord), 0.2);
}