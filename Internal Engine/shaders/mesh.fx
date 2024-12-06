cbuffer cbPerObject : register(b0)
{
    float4x4 WVP;
    float2 texture_scale;
    float2 texture_offset;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;

struct VS_INPUT
{
    float4 Pos : POSITION;
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
    output.Pos = mul(input.Pos, WVP);
    output.TexCoord = (input.TexCoord * texture_scale) + texture_offset;
    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float4 color = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
    if (color.a == 0)
        return float4(0.5f, 0.5f, 0.5f, 1.0f);
    return color;

}