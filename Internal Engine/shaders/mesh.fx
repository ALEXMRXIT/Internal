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
    float4 ShadowPos : TEXCOORD2;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    
    output.Pos = mul(input.Pos, WVP);
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

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float4 color = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
    float diffuseFactor = dot(input.Normal, normalize(direction.xyz));
    float4 lightIntensity = lerp(0.2f, 1.0f, saturate(diffuseFactor));
    
    float3 shadowPos = input.ShadowPos.xyz / input.ShadowPos.w;
    shadowPos.xy = shadowPos.xy * 0.5f + 0.5f;
    shadowPos.y = 1.0f - shadowPos.y;

    float shadowBias = max(0.01 * (1.0 - diffuseFactor), 0.001);
    float shadow = 1.0f;
    if (all(shadowPos.xy < 0) || all(shadowPos.xy > 1))
    {
        shadow = ShadowMap.SampleCmpLevelZero(gSamShadow, shadowPos.xy, shadowPos.z - shadowBias);
    }
    
    float4 finalColor = color * lightIntensity * shadow;
    finalColor.a = color.a;
    
    return finalColor;
}