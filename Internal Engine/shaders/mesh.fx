cbuffer cbPerFrame : register(b0)
{
    float4 direction;
    float4 ambient;
    float4 diffuse;
    float intensity;
    float darkness;
    float4x4 lightView;
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
    output.ShadowPos = mul(float4(input.Pos.xyz, 1.0f), lightView);
    
    return output;
}

Texture2D ObjTexture : register(t0);
Texture2D ShadowMap : register(t1);

SamplerState ObjSamplerState : register(s0);
SamplerComparisonState gSamShadow : register(s1)
{
    Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
    AddressU = Border;
    AddressV = Border;
    BorderColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    ComparisonFunc = LESS_EQUAL;
};

float CalculateShadow(float4 shadowPos)
{
    float3 projCoords = shadowPos.xyz / shadowPos.w;
    
    projCoords.xy = 0.5f * projCoords.xy + 0.5f;
    projCoords.y = 1.0f - projCoords.y;
    
    if (projCoords.z > 1.0f || projCoords.x < 0.0f || projCoords.x > 1.0f ||
       projCoords.y < 0.0f || projCoords.y > 1.0f)
        return 1.0f;
    
    const float bias = 0.001f;
    float shadow = 0.0f;
    float2 texelSize = 1.0f / 2048.0f;
    
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float depth = ShadowMap.SampleCmpLevelZero(gSamShadow,
                projCoords.xy + float2(x, y) * texelSize, projCoords.z - bias);
            shadow += depth;
        }
    }
    
    return shadow / 9.0f;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    //float4 color = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
    //
    //if (color.a == 0.0f)
    //    return float4(0.3f, 0.3f, 0.3f, alpha);
    //
    //float3 lightDir = normalize(-direction.xyz);
    //
    //float diffuseFactor = saturate(dot(input.Normal, lightDir));
    //float shadowFactor = 1.0 - diffuseFactor;
    //
    //float3 shadowColor = lerp(float3(0, 0, 0), color.rgb, darkness);
    //
    //float3 finalColor = (ambient.rgb * color.rgb) + (diffuseFactor * diffuse.rgb * color.rgb * intensity);
    //finalColor = lerp(finalColor, shadowColor, shadowFactor);
    //
    //float shadow = CalculateShadow(input.ShadowPos);
    //finalColor *= shadow;
    //
    //float3 baseColor = finalColor + texture_color.rgb;
    //
    //return float4(baseColor, color.a * alpha);
    
    float depth = ShadowMap.SampleLevel(ObjSamplerState, input.ShadowPos.xy, input.ShadowPos.w).r;
    return float4(depth, depth, depth, 1.0f);
}