// Custom struct for inputting information from the vertex buffer to the vertex shader
struct VSInput {
    float2 inPosition;
    float3 inColor;
}

// Custom struct for outputting vertex information from the vertex shader
struct VSOutput {
    float4 pos : SV_Position;
    float3 color;
}



// Define the vertex shader
[shader("vertex")]
VSOutput vertMain(VSInput input) {
    // Define output to be passed to fragment shader
    VSOutput output;

    // Convert each 2D vertex position to a homogenous coordinate to allow translation/projection
    // Must be 4D because Vulkan assumes 3D coordinates, even though we're drawing a 2D triangle
    // X, Y, Z, W
    output.pos = float4(input.inPosition, 0.0, 1.0);

    // Add the associated vertex color to the struct
    output.color = input.inColor;

    // Pass the processed vertex information to the fragment shader
    return output;
}



// Define the fragment shader
[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_Target {
    // Return the color of the fragment for the given vertex
    // Assuming an alpha channel of 1.0 for now
    return float4(vertIn.color, 1.0);
}