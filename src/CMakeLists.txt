# for embed shader code
include(FetchContent)
FetchContent_Declare(
  battery-embed
  GIT_REPOSITORY https://github.com/batterycenter/embed.git
  GIT_TAG        v1.2.16
)
FetchContent_MakeAvailable(battery-embed)

execute_process(COMMAND glslc ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.vert -o ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.vert.spv)
execute_process(COMMAND glslc ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.frag -o ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.frag.spv)
add_library(
    brightcpp
    window.cpp
    system.cpp
    glfw.cpp
    vulkan.cpp
)
target_include_directories(brightcpp PUBLIC ../include)
b_embed(brightcpp shaders/shader.vert.spv)
b_embed(brightcpp shaders/shader.frag.spv)

set_target_properties(brightcpp PROPERTIES CXX_STANDARD 20)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(brightcpp PRIVATE glfw)
find_package(Vulkan REQUIRED)
target_link_libraries(brightcpp PRIVATE Vulkan::Vulkan)
