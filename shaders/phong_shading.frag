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
#version 450 core

layout(location=1) in vec3 NRM;

in vec3 FragPos; 
in vec2 TexCoords;

out vec4 outColor;

uniform sampler2D DiffuseMap;
uniform sampler2D SpecularMap;
uniform sampler2D skybox_LEFT;
uniform sampler2D skybox_RIGHT;
uniform sampler2D skybox_BOTTOM;
uniform sampler2D skybox_TOP;
uniform sampler2D skybox_BACK;
uniform sampler2D skybox_FRONT;

uniform vec3 viewPos;
uniform bool UseTexture;
uniform bool UseUVFromGPU;
uniform bool UsePosTOuv;
uniform int  UVprojectionType;

uniform vec3 material_Ambient;
uniform vec3 material_Diffuse;
uniform vec3 material_Specular;
uniform vec3 material_Emissive;

uniform bool VisualRefraction;
uniform bool VisualReflection;
uniform float RefractionIndex;
uniform float FresnelPower;
uniform float PhongEnvironmentRatio;
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

vec2 calcUV_cylindrical(vec3 entity)
{
    vec3 v = entity;
    vec2 uv;
    float ux = degrees(atan(v.z/ v.x));
    ux += 180.f;
    float uy = (v.y+1.f) / (2.f);
    uv.x = ux / 360.f;
    uv.y = uy;
    return uv;
}
vec2 calcUV_spherical(vec3 entity)
{
    vec3 v = entity;
    vec2 uv;
    float  r = sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
    float theta = degrees(atan(v.z/v.x));
    theta += 180.f;
    float phi = 180.f - (acos(v.y / r))* 180.f / acos(-1) ;
    uv.x = theta / 360.f;
    uv.y = phi / 180.f;

    return uv;
}
vec2 calcUV_cube(vec3 entity)
{ 

  float x = entity.x;
  float y = entity.y;
  float z = entity.z;
  float absX = abs(x);
  float absY = abs(y);
  float absZ = abs(z);
  bool isXPositive = x > 0 ? true : false;
  bool isYPositive = y > 0 ? true : false;
  bool isZPositive = z > 0 ? true : false;
  
  float maxAxis =0;
  float uc =0;
  float vc =0;
  
  // POSITIVE X
  if (isXPositive && absX >= absY && absX >= absZ) {
    // u (0 to 1) goes from +z to -z
    // v (0 to 1) goes from -y to +y
    maxAxis = absX;
    uc = -z;
    vc = y;
  }
  // NEGATIVE X
  else if (!isXPositive && absX >= absY && absX >= absZ) {
    // u (0 to 1) goes from -z to +z
    // v (0 to 1) goes from -y to +y
    maxAxis = absX;
    uc = z;
    vc = y;
  }
  // POSITIVE Y
  else if (isYPositive && absY >= absX && absY >= absZ) {
    // u (0 to 1) goes from -x to +x
    // v (0 to 1) goes from +z to -z
    maxAxis = absY;
    uc = x;
    vc = -z;
  }
  // NEGATIVE Y
  else if (!isYPositive && absY >= absX && absY >= absZ) {
    // u (0 to 1) goes from -x to +x
    // v (0 to 1) goes from -z to +z
    maxAxis = absY;
    uc = x;
    vc = z;
  }
  // POSITIVE Z
  else if (isZPositive && absZ >= absX && absZ >= absY) {
    // u (0 to 1) goes from -x to +x
    // v (0 to 1) goes from -y to +y
    maxAxis = absZ;
    uc = x;
    vc = y;
  }
  // NEGATIVE Z
  else if (!isZPositive && absZ >= absX && absZ >= absY) {
    // u (0 to 1) goes from +x to -x
    // v (0 to 1) goes from -y to +y
    maxAxis = absZ;
    uc = -x;
    vc = y;
  }

  vec2 uv=vec2(0,0);
  // Convert range from -1 to 1 to 0 to 1
  uv.x = 0.5f * (uc / maxAxis + 1.0f);
  uv.y = 0.5f * (vc / maxAxis + 1.0f);
  return uv;
}


vec2 CalcUV(int mode)
{
    vec2 UV;
    vec3 v;
    if (UsePosTOuv)
        v = FragPos;
    else
        v = norm;
    if(mode == 0)
    {     
        UV = calcUV_cylindrical(v);
    }
    else if(mode ==1)
    {
        UV = calcUV_spherical(v);
    }
    else
    {
        UV = calcUV_cube(v);
    }

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

vec3 GetCubeTexture(vec3 v)
{
    vec3 absVec = abs(v);
    vec2 uv = vec2(0,0);
    if( absVec.x >= absVec.y && absVec.x >= absVec.z )  
    {
        (v.x < 0.0) ? (uv.x =-v.z/ absVec.x) : (uv.x = v.z/ absVec.x );
        uv.y = v.y / absVec.x;
        uv = (uv+1.f)/2.f;

        if(v.x < 0)
            return texture(skybox_RIGHT, uv).rgb;
        else
            return texture(skybox_LEFT, uv).rgb;
    }
    else if( absVec.y >= absVec.x && absVec.y >= absVec.z ) 
    {
        (v.y < 0.0) ? (uv.y = v.z / absVec.y) : (uv.y =  -v.z / absVec.y);
        uv.x =-v.x / absVec.y;
        uv = (uv+1.f)/2.f;

        if(v.y < 0)
            return texture(skybox_BOTTOM, uv).rgb;
        else
            return texture(skybox_TOP, uv).rgb;
    }
    else if( absVec.z >= absVec.x && absVec.z >= absVec.y ) 
    {
         (v.z < 0.0) ? (uv.x = v.x / absVec.z) : (uv.x = -v.x / absVec.z);
         uv.y = v.y / absVec.z;
         uv = (uv+1.f)/2.f;

        if(v.z < 0)
            return texture(skybox_BACK, uv).rgb;
        else
            return texture(skybox_FRONT, uv).rgb;
    }
}

vec3 CalcEnvironmentReflection()
{
    vec3 I = normalize(viewPos-FragPos);

    vec3 N = normalize(NRM);


    vec3 reflectDir = (2*dot(N,I) * N - I);
    return GetCubeTexture(reflectDir);

}

vec3 CalcEnvironmentRefraction() 
{
    vec3 I = normalize(viewPos - FragPos);
    vec3 N = normalize(NRM);
    float K = 1.0/RefractionIndex;
    float d = dot(N,I);

    float EtaR = K*0.99;
    float EtaG = K; // Ratio of indices of refraction
    float EtaB = K*1.01;

    vec3 RefractR_T = (EtaR*d - sqrt(1-pow(EtaR,2)*(1-pow(d,2))))*N - (EtaR*I);
    vec3 RefractG_T = (EtaG*d - sqrt(1-pow(EtaG,2)*(1-pow(d,2))))*N - (EtaG*I);
    vec3 RefractB_T = (EtaB*d - sqrt(1-pow(EtaB,2)*(1-pow(d,2))))*N - (EtaB*I);

    RefractR_T = GetCubeTexture(RefractR_T);
    RefractG_T = GetCubeTexture(RefractG_T);
    RefractB_T = GetCubeTexture(RefractB_T);    
    vec3 RefractColor;
    RefractColor.r = RefractR_T.r;
    RefractColor.g = RefractG_T.g;
    RefractColor.b = RefractB_T.b;

    return RefractColor;
}

float CalcRatio()
{
    vec3 I = normalize(viewPos - FragPos);
    vec3 N = normalize(NRM);
    float Eta = 1.0/RefractionIndex;
    float F = ((1.0-Eta) * (1.0-Eta)) / ((1.0+Eta) * (1.0+Eta));

    float Ratio = F + (1.0 - F) * pow((1.0 - dot(-I, N)), FresnelPower);
    return Ratio;
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

    vec3 EnvironmentColor=vec3(0);
    if(VisualReflection && !VisualRefraction)
        EnvironmentColor=  CalcEnvironmentReflection();   
    if(VisualRefraction && !VisualReflection)
        EnvironmentColor=  CalcEnvironmentRefraction();   
    if(VisualRefraction && VisualReflection)
        EnvironmentColor = mix(CalcEnvironmentRefraction(),CalcEnvironmentReflection(),CalcRatio());
	

    outColor =  vec4( mix(final,EnvironmentColor,PhongEnvironmentRatio), 1.0);   
   
}