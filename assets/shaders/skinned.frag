#version 330 core

in  vec2 TexCoord;
in  vec3 FragPos;
in  vec3 Normal;
out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec3      uViewPos;
uniform vec3      uAmbient;

struct SpotLight {
    vec3  position;
    vec3  direction;
    vec3  color;
    float intensity;
    float cutoffInner;
    float cutoffOuter;
    float constant;
    float linear;
    float quadratic;
};
uniform SpotLight uLight;

void main() {
    vec4 color = texture(uTexture, TexCoord);
    if (color.a < 0.05) discard;

    vec3 albedo = color.rgb;
    vec3 norm   = normalize(Normal);

    vec3 lightVec = uLight.position - FragPos;
    float dist    = length(lightVec);
    vec3 lightDir = normalize(lightVec);

    float theta        = dot(lightDir, normalize(-uLight.direction));
    float coneEpsilon  = uLight.cutoffInner - uLight.cutoffOuter;
    float coneStrength = clamp((theta - uLight.cutoffOuter) / coneEpsilon, 0.0, 1.0);

    float atten = 1.0 / (uLight.constant
                       + uLight.linear    * dist
                       + uLight.quadratic * dist * dist);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLight.color * uLight.intensity;

    vec3 viewDir    = normalize(uViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), 24.0);
    vec3 specular   = spec * uLight.color * 0.2;

    vec3 ambient  = uAmbient * albedo;
    vec3 lighting = (diffuse + specular) * atten * coneStrength;
    vec3 result   = ambient + lighting * albedo;

    FragColor = vec4(result, color.a);
}
