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
    float3 WorldPos : TEXCOORD1;
    float4 ShadowPos : TEXCOORD2;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    
    output.Pos = mul(input.Pos, WVP);
    output.Normal = mul(input.Normal, (float3x3) World);
    output.Normal = normalize(output.Normal);
    output.TexCoord = float2(input.TexCoord.x, -input.TexCoord.y) * texture_scale + texture_offset;
    output.WorldPos = mul(input.Pos, World).xyz;
    output.ShadowPos = mul(float4(output.WorldPos, 1.0f), lightView);
    
    return output;
}

Texture2D ObjTexture : register(t0);
Texture2D ShadowMap : register(t1);
SamplerState ObjSamplerState : register(s0);
SamplerState ShadowSamplerState : register(s1);

float CalculateShadow(float4 shadowPos)
{
    shadowPos.xyz /= shadowPos.w;
    float2 shadowUV = shadowPos.xy * 0.5f + 0.5f;
    shadowUV.y = 1.0f - shadowUV.y; // Flip Y для DirectX текстуры

    // Глубина в пространстве света [0, 1]
    float depth = shadowPos.z;

    // Bias для избежания self-shadowing
    const float bias = 0.001f;
    depth -= bias;

    // Проверка границ
    if (shadowUV.x < 0.0f || shadowUV.x > 1.0f ||
        shadowUV.y < 0.0f || shadowUV.y > 1.0f ||
        depth < 0.0f || depth > 1.0f)
    {
        return 1.0f; // Вне тени
    }

    // Сравниваем глубину
    float shadowDepth = ShadowMap.Sample(ShadowSamplerState, shadowUV).r;
    return (depth <= shadowDepth) ? 1.0f : darkness; // darkness = 0.3f например
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float4 color = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
    
    if (color.a == 0.0f)
        return float4(0.3f, 0.3f, 0.3f, alpha);
    
    float3 lightDir = normalize(-direction.xyz);
    
    float diffuseFactor = saturate(dot(input.Normal, lightDir));
    float shadowFactor = 1.0 - diffuseFactor;
    
    float3 shadowColor = lerp(float3(0, 0, 0), color.rgb, darkness);
    
    float3 finalColor = (ambient.rgb * color.rgb) + (diffuseFactor * diffuse.rgb * color.rgb * intensity);
    finalColor = lerp(finalColor, shadowColor, shadowFactor);
    
    float shadow = CalculateShadow(input.ShadowPos);
    finalColor *= shadow;
    
    float3 baseColor = finalColor + texture_color.rgb;
    
    return float4(baseColor, color.a * alpha);
}