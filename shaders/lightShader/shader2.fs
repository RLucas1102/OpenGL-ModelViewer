#version 330 core

in vec3 initialColor;

out vec4 FragColor;

void main() {

    FragColor = vec4(initialColor, 1.0f);

}