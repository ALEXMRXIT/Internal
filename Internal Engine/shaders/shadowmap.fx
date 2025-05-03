cbuffer cbPerObject : register(b0)
{
    float4x4 WVP;
};

struct VS_INPUT
{
    float4 Pos : POSITION;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float Depth : TEXCOORD0;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Pos = mul(input.Pos, WVP);
    output.Depth = input.Pos.z / input.Pos.w;
    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    return float4(input.Depth, input.Depth, input.Depth, 1.0f);
}