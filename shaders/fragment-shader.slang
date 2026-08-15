import shader_types;

// Define the fragment shader
[shader("fragment")]
float4 fragMain(VertexOutput inVert) : SV_Target {
    // Return the color of the fragment for the given vertex
    // Assuming an alpha channel of 1.0 for now
    return float4(inVert.color, 1.0);
}