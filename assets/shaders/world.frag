#version 420 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec3      uViewPos;   // camera position
uniform vec3      uAmbient;   // global ambient (very dark)

struct SpotLight {
    vec3  position;
    vec3  direction;
    vec3  color;
    float intensity;
    float cutoffInner;   // cos of inner angle
    float cutoffOuter;   // cos of outer angle
    float constant;
    float linear;
    float quadratic;
};
uniform SpotLight uLight;

struct PointLight {
    vec3  position;
    vec3  color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
};
uniform PointLight uMuzzleLight;

void main() {
    vec3  texColor  = texture(uTexture, TexCoord).rgb;
    vec3  norm      = normalize(Normal);

    // ── vector from fragment to light ───────────────────
    vec3  lightVec  = uLight.position - FragPos;
    float dist      = length(lightVec);
    vec3  lightDir  = normalize(lightVec);

    // ── spotlight cone check ─────────────────────────────
    float theta     = dot(lightDir, normalize(-uLight.direction));
    float epsilon   = uLight.cutoffInner - uLight.cutoffOuter;
    // smoothstep gives a soft edge instead of a hard cutoff
    float coneIntensity = clamp((theta - uLight.cutoffOuter) / epsilon,
                                 0.0, 1.0);

    // ── attenuation (light fades with distance) ──────────
    float atten = 1.0 / (uLight.constant
                       + uLight.linear    * dist
                       + uLight.quadratic * dist * dist);

    // ── diffuse ──────────────────────────────────────────
    float diff    = max(dot(norm, lightDir), 0.0);
    vec3  diffuse = diff * uLight.color * uLight.intensity;

    // ── specular (Phong) ─────────────────────────────────
    vec3  viewDir    = normalize(uViewPos - FragPos);
    vec3  reflectDir = reflect(-lightDir, norm);
    float spec       = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3  specular   = spec * uLight.color * 0.3;

    // ── combine ──────────────────────────────────────────
    vec3 ambient  = uAmbient * texColor;
    vec3 lighting = (diffuse + specular) * atten * coneIntensity;

    // ── muzzle flash point light ─────────────────────────
    vec3  muzzleVec  = uMuzzleLight.position - FragPos;
    float muzzleDist = length(muzzleVec);
    vec3  muzzleDir  = normalize(muzzleVec);

    float muzzleAtten = 1.0 / (uMuzzleLight.constant
                             + uMuzzleLight.linear    * muzzleDist
                             + uMuzzleLight.quadratic * muzzleDist * muzzleDist);

    float muzzleDiff   = max(dot(norm, muzzleDir), 0.0);
    vec3  muzzleDiffuse = muzzleDiff * uMuzzleLight.color * uMuzzleLight.intensity;
    lighting += muzzleDiffuse * muzzleAtten * texColor;

    vec3 result   = ambient + lighting * texColor;

    // ── horror atmosphere: slight desaturation in dark areas
    float brightness = dot(result, vec3(0.299, 0.587, 0.114));
    result = mix(result, vec3(brightness) * vec3(0.8, 0.85, 1.0),
                 clamp(1.0 - brightness * 8.0, 0.0, 0.4));

    FragColor = vec4(result, 1.0);
}
