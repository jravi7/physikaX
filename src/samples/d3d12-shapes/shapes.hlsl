
cbuffer cbPerPass: register(b0)
{
    float4x4 mvp; 
};

struct VertexOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VertexOut VSMain(float3 position : POSITION, float4 color : COLOR)
{
    VertexOut result;

    result.position = mul(float4(position, 1.0f), mvp);
    result.color    = color;

    return result;
}

float4 PSMain(VertexOut input) : SV_TARGET
{
    return input.color;
}