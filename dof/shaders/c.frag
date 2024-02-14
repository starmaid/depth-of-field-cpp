#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;
in float focus;

uniform sampler2D colorTex;
uniform sampler2D depthTex;

void main()
{
    FragColor = texture(colorTex, TexCoords) - texture(depthTex, TexCoords);
}