#version 330 core

layout(location = 0) in vec4 aVertex; // x y u v packed

out vec2 TexCoord;

uniform mat4 uProjection;

void main() {
    gl_Position = uProjection * vec4(aVertex.xy, 0.0, 1.0);
    TexCoord    = aVertex.zw;
}
