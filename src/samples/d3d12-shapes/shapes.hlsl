
cbuffer cbPerPass : register(b1)
{
    float4x4 view;              // 64
    float4x4 projection;        // 64
    float4x4 viewProjection;    // 64
    float    totalTime = 0.0f;  // 4
    float    deltaTime = 0.0f;  // 4
    float    pad;               // 4
    float4   pad2;              // 16
    float3x3 pad3;              // 36
};


cbuffer cbPerObject : register(b0)
{
    float4x4 modelMatrix;       //64
};

struct VertexOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VertexOut VSMain(float3 position : POSITION, float4 color : COLOR)
{
    VertexOut result;

    float4x4 mvp = mul(modelMatrix, viewProjection); 
    result.position = mul(float4(position, 1.0f), mvp);
    result.color    = color;

    return result;
}

float4 PSMain(VertexOut input) : SV_TARGET
{
    return input.color;
}