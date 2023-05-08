//Código fonte do Fragment Shader (em GLSL)
#version 400
out vec4 color;
in vec4 finalColor;
in vec2 texCoord;

uniform sampler2D tex_buffer;
uniform vec2 offsets;

void main()
{
    //color = finalColor;
    color = texture(tex_buffer,texCoord + offsets);
}