#version 330 core

in  vec2      TexCoord;
out vec4      FragColor;

uniform sampler2D uText;
uniform vec3      uTextColor;

void main() {
    // GL_RED texture — red channel is the alpha mask
    float alpha = texture(uText, TexCoord).r;
    if (alpha < 0.05) discard;
    FragColor = vec4(uTextColor, alpha);
}
