#version 330 core
in vec3 TexCoords; // Texture coordinates from the vertex shader

out vec4 FragColor; // Final fragment color

uniform samplerCube skybox; // Cubemap texture
uniform float brightness;   // Brightness control

void main() {
    vec4 color = texture(skybox, TexCoords); // Sample the cubemap texture
    FragColor = vec4(color.rgb * brightness, color.a); // Apply brightness to the color
}