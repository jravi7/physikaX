StructuredBuffer<float> vecAdd1: register(t0); 
StructuredBuffer<float> vecAdd2: register(t1);
RWStructuredBuffer<float> output: register(u0); 

[numthreads(32, 1, 1)]
void CSMain(int3 dtid: SV_DispatchThreadID)
{
    output[dtid.x] = 100 * (vecAdd1[dtid.x] + vecAdd2[dtid.x]);
}