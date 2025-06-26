cbuffer cbPerObject : register(b0)
{
    float4x4 WVP;
    float4x4 World;
    float4x4 ViewProjection;
    float4x4 InverseWorld;
    float4x4 LightPos;
    float2 texture_scale;
    float2 texture_offset;
};

struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    float4 ShadowPos : TEXCOORD1;
    float3 WorldPos : TEXCOORD2;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    
    float4 worldPos = mul(input.Pos, World);
    output.Pos = mul(worldPos, ViewProjection);
    output.WorldPos = worldPos.xyz;
    output.TexCoord = float2(input.TexCoord.x, -input.TexCoord.y) * texture_scale + texture_offset;
    output.Normal = mul(input.Normal, (float3x3)InverseWorld);
    output.Normal = normalize(output.Normal);
    output.ShadowPos = mul(worldPos, LightPos);
    
    return output;
}

cbuffer directionOption : register(b0)
{
    float4 lightDirection;
    float4 lightColor;
    float4 ambiend_color;
    float baked;
    float diffuseIntensity;
    float shadowIntensity;
    float bias;
    float2 shadowSize;
    float pcfSize;
    float3 cameraPos;
    float2 paddingDirection;
}

cbuffer modelMaterial : register(b1)
{
    float4 specularColor;
    float specularPower;
    float specularIntensity;
    float2 paddingMaterial;
}

Texture2D ObjTexture : register(t0);
Texture2D<float> ShadowMap : register(t1);

SamplerState ObjSamplerState : register(s0);
SamplerComparisonState gSamShadow : register(s1);

float PCF(float2 uv, float depth, float2 texelSize)
{
    float shadow = 0.0;
    [loop] for (int x = -pcfSize; x <= pcfSize; ++x)
    {
        [loop] for (int y = -pcfSize; y <= pcfSize; ++y)
        {
            float2 offset = float2(x, y) * texelSize;
            shadow += ShadowMap.SampleCmpLevelZero(gSamShadow, uv + offset, depth);
        }
    }
    float totalSamples = (2 * pcfSize + 1) * (2 * pcfSize + 1);
    return shadow / totalSamples;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float3 normal = normalize(input.Normal);
    
    float3 lightDir = normalize(lightDirection);
    
    float diff = max(dot(normal, lightDir), 0.0);
    float3 diffuse = diff * ambiend_color * diffuseIntensity;
    
    float3 viewDir = normalize(cameraPos - input.WorldPos);
    float3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    float3 specular = spec * lightColor;
    
    float4 texColor = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
    float3 result = (ambiend_color.xyz + diffuse + specular) * texColor.rgb;
    return float4(result, texColor.a);
    
        //float3 shadowPos = input.ShadowPos.xyz / input.ShadowPos.w;
    //shadowPos.xy = saturate(shadowPos.xy * 0.5f + 0.5f);
    //shadowPos.y = 1.0f - shadowPos.y;
    
    //float shadowBias = max(0.001 * (1.0 - dot(normal, lightDir)), 0.0005);
    //float2 texelSize = 1.0 / shadowSize;
    //float depthTest = shadowPos.z - shadowBias;
    //float shadowResult = (baked == 1.0f) ?
    //        step(depthTest, ShadowMap.Sample(ObjSamplerState, shadowPos.xy).r) :
    //        PCF(shadowPos.xy, depthTest, texelSize);
        
    //float shadowFactor = lerp(1.0f, shadowResult, shadowIntensity);
    
    //float4 finalColor = color * ambiend_color;
    //finalColor.rgb *= diffuseShadow;                        // diffuse shadow
    //finalColor.rgb *= shadowFactor * diffuseIntensity;      // shadow map + intensity
    //finalColor.a = color.a;
    //
    //return finalColor;
}