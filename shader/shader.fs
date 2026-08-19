#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 ourFragColor;

uniform sampler2D texture1;

void main()
{   
    // first arg as texture sampler and as its second argument the corresponding texture coordinates
    FragColor = texture(texture1, TexCoord) * vec4(ourFragColor, 1.0f);
    // FragColor = vec4(ourFragColor, 1.0f);
}