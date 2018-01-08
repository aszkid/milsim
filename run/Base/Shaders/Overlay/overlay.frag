#version 420 core

layout (binding = 0) uniform sampler2D overlay;

out vec4 FragColor;
in vec2 texCoord;

void main()
{
    FragColor = texture(overlay, texCoord);
}
