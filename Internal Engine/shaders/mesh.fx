#define PCF_SIZE 2

cbuffer cbPerFrame : register(b0)
{
    float4x4 lightViewProj;
    float4 direction;
};

cbuffer cbPerObject : register(b1)
{
    float4x4 WVP;
    float4x4 World;
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
    float3 LightDirection : TEXCOORD1;
    float4 ShadowPos : TEXCOORD2;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    
    output.Pos = mul(input.Pos, WVP);
    output.TexCoord = float2(input.TexCoord.x, -input.TexCoord.y) * texture_scale + texture_offset;
    output.Normal = normalize(mul(input.Normal, (float3x3) World));
    output.LightDirection = normalize(mul(input.Pos, LightPos));
    output.ShadowPos = mul(input.Pos, LightPos);
    
    return output;
}

Texture2D ObjTexture : register(t0);
Texture2D<float> ShadowMap : register(t1);

SamplerState ObjSamplerState : register(s0);
SamplerComparisonState gSamShadow : register(s1);

float PCF(float2 uv, float depth, float2 texelSize)
{
    float shadow = 0.0;
    for (int x = -PCF_SIZE; x <= PCF_SIZE; ++x)
    {
        for (int y = -PCF_SIZE; y <= PCF_SIZE; ++y)
        {
            float2 offset = float2(x, y) * texelSize;
            shadow += ShadowMap.SampleCmpLevelZero(gSamShadow, uv + offset, depth);
        }
    }
    float totalSamples = (2 * PCF_SIZE + 1) * (2 * PCF_SIZE + 1);
    return shadow / totalSamples;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float4 color = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
    float diffuseFactor = dot(input.Normal, input.LightDirection);
    float4 lightIntensity = lerp(0.2f, 1.0f, saturate(diffuseFactor));
    return lightIntensity;
    
    //float3 shadowPos = input.ShadowPos.xyz / input.ShadowPos.w;
    //shadowPos.xy = shadowPos.xy * 0.5f + 0.5f;
    //shadowPos.y = 1.0f - shadowPos.y;
    
    //float2 texelSize = 1.0 / float2(4192, 4192);
    //float shadowBias = max(0.005 * (1.0 - diffuseFactor), 0.001);
    //float shadow = 1.0f;
    //float shadowDarkness = 2.25f;
    //if (all(shadowPos.xy >= 0) && all(shadowPos.xy <= 1))
    //{
        //float pcf = PCF(shadowPos.xy, shadowPos.z - shadowBias, texelSize);
        //shadow = lerp(1.0, pcf, shadowDarkness);
    //}
    
    //float ambient = 1.0f;
    //float4 finalColor = color * (ambient + lightIntensity * shadow);
    //finalColor.a = color.a;
    //return finalColor;
}