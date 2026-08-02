# Project definitions
cmake_minimum_required(VERSION 3.25)
project(VulkanTutorial LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate compile_commands.json for VSCode IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# The executable target
add_executable(VulkanTutorial
    main.cpp
    src/app/Application.cpp
    src/window/Window.cpp
)

# External dependencies
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

# Internal headers
target_include_directories(VulkanTutorial PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link dependencies
target_link_libraries(VulkanTutorial
    PRIVATE
        Vulkan::Vulkan
        glfw
)