#version 330 core

in vec3 normal;
in vec3 gsFragPos;
in vec3 gsLightPos;

in vec3 initialColor;
out vec4 FragColor; // Single output variable is the fragment color

void main() {

    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);  // Color of our light
    vec3 objectColor = vec3(1.0f, 0.0f, 0.0f); // Color of the object

    float ambientFactor = 0.1f;
    vec3 ambient = ambientFactor * lightColor;

    vec3 lightDir = normalize(gsLightPos - gsFragPos);
    float diffuseFactor = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = diffuseFactor * lightColor;

    vec3 viewDir = normalize(-gsFragPos); // Viewer is always as vec3(0,0,0) if we are doing lighting based on model-view space
    vec3 reflectDir = reflect(-lightDir, normal);
    float shininess = 0.5f;
    float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    vec3 specular = specularFactor * shininess * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0f);
}