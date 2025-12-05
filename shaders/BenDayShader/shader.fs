#version 330 core

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
float mySmoothStep(float edge0, float edge1, float value);
vec3 tile(vec3 _fragPos, float _zoom);

void main() {

    float intensity;
    vec3 color;
    vec3 norm = normalize(normal);

    intensity = dot(vec3(0.0f, -1.0f, 1.0f), norm);

    vec3 st = fragPos;
    st = tile(st, dotTiling);

    color = vec3(1.0f - circle(st, min(max(1.0 - intensity, 0.1), 0.9)));
    fragColor = vec4(color, 1.0f);

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

float mySmoothStep(float edge0, float edge1, float value) {
    float t;
    t = clamp((value - edge0) / (edge1 - edge0), 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);
}