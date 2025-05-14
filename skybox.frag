#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube daySkybox;
uniform samplerCube nightSkybox;
uniform float blendFactor; //0.0 = day, 1.0 = night

void main()
{
    vec4 dayColor = texture(daySkybox, TexCoords);
    vec4 nightColor = texture(nightSkybox, TexCoords);
    FragColor = mix(dayColor, nightColor, blendFactor); //use mix function to blend between day and night
}