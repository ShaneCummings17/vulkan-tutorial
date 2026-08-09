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