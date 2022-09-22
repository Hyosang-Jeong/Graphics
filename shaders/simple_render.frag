#version 450 core

layout(location=1) in vec3 NRM;

out vec4 outColor;
in vec3 FragPos; 

vec3 lightColor = vec3(1,1,1);

//ambient calculation
const float ambientStrength = 0.1f;
vec3 ambient = ambientStrength*lightColor;

uniform vec3 lightPos;
vec3 norm = normalize(NRM);
vec3 lightDir = normalize(lightPos - FragPos);  
float diff = max(dot(norm, lightDir), 0.0);
vec3 diffuse = diff * lightColor;

void main(void)
{
		vec3 result = (ambient + diffuse) * vec3(1,1,1);
		outColor =  vec4(result,1);
}