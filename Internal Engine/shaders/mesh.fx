#define PCF_SIZE 2

cbuffer cbPerObject : register(b0)
{
    float4x4 WVP;
    float4x4 World;
    float4x4 LightPos;
    float4 LightDirection;
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
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    
    float4 worldPos = mul(input.Pos, World);
    output.Pos = mul(input.Pos, WVP);
    output.TexCoord = float2(input.TexCoord.x, -input.TexCoord.y) * texture_scale + texture_offset;
    float3 worldNormal = normalize(mul(float4(input.Normal, 0.0), World)).xyz;
    output.Normal = worldNormal;
    output.ShadowPos = mul(worldPos, LightPos);
    
    return output;
}

cbuffer directionOption : register(b0)
{
    float4 lightDirection;
    float4 ambiend_color;
    float baked;
    float diffuseIntensity;
    float shadowIntensity;
    float bias;
    float2 shadowSize;
    float pcfSize;
    float padding;
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
    float4 color = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
    float3 normal = input.Normal;
    float3 lightDir = normalize(lightDirection.xyz);
    float diffuseShadow = saturate(dot(normal, -lightDir));
    
    float3 shadowPos = input.ShadowPos.xyz / input.ShadowPos.w;
    shadowPos.xy = saturate(shadowPos.xy * 0.5f + 0.5f);
    shadowPos.y = 1.0f - shadowPos.y;
    
    float shadowBias = max(0.001 * (1.0 - dot(normal, lightDir)), 0.0005);
    float2 texelSize = 1.0 / shadowSize;
    float depthTest = shadowPos.z - shadowBias;
    float shadowResult = (baked == 1.0f) ?
            step(depthTest, ShadowMap.Sample(ObjSamplerState, shadowPos.xy).r) :
            PCF(shadowPos.xy, depthTest, texelSize);
        
    float shadowFactor = lerp(1.0f, shadowResult, shadowIntensity);
    
    float4 finalColor = color * ambiend_color;
    finalColor.rgb *= diffuseShadow;                        // diffuse shadow
    finalColor.rgb *= shadowFactor * diffuseIntensity;      // shadow map + intensity
    finalColor.a = color.a;
    
    return finalColor;
}