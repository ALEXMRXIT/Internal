cbuffer cbPerObject : register(b0)
{
    float4x4 WVP;
};

struct VS_INPUT
{
    float3 Pos : POSITION;
    float2 TexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Pos = mul(float4(input.Pos, 1.0f), WVP).xyww;
    output.TexCoord = float2(input.TexCoord.x, input.TexCoord.y);
    return output;
}

Texture2D ObjTexture;
SamplerState ObjSamplerState;

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    return ObjTexture.Sample(ObjSamplerState, input.TexCoord);
}