target_sources(brightcpp PRIVATE
    vulkan/graphics.cpp
    vulkan/util.cpp
    vulkan/vma_impl.cpp
    vulkan/render_target.cpp
    vulkan/renderer2d.cpp
    vulkan/texture.cpp
    vulkan/vbuffer.cpp
)

message(STATUS "Compiling shader.vert...")
execute_process(COMMAND
  $ENV{VULKAN_SDK}/bin/glslc
  ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.vert
  -o ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.vert.spv
  RESULT_VARIABLE result
)
if(NOT result EQUAL "0")
    message( FATAL_ERROR "Failed to compile shader.vert")
endif()

message(STATUS "Compiling shader.frag...")
execute_process(COMMAND
  $ENV{VULKAN_SDK}/bin/glslc
  ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.frag
  -o ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.frag.spv
  RESULT_VARIABLE result
)
if(NOT result EQUAL "0")
    message( FATAL_ERROR "Failed to compile shader.frag")
endif()

b_embed(brightcpp shaders/shader.vert.spv)
b_embed(brightcpp shaders/shader.frag.spv)

# VMA
FetchContent_Declare(
  VulkanMemoryAllocator
  GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
  GIT_TAG v3.1.0)
FetchContent_MakeAvailable(VulkanMemoryAllocator)
target_link_libraries(brightcpp PRIVATE VulkanMemoryAllocator)

# VMA-Hpp
FetchContent_Populate(
  VulkanMemoryAllocator-Hpp
  GIT_REPOSITORY https://github.com/YaaZ/VulkanMemoryAllocator-Hpp.git
  GIT_TAG v3.1.0)
target_include_directories(brightcpp PRIVATE ${vulkanmemoryallocator-hpp_SOURCE_DIR}/include)

# stb
FetchContent_Populate(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb.git)
target_include_directories(brightcpp PRIVATE ${stb_SOURCE_DIR})

# Vulkan
if(IOS)
## on IOS
message(STATUS "iOS platform")

target_sources(brightcpp INTERFACE $ENV{VULKAN_SDK}/share/vulkan)
set_source_files_properties(Assets
   $ENV{VULKAN_SDK}/share/vulkan
   PROPERTIES
   MACOSX_PACKAGE_LOCATION Resources
)

# for loading cmake/FindVulkan.cmake
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(Vulkan REQUIRED COMPONENTS MoltenVK)
target_link_libraries(brightcpp PRIVATE
  "-framework UIKit"
  "-framework QuartzCore"
)
target_link_libraries(brightcpp PRIVATE Vulkan::Vulkan)
target_link_libraries(brightcpp PUBLIC
  "-framework Foundation"
)

else()
## on other platforms
find_package(Vulkan REQUIRED)
target_include_directories(brightcpp PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(brightcpp PRIVATE ${Vulkan_LIBRARIES})
endif()
