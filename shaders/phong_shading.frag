/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: simple_render.frag
Purpose: fragment shader for 3d object including light calculation
Language: glsl
Platform: Microsoft Visual Studio2019, Windows
Project:  Hyosang Jung_CS300_1
Author: Hyosang Jung, hyosang.jung, 055587
Creation date: 2022 - 09 - 12
End Header --------------------------------------------------------*/
#version 430 core

layout(location=1) in vec3 NRM;

in vec3 FragPos; 
in vec2 TexCoords;

out vec4 outColor;

struct Light 
{
    int type;                               //0
    vec3 ambient;                   //16
    vec3 diffuse;                     //32
    vec3 specular;                    //48
    vec3 position;                 //64
    vec3 direction;               //80
};
//size = 96

layout (std140, binding = 0) uniform LightProperties 
{
     Light light[16];     // 96 *  16 = 1536
     int activeLight;   // 16
}lightProperties;
//size = 1552

struct Material
{
    vec3 Kd;
    vec3 Ks;
    vec3 Ka;
    float ns;
};

uniform sampler2D material_Diffuse;
uniform sampler2D material_Specular;
uniform Material default_material;

uniform vec3 material_Ambient;
uniform vec3 viewPos = vec3(0,0,5);
uniform bool UseTexture;
uniform vec3 planeColor;

vec3 norm = normalize(NRM);
vec3 viewDir = normalize(viewPos - FragPos);

Material calculate_Material()
{
     vec3 kd = vec3(texture(material_Diffuse, TexCoords));
     vec3 ks = vec3(texture(material_Specular, TexCoords));
     vec3 ka = material_Ambient;
     float ns = ks.r;
     Material tmp = Material(kd,ks,ka,ns);
     return tmp;
}

vec3 CalcDirLight(Light light)
{
    Material material;

    if(UseTexture)
        material = calculate_Material();
    else
    {
       material = Material(vec3(1),vec3(1),vec3(1),1.f);
    }
    vec3 lightDir = normalize(light.position);  
    // diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, norm); 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.ns);
    // combine results

	 vec3 I_a = light.ambient *material.Ka;
     vec3 I_d = light.diffuse * diff*material.Kd;
     vec3 I_s =light.specular * spec * material.Ks;
    return (I_a + I_d  + I_s);
} 

void main(void)
{
     vec3 result = vec3(0);
    
    for(int i = 0; i < lightProperties.activeLight; i++)
        result += (CalcDirLight(lightProperties.light[i])); 

	 outColor =  vec4( result, 1.0);   
}