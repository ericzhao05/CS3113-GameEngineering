uniform sampler2D diffuse;
varying vec2 texCoordVar;
uniform int invert_colors;

void main() {
    vec4 color = texture2D(diffuse, texCoordVar);
    
    if (invert_colors == 1) {
        gl_FragColor = vec4(1.0 - color.r, 1.0 - color.g, 1.0 - color.b, color.a);
    } else {
        gl_FragColor = color;
    }
}
