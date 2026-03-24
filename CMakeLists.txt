cmake_minimum_required(VERSION 3.20)
project(SplotchyEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

add_executable(${PROJECT_NAME}
        src/main.cpp
)

target_link_libraries(${PROJECT_NAME}
        Vulkan::Vulkan
        glfw
)

target_include_directories(${PROJECT_NAME} PRIVATE
        "external/glm"
)