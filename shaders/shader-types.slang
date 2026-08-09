// Create a custom datatype for outputting vertex information to the graphics pipeline
struct VertexOutput {
    float3 color;
    float4 sv_position : SV_Position;
};