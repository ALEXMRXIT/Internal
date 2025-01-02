cbuffer cbPerFrame : register(b0)
{
    float4 direction;
    float4 ambient;
    float4 diffuse;
    float intensity;
    float darkness;
};

cbuffer cbPerObject : register(b1)
{
    float4x4 WVP;
    float4x4 World;
    float2 texture_scale;
    float2 texture_offset;
};

cbuffer cbSelectable : register(b2)
{
    float4 selectable;
    float4 texture_color;
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
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    
    output.Pos = mul(input.Pos, WVP);
    output.Normal = mul(input.Normal, (float3x3) World);
    output.Normal = normalize(output.Normal);
    output.TexCoord = float2(input.TexCoord.x, -input.TexCoord.y) * texture_scale + texture_offset;
    output.WorldPos = mul(input.Pos, World).xyz;
    
    return output;
}

Texture2D ObjTexture;
SamplerState ObjSamplerState;

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float4 color = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
    
    if (color.a == 0)
        return float4(0.3f, 0.3f, 0.3f, 1.0f);
    
    float3 lightDir = normalize(-direction.xyz);
    
    float diffuseFactor = saturate(dot(input.Normal, lightDir));
    float shadowFactor = 1.0 - diffuseFactor;
    
    float3 shadowColor = lerp(float3(0, 0, 0), color.rgb, darkness);
    
    float3 finalColor = (ambient.rgb * color.rgb) + (diffuseFactor * diffuse.rgb * color.rgb * intensity);
    finalColor = lerp(finalColor, shadowColor, shadowFactor);
    
    float3 baseColor = finalColor + texture_color.rgb;
    
    return float4(baseColor, color.a * selectable.y);
}