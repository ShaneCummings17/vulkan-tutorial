#include <vulkan-tutorial/graphics-pipeline/GraphicsPipeline.hpp>

// Standard C++ Libraries
#include <stdexcept>
#include <fstream>
#include <vector>
#include <string>
#include <format>

// Internal libraries
#include <vulkan-tutorial/buffers/VertexDefinition.hpp>

// Local helper functions
namespace {
    // Read binary data from files into memory
    static std::vector<char> readFile(const std::string& filename) {
        // Open an input file stream as binary
        // Immediately jump the read cursor to the end
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        // Check if the file was successfully found and opened
        if (!file.is_open()) {
            throw std::runtime_error(std::format("failed to open file {}!", filename));
        }

        // Report the current position of the cursor (file size in bytes)
        std::vector<char> buffer(file.tellg());

        // Move the cursor back to the beginning of the file
        file.seekg(0, std::ios::beg);

        // Read the bytes into memory
        file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));

        // Explicitly close the file; not required, but good practice
        file.close();

        // Return the loaded byte array
        return buffer;
    }

    // Create a Vulkan Shader Module object
    // [[nodiscard]] == Issue a warning if the program calls this function but throws away the returned value
    // Loading shaders is expensive! Will help when debugging to catch instances of unnecessary shader load
    [[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char>& code, const vk::raii::Device &logicalDevice) {
        // Create the configuration info about the loaded shader module
        vk::ShaderModuleCreateInfo createInfo{
            .codeSize = code.size() *sizeof(char),                          // The exact size of the code block in bytes
            .pCode = reinterpret_cast<const uint32_t*>(code.data())         // SPIR-V shaders are formatted as 32-bit words. Force the chars into this data format.
        };

        // Create the shader module, AKA allocate the module on the GPU
        vk::raii::ShaderModule shaderModule{
            logicalDevice,
            createInfo
        };

        // Return the shader module
        return shaderModule;
    }
}



/***** CONSTRUCTOR AND DESTRUCTOR *****/
GraphicsPipeline::GraphicsPipeline(
    const vk::raii::Device &logicalDevice,
    const vk::Extent2D &swapchainExtent,
    const vk::SurfaceFormatKHR &swapchainSurfaceFormat
) {
    createGraphicsPipeline(logicalDevice, swapchainExtent, swapchainSurfaceFormat);
}



/**** PUBLIC METHODS *****/
const vk::raii::Pipeline& GraphicsPipeline::getGraphicsPipeline() const {
    return graphicsPipeline;
}



/**** Private METHODS *****/
void GraphicsPipeline::createGraphicsPipeline(
    const vk::raii::Device &logicalDevice,
    const vk::Extent2D &swapchainExtent,
    const vk::SurfaceFormatKHR &swapchainSurfaceFormat
) {
    // STEP #1: Define the PROGRAMMABLE pipeline stages
    // Load compiled shader code file
    auto shaderCode = readFile("build/shaders/slang.spv");

    // Create shader module to load onto the device
    vk::raii::ShaderModule shaderModule = createShaderModule(shaderCode, logicalDevice);

    // Assign the vertex shader to a specific pipeline stage
    vk::PipelineShaderStageCreateInfo vertShaderStageInfo{
        .stage = vk::ShaderStageFlagBits::eVertex,              // The pipeline stage we're operating in
        .module = shaderModule,                                 // The shader module to reference
        .pName = "vertMain"                                     // The entrypoint function to use
    };

    // Assign the fragment shader to a specific pipeline stage
    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{
        .stage = vk::ShaderStageFlagBits::eFragment,            // The pipeline stage we're operating in
        .module = shaderModule,                                 // The shader module to reference
        .pName = "fragMain"                                     // The entrypoint function to use
    };

    // Create an array containing all shader structs
    vk::PipelineShaderStageCreateInfo shaderStages[] = {
        vertShaderStageInfo,
        fragShaderStageInfo
    };



    // STEP #2: Define the FIXED-FUNCTION pipeline stages
    // Describe the format of the vertex data that will be passed to the vertex shader
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &bindingDescription,
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
        .pVertexAttributeDescriptions = attributeDescriptions.data()
    };

    // Define the input assembly (what kind of geometry will be drawn and if primitive restart should be enabled)
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{
        .topology = vk::PrimitiveTopology::eTriangleList,       // Triangle from every three vertices without reuse of vertices
    };

    // Define the viewport (the region of the framebuffer that the output will be rendered to)
    // This will almost always be (0,0) to (width, height) of the swapChainExtent
    vk::Viewport viewport{0.0f, 0.0f, static_cast<float>(swapchainExtent.width), static_cast<float>(swapchainExtent.height), 0.0f, 1.0f};

    // Define the scissor rectangles (the region of the framebuffer within which pixels will actually be stored)
    // i.e. What is the rasterizer keeping? It'll discard everything else
    vk::Rect2D scissor{
        vk::Offset2D{0, 0},
        swapchainExtent
    };

    // Define the number of viewports and scissors at pipeline creation time
    vk::PipelineViewportStateCreateInfo viewportState{
        .viewportCount = 1,
        .scissorCount = 1
    };

    // Define the rasterizer
    // The rasterizer takes geometry shaped by vertices and turns it into fragments to be colored by the fragment shader
    vk::PipelineRasterizationStateCreateInfo rasterizer{
        .depthClampEnable = vk::False,                          // If true, fragments beyond near and far planes are clamped instead of discarded
        .rasterizerDiscardEnable = vk::False,                   // If true, then geometry never passes through the rasterizer; disables all output
        .polygonMode = vk::PolygonMode::eFill,                  // Determines how fragments are generated for geometry; this one means "fill the area of the polygon with fragments"
        .cullMode = vk::CullModeFlagBits::eBack,                // Determines the type of face culling to use; we're culling the back faces
        .frontFace = vk::FrontFace::eClockwise,                 // Specifies the vertex order; Clockwise because Vulkan is a y-down API. Right-hand rule NOT applicable for Vulkan.
        .depthBiasEnable = vk::False,                           // Sometimes used for shadow mapping; ignore for now
        .lineWidth = 1.0f                                       // Describes the thickness of lines in terms of number of fragments
    };

    // Define multisampling (useful for performing anti-aliasing)
    // Keeping disabled for now
    vk::PipelineMultisampleStateCreateInfo multisampling{
        .rasterizationSamples = vk::SampleCountFlagBits::e1,
        .sampleShadingEnable = vk::False
    };

    // Define the color blending configuration for each attached framebuffer (we have 1 for now)
    // Combine the returned color from the fragment shader with the color already in the framebuffer
    vk::PipelineColorBlendAttachmentState colorBlendAttachment{
        .blendEnable = vk::False,                               // If false, new color from fragment shader is passed through unmodified; if true, blend
        .colorWriteMask =                                       // Tell the rendering pipeline which color channels it is allowed to modify; in most cases we want every color channel to be editable
            vk::ColorComponentFlagBits::eR |
            vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB |
            vk::ColorComponentFlagBits::eA
    };

    // Define the GLOBAL color blending settings
    vk::PipelineColorBlendStateCreateInfo colorBlending{
        .logicOpEnable = vk::False,                             // If false, use mix old and new value to produce final color; if true, do a bitwise operation
        .logicOp = vk::LogicOp::eCopy,                          // The bitwise operation to use in bitwise mixing
        .attachmentCount = 1,                                   // Number of attached framebuffers
        .pAttachments = &colorBlendAttachment                   // List of attachment states
    };



    // STEP #3: Setup DYNAMIC RENDERING; stuff that doesn't need to be baked into the pipeline state, but can instead be modified at draw time
    // Define what is required to be dynamically defined (viewport and scissor)
    std::vector<vk::DynamicState> dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};

    // Define dynamic state configuration info
    vk::PipelineDynamicStateCreateInfo dynamicState{
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data()
    };



    // STEP #4: Set up the GRAPHICS PIPELINE object
    // Define the pipeline layout
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
        .setLayoutCount = 0,                                    // What descriptor sets to use; ignore for now
        .pushConstantRangeCount = 0                             // The push constants my shader expects (data pushed directly into command buffer); ignore for now
    };

    // Execute the creation of the pipeline layout on the GPU
    pipelineLayout = vk::raii::PipelineLayout(logicalDevice, pipelineLayoutInfo);

    // We'll be using one color attachment with the format of our swap chain images
    vk::PipelineRenderingCreateInfo PipelineRenderingCreateInfo{
        .colorAttachmentCount = 1,
        .pColorAttachmentFormats = &swapchainSurfaceFormat.format
    };

    // Chain the traditional pipeline struct to a newer struct that enabled Dynamic rendering
    // Define the chain; create a GraphicsPipelineCreateInfo struct and automatically link it to the new PipelineRenderingCreateInfo struct
    // This allows dynamic rendering
    vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain = {
        // Old struct (GraphicsPipelineCreateInfo)
        {
            .stageCount = 2,                                            // Number of pipeline stages
            .pStages = shaderStages,                                    // Binds the shader modules to the pipeline
            .pVertexInputState = &vertexInputInfo,                      // Binds the vertex layout to the pipeline
            .pInputAssemblyState = &inputAssembly,                      // Defines the geometry topology; aka, draw triangles instead of isolated points or lines
            .pViewportState = &viewportState,                           // Defines the viewport and the scissor
            .pRasterizationState = &rasterizer,                         // Bind the rasterizer configuration to the pipeline
            .pMultisampleState = &multisampling,                        // Configure anti-aliasing on the pipeline
            .pColorBlendState = &colorBlending,                         // Binds our color masking/blending settings to the pipeline
            .pDynamicState = &dynamicState,                             // Tells the GPU which parts of the pipeline are allowed the change without a full pipeline rebuild (dynamic rendering)
            .layout = pipelineLayout,                                   // Binds the pipeline info
            .renderPass = nullptr                                       // Using dynamic rendering, so traditional render pass is not needed; skipping
        },
        // New struct (PipelineRenderingCreateInfo)
        {
            .colorAttachmentCount = 1,                                  // Expect exactly one framebuffer to draw onto
            .pColorAttachmentFormats = &swapchainSurfaceFormat.format   // Tells the pipeline the pixel format that the image will use; matcht to Swapchain (RGBA)
        }
    };

    // Pass the defined pipeline info into a Vulkan Pipeline object and tie it to our logicalDevice
    graphicsPipeline = vk::raii::Pipeline(
        logicalDevice,
        nullptr, // References an optional PipelineCache object; used to store and reuse data relevant to pipeline creation across multiple calls
        pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>()
    );
}