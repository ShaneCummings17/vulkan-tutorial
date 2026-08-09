# Project definitions
cmake_minimum_required(VERSION 3.25)
project(VulkanTutorial LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate compile_commands.json for VSCode IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# The executable target
add_executable(${PROJECT_NAME}
    main.cpp
    src/app/Application.cpp
    src/window/Window.cpp
    src/vulkan-api/Vulkan.cpp
    src/devices/PhysicalDevice.cpp
    src/devices/LogicalDevice.cpp
    src/swapchain/Swapchain.cpp
    src/graphics-pipeline/GraphicsPipeline.cpp
    src/commands/Commands.cpp
)

# External dependencies
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

# Internal headers
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE VULKAN_HPP_NO_STRUCT_CONSTRUCTORS)

# Link dependencies
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Vulkan::Vulkan
        glfw
)

# Shaders
find_program(SLANGC_EXECUTABLE slangc REQUIRED)
function(add_slang_shader_target TARGET)
    cmake_parse_arguments(
        SHADER
        ""
        ""
        "SOURCES"
        ${ARGN}
    )

    # Source shaders
    set(SHADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)

    # Compiled shaders
    set(SHADERS_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders)

    set(ENTRY_POINTS
        -entry vertMain
        -entry fragMain
    )

    add_custom_command(
        OUTPUT ${SHADERS_BINARY_DIR}/slang.spv
        COMMAND ${CMAKE_COMMAND} -E make_directory
                ${SHADERS_BINARY_DIR}
        COMMAND ${SLANGC_EXECUTABLE}
                ${SHADER_SOURCES}
                -target spirv
                -profile spirv_1_4
                -emit-spirv-directly
                -fvk-use-entrypoint-name
                ${ENTRY_POINTS}
                -o ${SHADERS_BINARY_DIR}/slang.spv
        DEPENDS ${SHADER_SOURCES}
        COMMENT "Compiling Slang shaders"
        VERBATIM
    )

    add_custom_target(
        ${TARGET}
        DEPENDS ${SHADERS_BINARY_DIR}/slang.spv
    )
endfunction()

add_slang_shader_target(Shaders
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/vertex-shader.slang
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fragment-shader.slang
)
add_dependencies(${PROJECT_NAME} Shaders)