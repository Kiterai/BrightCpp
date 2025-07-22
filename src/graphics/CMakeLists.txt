target_sources(brightcpp PRIVATE
    image.cpp
    render_target.cpp
    renderer2d.cpp
    renderer2d_middleware.cpp
)
add_subdirectory(vulkan)

# stb
FetchContent_Populate(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb.git)
target_include_directories(brightcpp PRIVATE ${stb_SOURCE_DIR})
