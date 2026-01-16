#version 460 core
out vec4 FragColor;

in vec3 vNormal;
in vec3 vWorldPos;
in vec2 vTexCoord;

uniform vec3 uColor; 
uniform vec3 uViewPos;
uniform sampler2D uDiffuseMap; // Optional: for future texture support
uniform bool uUseTexture;

struct Light {
    vec3 color;
    float intensity;
    vec3 position;
    float range;
    vec3 direction;
    float spotAngle; 
    int type;
    float shadowBias;
    int castShadows;
    float padding;
};

layout(std140, binding = 1) uniform LightBuffer {
    Light lights[256];
    int numLights;
};

void main() {
    vec3 normal = normalize(vNormal);
    vec3 viewDir = normalize(uViewPos - vWorldPos);
    
    // Determine base color (Texture or Solid Color)
    vec3 baseColor = uColor;
    if(uUseTexture) {
        baseColor *= texture(uDiffuseMap, vTexCoord).rgb;
    }

    vec3 lighting = vec3(0.0);

    for(int i = 0; i < numLights; i++) {
        Light light = lights[i];
        vec3 L;
        float attenuation = 1.0;

        // 1. Light Direction & Distance
        if (light.type == 0) { // Directional
            L = normalize(-light.direction);
        } else {
            L = normalize(light.position - vWorldPos);
            float distance = length(light.position - vWorldPos);
            
            // Physically-based Inverse Square Falloff
            // We use the light's 'range' as a hard cutoff
            float d = distance / light.range;
            attenuation = clamp(1.0 - d*d*d*d, 0.0, 1.0) / (distance * distance + 1.0);
            attenuation *= light.intensity;
        }

        // 2. Spot Light Masking
        if (light.type == 2) {
            float theta = dot(L, normalize(-light.direction));
            // Softening the edge of the spot (Penumbra)
            float epsilon = 0.05; 
            attenuation *= smoothstep(light.spotAngle, light.spotAngle + epsilon, theta);
        }

        // 3. Diffuse (Lambert)
        float diff = max(dot(normal, L), 0.0);
        
        // 4. Specular (Blinn-Phong)
        vec3 halfwayDir = normalize(L + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0); // 64 = sharper highlights

        lighting += (diff + spec * 0.5) * light.color * attenuation;
    }

    // Combine and apply Tone Mapping / Gamma Correction
    vec3 ambient = baseColor * 0.03;
    vec3 result = ambient + (baseColor * lighting);

    // Simple Reinhard Tone Mapping (prevents "blown out" white spots)
    result = result / (result + vec3(1.0));
    // Gamma Correction
    result = pow(result, vec3(1.0 / 2.2));

    FragColor = vec4(result, 1.0);
}
