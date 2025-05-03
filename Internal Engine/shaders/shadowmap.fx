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
    float4 depthPosition : TEXCOORD0;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Pos = mul(input.Pos, WVP);
    output.depthPosition = input.Pos;
    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float depth = input.depthPosition.z / input.depthPosition.w;
    return float4(depth, depth, depth, 1.0f);
}