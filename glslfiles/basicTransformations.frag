#version 400
in  vec2 ex_TexCoord; //texture coord arriving from the vertex
in  vec3 ex_Normal;  //normal arriving from the vertex
out vec4 out_Color;   //colour for the pixel
in vec3 ex_LightDir;  //light direction arriving from the vertex
in vec3 ex_PositionEye;
uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;
uniform vec4 material_ambient;
uniform vec4 material_diffuse;
uniform vec4 material_specular;
uniform float material_shininess;
uniform sampler2D DiffuseMap;
uniform float fog_maxdist;
uniform float fog_mindist;
uniform vec4 fog_color;

//pointlight properties
const int MAX_POINT_LIGHTS = 8;
uniform int num_point_lights;
uniform struct PointLight {
    vec3 position; 
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float constant; 
    float linear;
    float quadratic;
} pointLights[MAX_POINT_LIGHTS];

//spotlight properties
const int MAX_SPOT_LIGHTS = 8;
uniform int num_spot_lights;
uniform struct SpotLight {
    vec3 position;     
    vec3 direction;    
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float cutOff;      
    float constant;    
    float linear;
    float quadratic;
} spotLights[MAX_SPOT_LIGHTS];

vec4 calculatePointLight(PointLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - ex_PositionEye);
    
    //calculate diffuse
    float NdotL = max(dot(normal, lightDir), 0.0);
    
    //calculate specular
    vec3 r = reflect(-lightDir, normal);
    float RdotV = max(0.0, dot(r, viewDir));
    
    //attenuation
    float distance = length(light.position - ex_PositionEye);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    //combine results
    vec4 ambient = light.ambient * material_ambient;
    vec4 diffuse = vec4(0.0);
    if(NdotL > 0.0) {
        diffuse = light.diffuse * material_diffuse * NdotL;
    }
    vec4 specular = light.specular * material_specular * pow(RdotV, material_shininess);
    
    return (ambient + diffuse + specular) * attenuation;
}

vec4 calculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - ex_PositionEye);
    
    //diffuse
    float NdotL = max(dot(normal, lightDir), 0.0);
    
    //specular
    vec3 r = reflect(-lightDir, normal);
    float RdotV = max(0.0, dot(r, viewDir));
    
    //calculate spotlight effect
    float theta = dot(lightDir, normalize(-light.direction));
    //float intensity = clamp((theta) / light.cutOff, 0.0, 1.0);
    float intensity = 0.0;
    if(theta > light.cutOff) {
        intensity = 1.0;
    }
    
    //attenuation
    float distance = length(light.position - ex_PositionEye);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    //combine results
    vec4 ambient = light.ambient * material_ambient;
    vec4 diffuse = vec4(0.0);
    if(NdotL > 0.0) {
        diffuse = light.diffuse * material_diffuse * NdotL;
    }
    vec4 specular = light.specular * material_specular * pow(RdotV, material_shininess);
    
    // Apply spotlight intensity and attenuation
    diffuse *= intensity;
    specular *= intensity;
    
    return (ambient + diffuse + specular) * attenuation;
}

void main(void)
{
	//out_Color = texture(DiffuseMap, ex_TexCoord); //show texture values
	//out_Color = vec4(ex_Normal,1.0); //Show normals
	//out_Color = vec4(ex_TexCoord,0.0,1.0); //show texture coords
	//Calculate lighting
	vec3 n, L;
	vec4 directionalColor;
	float NdotL;
	
	n = normalize(ex_Normal);
	L = normalize(ex_LightDir);
	vec3 v = normalize(-ex_PositionEye);
	vec3 r = reflect(-L, n);
	
	float RdotV = max(0.0, dot(r, v));
	NdotL = max(dot(n, L),0.0);
	directionalColor = light_ambient * material_ambient;
	
	if(NdotL > 0.0) 
	{
		directionalColor += (light_diffuse * material_diffuse * NdotL);
	}
	directionalColor += material_specular * light_specular * pow(RdotV, material_shininess);
	
	// Calculate point light contribution
	vec4 pointLightColor = vec4(0.0);
	for(int i = 0; i < num_point_lights && i < MAX_POINT_LIGHTS; i++) {
		pointLightColor += calculatePointLight(pointLights[i], n, v);
	}
	
	// Calculate spotlight contribution
	vec4 spotLightColor = vec4(0.0);
	for(int i = 0; i < num_spot_lights && i < MAX_SPOT_LIGHTS; i++) {
		spotLightColor += calculateSpotLight(spotLights[i], n, v);
	}
	
    // Sample the texture color
    vec4 textureColor = texture(DiffuseMap, ex_TexCoord);
    
    // Combine all lighting with texture color
    vec4 finalColor = (directionalColor + pointLightColor + spotLightColor) * textureColor;
    
    // Calculate fog
    float dist = length(ex_PositionEye);
    float fog_factor = (fog_maxdist - dist) / (fog_maxdist - fog_mindist);
    fog_factor = clamp(fog_factor, 0.0, 1.0); //keep the factor between 0 and 1
    
    // Combine fog with the final color
    out_Color = mix(fog_color, finalColor, fog_factor);
    //out_Color = vec4(1.0, 0.0, 0.0, 1.0); // Uncomment to show red color
	//out_Color = mix(fog_color, directionalColor, fog_factor); //tex color
	//out_Color = color;  //show just lighting
	//out_Color = texture(DiffuseMap, ex_TexCoord); //show texture only
	//vec4 lighting = directionalColor + pointLightColor + spotLightColor;
    //out_Color = directionalColor * texture(DiffuseMap, ex_TexCoord); //show texture and lighting
}