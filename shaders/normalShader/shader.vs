#version 330 core

layout (location = 0) in vec3 aPos;    // Position of vertices is at attribute location 0
layout (location = 1) in vec3 aNormal; // Normal of vertices is at attribute location 1

layout (std140) uniform matrices {
                 
    mat4 view;
    mat4 projection;
    mat4 model;
    
};

out vec3 normal;

void main() {

    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    normal = mat3(transpose(inverse(model))) * aNormal;
    
}

