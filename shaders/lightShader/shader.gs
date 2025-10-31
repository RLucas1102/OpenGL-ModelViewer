#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    mat4 projection;
    vec3 fragPos;
    vec3 lightPos;
} gs_in[];

out vec3 normal;
out vec3 gsFragPos;
out vec3 gsLightPos;
out vec3 initialColor;

void main() {

    vec3 a = vec3(gl_in[1].gl_Position) - vec3(gl_in[0].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[0].gl_Position);
    normal = normalize(cross(a, b));

    for(int i = 0; i < 3; i++) {
        initialColor = normal;
        gsFragPos = gs_in[i].fragPos;
        gsLightPos = gs_in[i].lightPos;
        gl_Position = gs_in[0].projection * gl_in[i].gl_Position;
        EmitVertex();
    }

    EndPrimitive();

}