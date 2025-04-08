#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D ourTexture;
uniform int isCharging; // 0 for IDLE, 1 for CHARGING

void main()
{
    vec4 texColor = texture(ourTexture, TexCoords);
    
    if (isCharging == 1) {
        // CHARGING state - Apply spotlight effect
        float distance = length(TexCoords - vec2(0.5, 0.5));
        float spotlight = 1.0 - smoothstep(0.0, 0.5, distance);
        
        // If it's a background pixel (assuming alpha < 1.0 means background)
        if (texColor.a < 1.0) {
            // Turn background black when charging
            color = vec4(0.0, 0.0, 0.0, texColor.a);
        } else {
            // Apply spotlight effect to non-background elements
            color = texColor * spotlight;
        }
    } else {
        // IDLE state - Regular textured rendering
        color = texColor;
    }
} 