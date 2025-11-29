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
};

in vec3 normal;
in vec3 fragPos;
in mat4 viewMat;

out vec4 fragColor;

vec3 CalcPLight(float shininess);
vec3 CalcDirLight(float shininess);

void main() {

    float shininess = shininessIn;

    vec3 result = vec3(0.0f, 0.0f, 0.0f);

    result += CalcDirLight(shininess);
    result += CalcPLight(shininess);

    fragColor = vec4(result, 1.0f);

}

vec3 CalcDirLight(float shininess) {

    // Create a light factor for global illumination
    vec3 ambient = vec3(dirLightAmb) * vec3(diffuseIn);

    // Create a light factor for diffuse light
    // First normalize all relevant vectors and find direction from frag to light
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-vec3(viewMat * vec4(vec3(dirLightDir), 1.0f)));

    // Calculate diffuse factor
    float diff = max(dot(norm, lightDir), 0.0); // If angle is negative, make 0.0

    vec3 diffuse = vec3(dirLightDif) * (diff * vec3(diffuseIn));

    // Create a light factor for specular light
    vec3 viewDir = normalize(-fragPos); // viewer is always at (0,0,0) in view space
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);

    vec3 specular = vec3(dirLightSpc) * (spec * vec3(specularIn));

    // Multiply lighting factors by object color and output
    vec3 result = ambient + diffuse + specular;

    return result;
}

vec3 CalcPLight(float shininess) {

    // Need light position in view space
    vec3 viewLightPos = vec3(viewMat * vec4(vec3(plightPos), 1.0f)); // Passing in vec4 caused an error when multiplying view matrix

    // Create a light factor for global illumination
    vec3 ambient = vec3(plightAmbient) * vec3(diffuseIn);

    // Create a light factor for diffuse light
    // First normalize all relevant vectors and find direction from frag to light
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(viewLightPos - fragPos);

    // Calculate diffuse factor
    float diff = max(dot(norm, lightDir), 0.0); // If angle is negative, make 0.0

    vec3 diffuse = vec3(plightDiffuse) * (diff * vec3(diffuseIn));

    // Create a light factor for specular light
    vec3 viewDir = normalize(-fragPos); // viewer is always at (0,0,0) in view space
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);

    vec3 specular = vec3(plightSpecular) * (spec * vec3(specularIn));

    // Find attenuation
    float distance = length(viewLightPos - fragPos);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    // Multiply lighting factors by object color and output
    vec3 result = ambient + diffuse + specular;

    return result;

}