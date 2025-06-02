cbuffer cbPerFrame : register(b0)
{
    float4 direction;
    float intensity;
    float4x4 lightViewProj;
};

cbuffer cbPerObject : register(b1)
{
    float4x4 WVP;
    float4x4 World;
    float2 texture_scale;
    float2 texture_offset;
};

cbuffer cbAdditionalColored : register(b2)
{
    float4 texture_color;
    float alpha;
}

struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float4 ShadowPos : TEXCOORD1;
    float3 WorldPos : TEXCOORD2;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    
    output.Pos = mul(input.Pos, WVP);
    output.WorldPos = mul(input.Pos, World).xyz;
    output.Normal = mul(input.Normal, (float3x3)World);
    output.Normal = normalize(output.Normal);
    output.TexCoord = float2(input.TexCoord.x, -input.TexCoord.y) * texture_scale + texture_offset;
    output.ShadowPos = mul(float4(input.Pos.xyz, 1.0f), lightViewProj);
    
    return output;
}

Texture2D ObjTexture : register(t0);
Texture2D<float> ShadowMap : register(t1);

SamplerState ObjSamplerState : register(s0);
SamplerComparisonState gSamShadow : register(s1);

float CalculateShadow(float4 shadowPos, float3 normal, float3 lightDir)
{
    float3 projCoords = shadowPos.xyz / shadowPos.w;
    
    projCoords.xy = projCoords.xy * 0.5 + 0.5;
    projCoords.y = 1.0 - projCoords.y;
    
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
    
    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
        return 1.0;
    
    return ShadowMap.SampleCmpLevelZero(gSamShadow, projCoords.xy, projCoords.z - bias);
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float3 projCoords = input.ShadowPos.xyz / input.ShadowPos.w;
    
    if (any(abs(projCoords.xy) > 1.0f))
        return float4(1, 0, 0, 1); // Красный - за пределами XY
    if (projCoords.z < 0.0 || projCoords.z > 1.0)
        return float4(0, 0, 1, 1); // Синий - за пределами Z
        
    return float4(0, 1, 0, 1); // Зеленый - внутри frustum
}