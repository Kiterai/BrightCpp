# for embed shader code
include(FetchContent)
FetchContent_Declare(
  battery-embed
  GIT_REPOSITORY https://github.com/batterycenter/embed.git
  GIT_TAG        v1.2.16
)
FetchContent_MakeAvailable(battery-embed)

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4)
FetchContent_MakeAvailable(glfw)

execute_process(COMMAND glslc ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.vert -o ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.vert.spv)
execute_process(COMMAND glslc ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.frag -o ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.frag.spv)
add_library(
    brightcpp
    window.cpp
    system.cpp
    glfw.cpp

    vulkan.cpp
    vulkan/common.cpp
    vulkan/render_target.cpp
    vulkan/render_proc.cpp
)
target_include_directories(brightcpp PUBLIC ../include)
b_embed(brightcpp shaders/shader.vert.spv)
b_embed(brightcpp shaders/shader.frag.spv)

find_package(Vulkan REQUIRED)
target_include_directories(brightcpp PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(brightcpp PRIVATE ${Vulkan_LIBRARIES})

set_target_properties(brightcpp PROPERTIES CXX_STANDARD 20)
target_link_libraries(brightcpp PRIVATE glfw)
