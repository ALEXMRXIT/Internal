cbuffer cbPerObject : register(b0)
{
    float4x4 WVP;
};

float4 VS(float3 position : POSITION) : SV_POSITION
{
    return mul(float4(position, 1.0f), WVP);
}

float4 PS() : SV_Target
{
    return float4(0.0f, 0.0f, 0.0f, 1.0f);
}