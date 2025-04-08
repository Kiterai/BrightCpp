add_executable(simple_app simple_app/main.cpp)
target_link_libraries(simple_app brightcpp)
set_target_properties(simple_app PROPERTIES CXX_STANDARD 11)

add_executable(practical_app practical_app/main.cpp)
target_link_libraries(practical_app brightcpp)
set_target_properties(practical_app PROPERTIES CXX_STANDARD 20)

add_executable(sample_game sample_game/main.cpp)
target_link_libraries(sample_game brightcpp)
set_target_properties(sample_game PROPERTIES CXX_STANDARD 20)
