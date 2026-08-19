#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

uniform float x_offset;
uniform float y_offset;

out vec3 ourFragColor;
out vec2 TexCoord;

void main()
{
    gl_Position = vec4(vec3(aPos.x + x_offset, aPos.y + y_offset, aPos.z), 1.0);
    ourFragColor = aColor;
    TexCoord = aTexCoord;
}