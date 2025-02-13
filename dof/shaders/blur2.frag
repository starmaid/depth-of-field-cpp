#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D image;
uniform float blur_radius; // Controls the blurriness
uniform vec2 resolution;

void main() {
    vec4 color = vec4(0.0);
    float weightSum = 0.0;

    for (int x = -4; x <= 4; x++) {
        for (int y = -4; y <= 4; y++) {
            float weight = exp(-(x*x + y*y) / (2.0 * blur_radius * blur_radius));
            weightSum += weight;
            color += texture(image, TexCoords + vec2(x, y) / resolution) * weight;
        }
    }
    FragColor = color / weightSum; // Normalize
}


