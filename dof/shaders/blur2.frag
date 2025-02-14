#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D image;
uniform sampler2D cocTex;
uniform float blur_radius; // Controls the blurriness
uniform vec2 resolution;

void main() {
    vec4 blur_color_1 = vec4(0.0);
    vec4 blur_color_2 = vec4(0.0);
    vec4 blur_color_3 = vec4(0.0);
    float weightSum = 0.0;

    float str1 = texture(cocTex, TexCoords).r;
    float str2 = texture(cocTex, TexCoords).g;

    float str = str1 > str2 ? str1 : str2;

    for (int x = -4; x <= 4; x++) {
        for (int y = -4; y <= 4; y++) {
            float weight = exp(-(x*x + y*y) / (2.0 * blur_radius * blur_radius));
            weightSum += weight;
            blur_color_1 += texture(image, TexCoords + vec2(x, y) / (resolution)) * weight;
        }
    }

    for (int x = -4; x <= 4; x++) {
        for (int y = -4; y <= 4; y++) {
            float weight = exp(-(x*x + y*y) / (2.0 * blur_radius * blur_radius));
            blur_color_2 += texture(image, TexCoords + vec2(x, y) / (resolution/2)) * weight;
        }
    }

    for (int x = -4; x <= 4; x++) {
        for (int y = -4; y <= 4; y++) {
            float weight = exp(-(x*x + y*y) / (2.0 * blur_radius * blur_radius));
            blur_color_3 += texture(image, TexCoords + vec2(x, y) / (resolution/4)) * weight;
        }
    }

    //float str_weak = str > 0.7f ? 0.7f : str; 
    //float str_strong = str-0.7f > 0 ? str-0.7f : 0.0f; 
    
    //str = str_weak + str_mid + str_strong

    float str_weak = str > 0.4f ? 0.4f : str;

    float str_strong = str-0.7f > 0 ? str-0.7f : 0.0f; 
    
    float str_mid = str - str_weak - str_strong > 0 ? str - str_weak - str_strong : 0.0f;


    FragColor = (blur_color_1 / weightSum) * str_weak 
                + (blur_color_2 / weightSum) * str_mid 
                + (blur_color_3 / weightSum) * str_strong 
                + vec4(texture(image, TexCoords.xy)) * (1-str); // Normalize
}


