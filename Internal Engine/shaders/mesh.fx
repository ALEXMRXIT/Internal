#define PCF_SIZE 2

cbuffer cbPerObject : register(b0)
{
    float4x4 WVP;
    float4x4 World;
    float4x4 ViewProjection;
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
    output.Normal = mul(input.Normal, (float3x3)World);
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
    float3 cameraPos;
    float3 paddingDirection;
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

static const float2 shadowTexelSize = 1.0f / shadowSize;

float PCF(float2 uv, float depth, float2 texelSize)
{
    float shadow = 0.0;
    float totalSamples = 0.0;
    
    [unroll]
    for (int x = -PCF_SIZE; x <= PCF_SIZE; ++x)
    {
        [unroll]
        for (int y = -PCF_SIZE; y <= PCF_SIZE; ++y)
        {
            shadow += ShadowMap.SampleCmpLevelZero(gSamShadow,
                uv + float2(x, y) * texelSize, depth);
            totalSamples += 1.0;
        }
    }
    
    return shadow * rcp(totalSamples);
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float4 texColor = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
    [flatten]
    if (texColor.a == 0.0f)
        texColor = float4(0.3f, 0.3f, 0.3f, 1.0f);
    
    float3 normal = normalize(input.Normal);
    float3 lightDir = normalize(-lightDirection);
    float NdotL = max(saturate(dot(normal, lightDir)), 0.1f);
    
    float3 shadowPos = input.ShadowPos.xyz / input.ShadowPos.w;
    shadowPos.xy = saturate(shadowPos.xy * 0.5f + 0.5f);
    shadowPos.y = 1.0f - shadowPos.y;
    
    float depthTest = shadowPos.z - (0.05 * bias);
    float shadowResult = (baked == 1.0f) ?
            ShadowMap.SampleCmpLevelZero(gSamShadow, shadowPos.xy, depthTest) :
            PCF(shadowPos.xy, depthTest, shadowTexelSize);
    
    float3 ambient = ambiend_color.rgb * texColor.rgb;
    float3 diffuse = NdotL * lightColor.rgb * diffuseIntensity;
    
    float3 specular = 0.0;
    [branch]
    if (NdotL > 0.0)
    {
        float3 viewDir = normalize(cameraPos - input.WorldPos);
        float3 reflectDir = reflect(-lightDir, normal);
        float specularTerm = pow(max(dot(viewDir, reflectDir), 0.0001),
                          max(specularPower, 1.0));
        specular = specularTerm * lightColor.rgb * specularColor.rgb * specularIntensity;
    }
    
    float shadowFactor = lerp(1.0f, shadowResult, shadowIntensity);
    float3 lighting = ambient + (diffuse + specular) * shadowFactor;
    return float4(lighting * texColor.rgb, texColor.a);
}