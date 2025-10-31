#version 330 core

in VS_OUT {

    vec3 instanceColor;

} vs_in;

out vec4 FragColor;

void main() {
    FragColor = vec4(vs_in.instanceColor, 1.0f);
}