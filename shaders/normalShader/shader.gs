#version 330 core

layout (triangles) in; // Take in primitives as triangles
layout (line_strip, max_vertices = 2) out; // Output primitives as lines using line_strip

in VS_OUT{
    mat4 projection;
} gs_in[];

void main() {

    vec3 point1 = vec3(gl_in[0].gl_Position);
    vec3 point2 = vec3(gl_in[1].gl_Position);
    vec3 point3 = vec3(gl_in[2].gl_Position);

    vec3 a = point2 - point1;
    vec3 b = point3 - point1;
    vec3 normal = normalize(cross(a,b));

    vec3 center;
    center.x = (point1.x + point2.x + point3.x) / 3.0f;
    center.y = (point1.y + point2.y + point3.y) / 3.0f;
    center.z = (point1.z + point2.z + point3.z) / 3.0f;

    float magnitude = 0.2f;

    gl_Position = gs_in[0].projection * vec4(center, 1.0f);
    EmitVertex();

    gl_Position = gs_in[0].projection * (vec4(center, 1.0f) + (vec4(normal, 0.0f) * magnitude));
    EmitVertex();
    
    EndPrimitive();
}