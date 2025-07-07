# GLFW
if(NOT IOS)
message(STATUS "GLFW enabled")
target_sources(brightcpp PRIVATE glfw.cpp)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
if(UNIX)
set(GLFW_BUILD_X11 ON)
set(GLFW_BUILD_WAYLAND OFF)
endif()
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4)
FetchContent_MakeAvailable(glfw)
target_link_libraries(brightcpp PRIVATE glfw)
else()
message(STATUS "GLFW disabled")
endif()
