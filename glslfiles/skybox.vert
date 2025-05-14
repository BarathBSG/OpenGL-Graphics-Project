#version 330 core
layout (location = 0) in vec3 aPos; // Vertex position

out vec3 TexCoords; // Pass texture coordinates to the fragment shader

uniform mat4 projection; // Projection matrix
uniform mat4 view;       // View matrix

void main() {
    TexCoords = aPos; // Pass the vertex position as texture coordinates
    vec4 pos = projection * view * vec4(aPos, 1.0); // Apply projection and view transformations
    gl_Position = pos.xyww; // Set depth to the far plane