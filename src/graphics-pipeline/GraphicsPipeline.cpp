#include <vulkan-tutorial/graphics-pipeline/GraphicsPipeline.hpp>

// Standard C++ Libraries
#include <stdexcept>
#include <fstream>
#include <vector>
#include <string>
#include <format>

// Local helper functions
namespace {
    static std::vector<char> readFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error(std::format("failed to open file {}!", filename));
        }

        std::vector<char> buffer(file.tellg());

        file.seekg(0, std::ios::beg);
        file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));

        file.close();
        return buffer;
    }

    [[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char>& code, const vk::raii::Device &logicalDevice) {
        // Create the info about the loaded shader module
        vk::ShaderModuleCreateInfo createInfo{
            .codeSize = code.size() *sizeof(char),
            .pCode = reinterpret_cast<const uint32_t*>(code.data()) // Need to reinterpret as a char pointer
        };

        vk::raii::ShaderModule shaderModule{
            logicalDevice,
            createInfo
        };

        return shaderModule;
    }
}



/***** CONSTRUCTOR AND DESTRUCTOR *****/
GraphicsPipeline::GraphicsPipeline(
    const vk::raii::Device &logicalDevice,
    const vk::Extent2D &swapChainExtent
) {
    createGraphicsPipeline(logicalDevice, swapChainExtent);
}

GraphicsPipeline::~GraphicsPipeline() {}



/**** PUBLIC METHODS *****/



/**** Private METHODS *****/
void GraphicsPipeline::createGraphicsPipeline(
    const vk::raii::Device &logicalDevice,
    const vk::Extent2D &swapChainExtent
) {
    // STEP #1: Define the shader modules
    // Load shader code file
    auto shaderCode = readFile("build/shaders/slang.spv");

    // Create shader module
    vk::raii::ShaderModule shaderModule = createShaderModule(shaderCode, logicalDevice);

    // Assign the vertex shader to a specific pipeline stage
    vk::PipelineShaderStageCreateInfo vertShaderStageInfo{
        .stage = vk::ShaderStageFlagBits::eVertex,
        .module = shaderModule,
        .pName = "vertMain" // The entrypoint function to use
    };

    // Assign the fragment shader to a specific pipeline stage
    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{
        .stage = vk::ShaderStageFlagBits::eFragment,
        .module = shaderModule,
        .pName = "fragMain"
    };

    // Create an array containing all shader structs
    vk::PipelineShaderStageCreateInfo shaderStages[] = {
        vertShaderStageInfo,
        fragShaderStageInfo
    };


    // STEP 2: Define the dynamic pipeline states
    std::vector<vk::DynamicState> dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    vk::PipelineDynamicStateCreateInfo dynamicState{
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data()
    };


    // STEP #3: Define the fixed function pipeline states
    // Describe the format of the vertex data that will be passed to the vertex shader
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo; // Ignore for now as we aren't using a true vertex buffer

    // Define the input assembly (what kind of geometry will be drawn and if primitive restart should be enabled)
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{
        .topology = vk::PrimitiveTopology::eTriangleList, // Triangle from every three vertices without reuse of vertices
    };

    // Define the viewport (the region of the framebuffer that the output will be rendered to)
    // & scissor rectangles (the region within which pixels will actually be stored; i.e., what the rasterizer should not cut)
    // Going to draw to the entire framebuffer; no reason not to atm
    vk::Viewport viewport{0.0f, 0.0f, static_cast<float>(swapChainExtent.width), static_cast<float>(swapChainExtent.height), 0.0f, 1.0f};
    vk::Rect2D scissor{
        vk::Offset2D{0, 0},
        swapChainExtent
    };
    vk::PipelineViewportStateCreateInfo viewportState{
        .viewportCount = 1,
        .scissorCount = 1
    };

    // Create the rasterizer
    vk::PipelineRasterizationStateCreateInfo rasterizer{
        .depthClampEnable = vk::False,                  // If true, fragments beyond near and far planes are clamped instead of discarded
        .rasterizerDiscardEnable = vk::False,           // If true, then geometry never passes through the rasterizer; disables all output
        .polygonMode = vk::PolygonMode::eFill,          // Determines how fragemtns are generated for geometry
        .cullMode = vk::CullModeFlagBits::eBack,        // Determines the type of face culling to use
        .frontFace = vk::FrontFace::eClockwise,         // Specifies the vertex order for the faces to be considered front-facing
        .depthBiasEnable = vk::False,                   // Sometimes used for shadow mapping; ignore for now
        .lineWidth = 1.0f                               // Describes the thickness of lines in terms of number of fragments
    };

    // Configure multisampling (useful for performing anti-aliasing)
    // Keeping disabled for now
    vk::PipelineMultisampleStateCreateInfo multisampling{
        .rasterizationSamples = vk::SampleCountFlagBits::e1,
        .sampleShadingEnable = vk::False
    };

    // Combine the returned color from the fragment shader with the color already in the framebuffer
    vk::PipelineColorBlendAttachmentState colorBlendAttachment{
        .blendEnable = vk::False,                           // If false, new color from fragment shader is passed through unmodified
        .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
    };

    // Define the pipeline layout
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
        .setLayoutCount = 0,
        .pushConstantRangeCount = 0
    };
    pipelineLayout = vk::raii::PipelineLayout(logicalDevice, pipelineLayoutInfo);
}