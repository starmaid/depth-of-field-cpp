#version 330 core
in vec2 TexCoords;

uniform sampler2D colorTex;
uniform sampler2D cocTex;

out vec4 FragColor;

uniform sampler2D screenTexture;
const float offset = 1.0 / 300.0;  

void main()
{
    //float offset = texture(cocTex, TexCoords).r/300;
    float str1 = texture(cocTex, TexCoords).r;
    float str2 = texture(cocTex, TexCoords).g;

    float str = str1 > str2 ? str1 : str2;

    //vec2 offsets[9] = vec2[](
    //    vec2(-offset,  offset), // top-left
    //    vec2( 0.0f,    offset), // top-center
    //    vec2( offset,  offset), // top-right
    //    vec2(-offset,  0.0f),   // center-left
    //    vec2( 0.0f,    0.0f),   // center-center
    //    vec2( offset,  0.0f),   // center-right
    //    vec2(-offset, -offset), // bottom-left
    //    vec2( 0.0f,   -offset), // bottom-center
    //    vec2( offset, -offset)  // bottom-right    
    //);

    vec2 offsets[25] = vec2[](
        vec2(-offset*2, offset*2), // top-left
        vec2(-offset,   offset*2),
        vec2(0.0f,      offset*2),
        vec2(offset,    offset*2),
        vec2(offset*2,  offset*2), // end top row

        vec2(-offset*2, offset), // top-left
        vec2(-offset,   offset),
        vec2(0.0f,      offset),
        vec2(offset,    offset),
        vec2(offset*2,  offset), // end second row

        vec2(-offset*2, 0.0f), // top-left
        vec2(-offset,   0.0f),
        vec2(0.0f,      0.0f),
        vec2(offset,    0.0f),
        vec2(offset*2,  0.0f), // end center row

        vec2(-offset*2, -offset), // top-left
        vec2(-offset,   -offset),
        vec2(0.0f,      -offset),
        vec2(offset,    -offset),
        vec2(offset*2,  -offset), // end 4th row

        vec2(-offset*2, -offset*2), // top-left
        vec2(-offset,   -offset*2),
        vec2(0.0f,      -offset*2),
        vec2(offset,    -offset*2),
        vec2(offset*2,  -offset*2) // end last row 
    );

    //float kernel[9] = float[](
    //    1.0 / 16, 2.0 / 16, 1.0 / 16,
    //    2.0 / 16, 4.0 / 16, 2.0 / 16,
    //    1.0 / 16, 2.0 / 16, 1.0 / 16  
    //);

    float kernel[25] = float[](
        0.0037f, 0.0146f, 0.0256f, 0.0146f, 0.0037f,
        0.0146f, 0.0586f, 0.0952f, 0.0586f, 0.0146f,
        0.0256f, 0.0952f, 0.1684f, 0.0952f, 0.0256f,
        0.0146f, 0.0586f, 0.0952f, 0.0586f, 0.0146f,
        0.0037f, 0.0146f, 0.0256f, 0.0146f, 0.0037f
    );

    //float kernel[9] = float[](
    //    1, 1, 1,
    //    1, -8, 1,
    //    1, 1, 1
    //);

    
    // fill some of color with the blurred color
    vec3 sampleTex[25];
    for(int i = 0; i < 25; i++)
    {
        sampleTex[i] = vec3(texture(colorTex, TexCoords.xy + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 25; i++)
        col += sampleTex[i] * kernel[i] * str * 0.8;
    
    // fill the rest of the color with the unblurred color
    col += vec3(texture(colorTex, TexCoords.xy)) * (1-str);

    FragColor = vec4(col[0],col[1],col[2], 1.0);
}  


