// Custom struct for outputting vertex information to the fragment shader
struct VertexOutput {
    float3 color;
    float4 sv_position : SV_Position;
};

// Create "vertex buffer" that stores positions of vertices in triangle
// Obviously not a REAL buffer; but simple enough for now
static float2 positions[3] = float2[](
    float2(0.0, -0.5),
    float2(0.5, 0.5),
    float2(-0.5, 0.5)
);

// Create the color at each vertex
static float3 colors[3] = float3[](
    float3(1.0, 0.0, 0.0),
    float3(0.0, 1.0, 0.0),
    float3(0.0, 0.0, 1.0)
);



// Define the vertex shader
[shader("vertex")]
VertexOutput vertMain(uint vid: SV_VertexID) {
    // Define output to be passed to fragment shader
    VertexOutput output;

    // Convert each 2D vertex position to a homogenous coordinate to allow translation/projection
    // Must be 4D because Vulkan assumes 3D coordinates, even though we're drawing a 2D triangle
    // X, Y, Z, W
    output.sv_position = float4(positions[vid], 0.0, 1.0);

    // Add the associated vertex color to the struct
    output.color = colors[vid];

    // Pass the processed vertex information to the fragment shader
    return output;
}



// Define the fragment shader
[shader("fragment")]
float4 fragMain(VertexOutput inVert) : SV_Target {
    // Return the color of the fragment for the given vertex
    // Assuming an alpha channel of 1.0 for now
    return float4(inVert.color, 1.0);
}