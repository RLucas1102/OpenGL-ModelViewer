#version 330 core

layout (location = 0) in vec3 aPos; // Position of vertices is at attribute location 0

layout (std140) uniform matrices {

    mat4 model;                    
    mat4 view;
    mat4 projection;
    
};

out VS_OUT {
    mat4 projection;
} vs_out;

void main() {

    gl_Position = view * model * vec4(aPos, 1.0f);
    vs_out.projection = projection;

}