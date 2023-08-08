// Defaults for number of lights.
#ifndef NUM_LIGHTS
    #define NUM_LIGHTS 0
#endif 

#ifndef NUM_DIRECTIONAL_LIGHTS
    #define NUM_DIRECTIONAL_LIGHTS 0
#endif 

#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 0
#endif 

#ifndef NUM_SPOT_LIGHTS
    #define NUM_SPOT_LIGHTS 0
#endif 
struct VertexOut
{
    float4 position : SV_POSITION;
    float4 worldPosition: POSITION; 
    float4 worldNormal: NORMAL; 
    float4 color : COLOR;
};

struct VertexInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

struct Light
{
    float3 position; 
    float  cutOffStart;  //16
    float3 direction; 
    float  cutOffEnd;    //32 
    float3 strength; 
    float  innerCutOff;     //48
    float3 ambientStrength; 
    float outerCutOff;            //64 
};

struct Material
{
    float4 diffuseAlbedo;
    float3 fresnel;
    float  roughness;
};

cbuffer cbPerObject : register(b0)
{
    float4x4 gModelMatrix;       //64
    float3x3 gNormalMatrix;
};

cbuffer cbPerPass : register(b1)
{
    float4x4 gView;              // 64
    float4x4 gProjection;        // 64
    float4x4 gViewProjection;    // 64
    float3   gEyePosition;       // 
    float    gTotalTime = 0.0f;  // 16
    float    gDeltaTime = 0.0f;  // 
    float3   pad;               // 16
    Light   gLights[NUM_LIGHTS];
};

cbuffer materialData : register(b2)
{
    Material gMaterial;
};


float ComputeAttenuation(float d, float cutOffStart, float cutOffEnd)
{
    return saturate((cutOffEnd - d) / (cutOffEnd - cutOffStart));
}

float3 ComputeSchlickFresnel(float3 R0, float3 lightVector, float3 vertexNormal)
{
    float  cosineAngle = saturate(dot(vertexNormal, lightVector));
    float  f0          = 1 - cosineAngle; 

    float3 Rf = R0 + (1.0f - R0) * (f0 * f0 * f0 * f0 * f0);
    return Rf; 
}

float4 ComputeBlingPhongReflectance(float3 L, float3 N, float3 toEyeVector, float4 lightStrength, Material material)
{
    L = normalize(L);
    const float3 fresnelReflectance = ComputeSchlickFresnel(material.fresnel, L, N); 
    const float3 halfVector = normalize(L + toEyeVector); 
    const float shininess = 1.0f - material.roughness; 
    const float m = shininess * 256.0f; 
    const float lambertCos = saturate(dot(L, N)); 
    const float dotHN = saturate(dot(halfVector, N)); 
    float4 specular =  float4(fresnelReflectance * (m + 8)/8*pow(dotHN, m), 1.0f); 
    specular = specular / (specular + 1.0f);// Our spec formula goes outside [0,1] range, but we are doing LDR rendering.  So scale it down a bit.
    float4 blingPhongModel =  lightStrength * lambertCos * (material.diffuseAlbedo + specular);
    return blingPhongModel; 
}

float4 ComputePointLightContribution(float3 wVertexPosition, float3 wVertexNormal, float3 eyePosition,
                                      Light light, Material material)
{
    float4 ambient       = material.diffuseAlbedo * float4(light.ambientStrength, 1.0f); 
    float4 result        = ambient; 
    float3 lightVector = light.position - wVertexPosition; 
    float  d             = length(lightVector); 
    float  attenuation = ComputeAttenuation(d, light.cutOffStart, light.cutOffEnd);
    float4 lightStrength = float4(attenuation * light.strength, 1.0f); 
    float3 toEyeVector = eyePosition - wVertexPosition; 
    result += ComputeBlingPhongReflectance(lightVector, wVertexNormal, toEyeVector, lightStrength, material);
    return result;
}

float4 ComputeSpotLightContribution(float3 wVertexPosition, float3 wVertexNormal, float3 eyePosition,
                                      Light light, Material material)
{
    //! Spot lights based on https://learnopengl.com/Lighting/Light-casters
    float4 ambient       = material.diffuseAlbedo * float4(light.ambientStrength, 1.0f); 
    float4 result        = ambient; 
    // vector calculations
    float3 lightVector = light.position - wVertexPosition; 
    float3 toEyeVector = eyePosition - wVertexPosition; 
    float  d             = length(lightVector); 
    float attenuation          = ComputeAttenuation(d, light.cutOffStart, light.cutOffEnd);

    lightVector = normalize(lightVector); 
    float  thetaOfSpotAngle = saturate(dot(lightVector, -normalize(light.direction)));
    float  epsilon          = light.innerCutOff - light.outerCutOff;
    float  spotIntensity    = saturate(thetaOfSpotAngle - light.outerCutOff / epsilon);
    float4 lightStrength = float4(light.strength * attenuation, 1.0f); 
    result += ComputeBlingPhongReflectance(lightVector, wVertexNormal, toEyeVector, lightStrength, material);
    result *= spotIntensity; 
    
    return result;
}

float4 ComputeDirectionalLightContribution(float3 wVertexPosition, float3 wVertexNormal, float3 eyePosition,
                                      Light light, Material material)
{
    float4 ambient       = material.diffuseAlbedo * float4(light.ambientStrength, 1.0f); 
    float4 result        = ambient; 
    float3 lightVector   = normalize(- light.direction); 
    float4 lightStrength = float4(light.strength, 1.0f); 
    float3 toEyeVector = eyePosition - wVertexPosition; 
    result += ComputeBlingPhongReflectance(lightVector, wVertexNormal, toEyeVector, lightStrength, material);
    return result;
}


float4 ComputeLightContributions(float3 wVertexPosition, float3 wVertexNormal, float3 eyePosition, Light lights[NUM_LIGHTS],
                                   Material material)
{
    const int numDirLightsStart   = 0;
    const int numDirLightsEnd     = NUM_DIRECTIONAL_LIGHTS;
    const int numPointLightsStart = NUM_DIRECTIONAL_LIGHTS; 
    const int numPointLightsEnd   = NUM_DIRECTIONAL_LIGHTS + NUM_POINT_LIGHTS; 
    const int numSpotLightsStart  = NUM_DIRECTIONAL_LIGHTS + NUM_POINT_LIGHTS; 
    const int numSpotLightsEnd    = NUM_DIRECTIONAL_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS;

    float4 pointLightsContribution       = float4(0.0f, 0.0f, 0.0f, 0.0f); 
    float4 directionalLightsContribution = float4(0.0f, 0.0f, 0.0f, 0.0f); 
    float4 spotLightsContribution        = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    
    // Directional Lights
    for (int ii = numDirLightsStart; ii < numDirLightsEnd; ii++) {
        directionalLightsContribution += ComputeDirectionalLightContribution(wVertexPosition, wVertexNormal, eyePosition, lights[ii], material);
    }

    // Point Lights
    for (int ii = numPointLightsStart; ii < numPointLightsEnd; ii++) {
        pointLightsContribution +=
            ComputePointLightContribution(wVertexPosition, wVertexNormal, eyePosition, lights[ii], material);
    }

    // Spot Lights
    for (int ii = numSpotLightsStart; ii < numSpotLightsEnd; ii++) {
        spotLightsContribution+=ComputeSpotLightContribution(wVertexPosition, wVertexNormal, eyePosition, lights[ii], material);
    }

    return pointLightsContribution + directionalLightsContribution + spotLightsContribution; 
}

VertexOut VSMain(VertexInput vin)
{
    VertexOut vertexOut;

    float4x4 mvp            = mul(gModelMatrix, gViewProjection);
    vertexOut.position      = mul(float4(vin.position, 1.0f), mvp);
    vertexOut.worldPosition = mul(float4(vin.position, 1.0f), gModelMatrix);
    //vertexOut.worldNormal   = float4(mul(vin.normal, gNormalMatrix), 0.0f);
    vertexOut.worldNormal = float4(vin.normal, 0.0f);
    vertexOut.color         = vin.color;

    return vertexOut;
}

float4 PSMain(VertexOut psVertex) : SV_TARGET
{
    float3 wVertexPosition = (float3)psVertex.worldPosition; 
    float3 wVertexNormal = (float3)normalize(psVertex.worldNormal); 
    float4 result = ComputeLightContributions(wVertexPosition, wVertexNormal, gEyePosition, gLights, gMaterial);
    return result;
}