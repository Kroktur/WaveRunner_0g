struct VSInput {
    float3 inPosition;
    float4 inColor;
};

struct UniformBuffer {
    float4x4 InverseView;
    float4x4 view;
    float4x4 proj;
};


struct Data
{
    UniformBuffer CamUbo;
}
ParameterBlock<Data> data;



struct VSOutput
{
    float4 pos : SV_Position;
    float4 fragColor;
};


[shader("vertex")]
VSOutput vertMain(VSInput input) {
    VSOutput output;

    float4 wsPos =float4(input.inPosition, 1.0f);
    output.pos = mul(data.CamUbo.proj, mul(data.CamUbo.view, wsPos));
    output.fragColor = input.inColor;
    return output;
}


[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET {
    return vertIn.fragColor;
}