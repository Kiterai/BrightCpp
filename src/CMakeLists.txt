add_library(
    brightcpp
    window.cpp
)
target_include_directories(brightcpp PUBLIC ../include)

set_target_properties(brightcpp PROPERTIES CXX_STANDARD 20)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(brightcpp PRIVATE glfw)
