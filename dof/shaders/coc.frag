#version 330 core
in vec2 TexCoords;

uniform sampler2D colorTex;
uniform sampler2D depthTex;

uniform float _FocalPlaneDistance;
uniform float _FocusRange;

bool _ConsiderSkyInfinity = true;

out vec4 FragColor;

void main()
{
    float nearBegin = max(0.0f, _FocalPlaneDistance - _FocusRange);
    float nearEnd = _FocalPlaneDistance;
    float farBegin = _FocalPlaneDistance;
    float farEnd = _FocalPlaneDistance + _FocusRange;

    float depth = texture(depthTex, TexCoords).r;
    // 0.5 is about a meter?

    float nearCOC = 0.0f;
    if (depth < nearEnd)
        nearCOC = 1.0f / (nearBegin - nearEnd) * depth + -nearEnd / (nearBegin - nearEnd);
    else if (depth < nearBegin)
        nearCOC = 1.0f;
    
    float farCOC = 1.0f;
    if (depth < farBegin)
        farCOC = 0.0f;
    else if (depth < farEnd)
        farCOC = 1.0f / (farEnd - farBegin) * depth + -farBegin / (farEnd - farBegin);
    
    if (depth >= 999.0f && _ConsiderSkyInfinity)
        farCOC = 1.0f;

    if (nearCOC > 1f)
        nearCOC = 1f;
    if (farCOC > 1f)
        farCOC = 1f;

    FragColor = vec4(nearCOC,farCOC,0,1);
}


