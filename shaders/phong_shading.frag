/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: phong_shading.frag
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

uniform sampler2D DiffuseMap;
uniform sampler2D SpecularMap;

uniform vec3 viewPos;
uniform bool UseTexture;
uniform bool UseUVFromGPU;
uniform bool UsePosTOuv;
uniform int  UVprojectionType;

uniform vec3 material_Ambient;
uniform vec3 material_Diffuse;
uniform vec3 material_Specular;
uniform vec3 material_Emissive;

vec3 norm = normalize(NRM);
vec3 viewDir = normalize(viewPos - FragPos);

struct Light 
{
    int type;                                       
    vec3 ambient;                             
    vec3 diffuse;                                
    vec3 specular;                             
    vec3 position;                               
    vec3 direction;                             
    vec3 Inner_Outer_Falloff;                  //size = 112
}; 

layout (std140, binding = 0) uniform LightProperties 
{
     Light light[16];                  // 0
     vec3 coefficients;            // 112 *  16 = 1792
     vec3 fogColor;                 // 1808
     vec3 GlobalAmbient;    // 1824
     int activeLight;                // 1840
     float fog_far;
     float fog_near;                 //size = 1856
}lightProperties;


struct Material
{
    vec3 Kd;
    vec3 Ks;
    vec3 Ka;
    float ns;
};

vec2 calcUV_cylindrical()
{
    vec3 v;
    vec2 uv;
    if (UsePosTOuv)
        v = FragPos;
    else
        v = norm;
    float ux = degrees(atan(v.z/ v.x));
    ux += 180.f;
    float uy = (v.y+1.f) / (2.f);
    uv.x = ux / 360.f;
    uv.y = uy;
    return uv;
}
vec2 calcUV_spherical()
{
    vec3 v;
    vec2 uv;
    if (UsePosTOuv)
        v = FragPos;
    else
        v = norm;
    float  r = sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
    float theta = degrees(atan(v.z/v.x));
    theta += 180.f;
    float phi = 180.f - (acos(v.y / r))* 180.f / acos(-1) ;
    uv.x = theta / 360.f;
    uv.y = phi / 180.f;

    return uv;
}
vec2 calcUV_cube()
{
    vec3 absVec;
    vec3 v;

    vec2 uv;
    if (UsePosTOuv)
    {
        absVec = abs(FragPos);
        v = FragPos;
    }
    else
    {
        absVec = abs(norm);
        v = norm;
    }   
    if (absVec.x >= absVec.y && absVec.x >= absVec.z)
    {
        (v.x < 0.0) ? (uv.x = v.z) : (uv.x = -v.z);
        uv.y = v.y;
    }
    else if (absVec.y >= absVec.x && absVec.y >= absVec.z)
    {
        (v.y < 0.0) ? (uv.x = v.x) : (uv.x = -v.x);
        uv.y = v.z;
    }
    else if (absVec.z >= absVec.x && absVec.z >= absVec.y)
    {
        (v.z < 0.0) ? (uv.x = v.x) : (uv.x = -v.x);
        uv.y = v.y;
    }
    uv = (uv+1.f)/2.f;
    return uv;
}


vec2 CalcUV(int mode)
{
    vec2 UV;
    if(mode == 0)
        UV = calcUV_cylindrical();
    else if(mode ==1)
        UV = calcUV_spherical();
    else
        UV = calcUV_cube();

    return UV;
}

Material calculate_Material()
{
     vec2 UV;
     if(UseUVFromGPU)
        UV = CalcUV(UVprojectionType);
     else
        UV = TexCoords;
     vec3 kd = vec3(texture(DiffuseMap, UV));
     vec3 ks = vec3(texture(SpecularMap, UV));
     vec3 ka = material_Ambient;
     float ns = ks.r * ks.r;
     Material tmp = Material(kd,ks,ka,ns);
     return tmp;
}

vec3 CalcPointLight(Light light)
{
    Material material;
    vec3 lightDir = normalize(light.position - FragPos);   
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 reflectDir = 2*(dot(norm, lightDir))*norm - lightDir; 
    float distance =  length(light.position - FragPos);
    float att1 = lightProperties.coefficients.x;
    float att2 = lightProperties.coefficients.y * distance;
    float att3 = lightProperties.coefficients.z * (distance * distance);
    float att = 1.f/(att1 + att2 + att3);

    vec3 I_a;
    vec3 I_d;
    vec3 I_s;
    if(UseTexture)
    {
        material = calculate_Material();
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.ns);
	    I_a = light.ambient * material_Ambient * material.Ka ;
        I_d = light.diffuse * diff * material_Diffuse*material.Kd;
        I_s =light.specular * spec * material_Specular* material.Ks;
        return att * (I_a + I_d  + I_s);
    }
    else
    {
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        I_a = light.ambient * 0.1;
        I_d = light.diffuse * diff;
        I_s =light.specular * spec;
        return att * (I_a + I_d  + I_s);
    }
} 

vec3 CalcDirectionLight(Light light)
{
    Material material;
    vec3 lightDir = normalize(light.position);   
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 reflectDir = 2*(dot(norm, lightDir))*norm - lightDir; 

    vec3 I_a;
    vec3 I_d;
    vec3 I_s;
    if(UseTexture)
    {
        material = calculate_Material();
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.ns);
        I_a = light.ambient *material_Ambient * material.Ka ;
        I_d = light.diffuse * diff*material_Diffuse*material.Kd;
        I_s = light.specular * spec * material_Specular* material.Ks;
        return I_a + I_d  + I_s;
    }
    else
    {
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);

        I_a = light.ambient * 0.1;
        I_d = light.diffuse * diff;
        I_s = light.specular * spec;
        return I_a + I_d  + I_s;
    }
} 

vec3 CalcSpotLight(Light light)
{
    Material material;
    vec3 lightDir = normalize(light.position);   
    vec3 direction = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 reflectDir = 2*(dot(norm, lightDir))*norm - lightDir; 
    float distance =  length(light.position - FragPos);
    float att1 = lightProperties.coefficients.x;
    float att2 = lightProperties.coefficients.y * distance;
    float att3 = lightProperties.coefficients.z * (distance * distance);
    float att = 1.f/(att1 + att2 + att3);
    float spot;
    if(dot(lightDir,direction) < cos(radians(light.Inner_Outer_Falloff.y)))
        spot = 0;
    else if(dot(lightDir,direction) > cos(radians(light.Inner_Outer_Falloff.x)))
        spot = 1;
    else
    {
        float alpha = dot(lightDir,direction) - cos(radians(light.Inner_Outer_Falloff.y));
        float theta = cos(radians(light.Inner_Outer_Falloff.x)) - cos(radians(light.Inner_Outer_Falloff.y));
        spot = pow(alpha / theta , light.Inner_Outer_Falloff.z);
    }
    vec3 I_a;
    vec3 I_d;
    vec3 I_s;
    if(UseTexture)
    {
        material = calculate_Material();
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.ns);
        I_a = light.ambient *material_Ambient * material.Ka ;
        I_d = light.diffuse * diff*material_Diffuse*material.Kd;
        I_s = light.specular * spec * material_Specular* material.Ks;
        return att *spot* (I_a + I_d  + I_s);
    }
    else
    {
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        I_a = light.ambient *1;
        I_d = light.diffuse * diff;
        I_s =light.specular * spec;
        return att * spot*(I_a + I_d  + I_s);
    }
} 

void main(void)
{
    vec3 result = vec3(0);
    
    for(int i = 0; i < lightProperties.activeLight; i++)
    {
        if(lightProperties.light[i].type == 0)
            result += (CalcPointLight(lightProperties.light[i])); 
        else if(lightProperties.light[i].type == 1)
            result += (CalcDirectionLight(lightProperties.light[i])); 
        else if(lightProperties.light[i].type == 2)
            result += (CalcSpotLight(lightProperties.light[i]));  
    }
    if(UseTexture)
    {
        Material material = calculate_Material();       
        result = material_Emissive + (lightProperties.GlobalAmbient*material.Ka) + result;
    }
    else
        result = material_Emissive + (lightProperties.GlobalAmbient*0.2) + result;
        
    float S = (lightProperties.fog_far - abs(viewPos.z - FragPos.z))/(lightProperties.fog_far - lightProperties.fog_near);
    vec3 final = (S*result) + (1-S)*lightProperties.fogColor;

	 outColor =  vec4( final, 1.0);   
}