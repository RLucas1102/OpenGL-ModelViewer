#version 330 core
layout (std140) uniform pLight {
    vec4 plightAmbient;
    vec4 plightDiffuse;
    vec4 plightSpecular;
    vec4 plightPos;
    float constant;
    float linear;
    float quadratic;
};

layout (std140) uniform DirLight {
    vec4 dirLightDir;
    vec4 dirLightAmb;
    vec4 dirLightDif;
    vec4 dirLightSpc;
};

layout (std140) uniform Material {
    vec4 diffuseIn;
    vec4 specularIn;
    float shininessIn;
    int toonLevels;
    int dotTiling;
};

out vec4 fragColor;

in vec3 normal;
in vec3 fragPos;
in mat4 viewMat;

vec3 circle(vec3 _fragPos, float _radius);
vec3 tile(vec3 _fragPos, float _zoom);
vec3 CalcPLight();
vec3 CalcDirLight();

float toonScale = 1.0f / toonLevels;

void main() {

    vec3 result = vec3(0.0f, 0.0f, 0.0f);
    //result += CalcDirLight();
    result += CalcPLight();
    fragColor = vec4(result, 1.0f);

}

vec3 circle(vec3 _fragPos, float _radius) {
    vec2 pos = vec2(0.5) - _fragPos.xy;
    float edge0 = 1.0 - _radius;
    float edge1 = 1.0 - _radius + _radius * 0.2;
    float value = 1.0 - dot(pos, pos) * 3.14;
    return smoothstep(edge0 * vec3(1.0), edge1 * vec3(1.0), value * (1.0 - vec3(diffuseIn)));
}

vec3 tile(vec3 _fragPos, float _zoom) {
    _fragPos *=_zoom;
    return fract(_fragPos);
}

vec3 CalcDirLight() {

    vec3 st = fragPos;
    st = tile(st, dotTiling);

    // Create a light factor for global illumination
    vec3 ambient = vec3(dirLightAmb);

    // Create a light factor for diffuse light
    // First normalize all relevant vectors and find direction from frag to light
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-vec3(viewMat * vec4(vec3(dirLightDir), 1.0f)));

    // Calculate diffuse factor
    float diff = max(dot(norm, lightDir), 0.0); // If angle is negative, make 0.0
    diff = ceil(diff * toonLevels) * toonScale;

    vec3 diffuse = vec3(dirLightDif) * (vec3(1.0f - circle(st, min(max(1.0 - diff, 0.1), 0.9))));

    // Multiply lighting factors by object color and output
    vec3 result = ambient + diffuse;

    return result;
}

vec3 CalcPLight() {

    vec3 st = fragPos;
    st = tile(st, dotTiling);

    // Need light position in view space
    vec3 viewLightPos = vec3(viewMat * vec4(vec3(plightPos), 1.0f)); // Passing in vec4 caused an error when multiplying view matrix

    // Create a light factor for global illumination
    vec3 ambient = vec3(plightAmbient);

    // Create a light factor for diffuse light
    // First normalize all relevant vectors and find direction from frag to light
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(viewLightPos - fragPos);

    // Calculate diffuse factor
    float diff = max(dot(norm, lightDir), 0.0); // If angle is negative, make 0.0
    diff = ceil(diff * toonLevels) * toonScale;

    vec3 diffuse = vec3(plightDiffuse) * (vec3(1.0f - circle(st, min(max(1.0 - diff, 0.1), 0.9))));

    // Multiply lighting factors by object color and output
    vec3 result = ambient + diffuse;

    return result;

}