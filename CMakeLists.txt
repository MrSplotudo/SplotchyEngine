cmake_minimum_required(VERSION 3.20)
project(SplotchyEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)

include(FetchContent)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(tinyobjloader
        GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
        GIT_TAG v2.0.0rc13)
FetchContent_MakeAvailable(tinyobjloader)

file(GLOB_RECURSE SOURCES "src/*.cpp" "engine/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_definitions(${PROJECT_NAME} PRIVATE
        PROJECT_ROOT="${CMAKE_SOURCE_DIR}")

set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan glfw)
target_include_directories(${PROJECT_NAME} PRIVATE
        ${glm_SOURCE_DIR}
        ${tinyobjloader_SOURCE_DIR})

# Auto-compile shaders
find_program(GLSLC glslc HINTS $ENV{VULKAN_SDK}/bin REQUIRED)

file(GLOB SHADER_SOURCES
        "${CMAKE_SOURCE_DIR}/shaders/*.vert"
        "${CMAKE_SOURCE_DIR}/shaders/*.frag")

set(SPV_FILES)
foreach(SHADER ${SHADER_SOURCES})
    set(OUT ${SHADER}.spv)
    add_custom_command(
            OUTPUT ${OUT}
            COMMAND ${GLSLC} ${SHADER} -o ${OUT}
            DEPENDS ${SHADER}
            VERBATIM)
    list(APPEND SPV_FILES ${OUT})
endforeach()

add_custom_target(shaders DEPENDS ${SPV_FILES})
add_dependencies(${PROJECT_NAME} shaders)