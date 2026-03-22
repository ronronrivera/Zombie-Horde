#version 330 core

in  vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;

void main() {
    vec4 color = texture(uTexture, TexCoord);
    if (color.a < 0.05) discard;
    FragColor = color;
}
