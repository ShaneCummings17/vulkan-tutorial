// Custom struct for outputting vertex information to the fragment shader
struct VertexOutput {
    float3 color;
    float4 sv_position : SV_Position;
};