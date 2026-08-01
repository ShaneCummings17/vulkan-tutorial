cmake_minimum_required(VERSION 3.25)

project(VulkanTutorial LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(VulkanTutorial main.cpp)

find_package(Vulkan ${VCPKG_VULKAN_VERSION} ${VCPKG_VULKAN_COMPONENTS} REQUIRED)
if(NOT Vulkan_FOUND)
    set(CMAKE_FIND_DEBUG_MODE ON)
    find_package(Vulkan ${VCPKG_VULKAN_VERSION} ${VCPKG_VULKAN_COMPONENTS})
endif()
find_package(glfw3 CONFIG REQUIRED)

target_link_libraries(VulkanTutorial
    PRIVATE
    Vulkan::Vulkan
    glfw
)