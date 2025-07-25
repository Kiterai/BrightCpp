target_sources(brightcpp PRIVATE
    graphics.cpp
    util.cpp
    vma_impl.cpp
    rendertarget_factory.cpp
    window_rendertarget.cpp
    texture_rendertarget.cpp
    renderer2d.cpp
    texture.cpp
    vbuffer.cpp
)

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
