# for embed shader code
include(FetchContent)
FetchContent_Declare(
  battery-embed
  GIT_REPOSITORY https://github.com/batterycenter/embed.git
  GIT_TAG        v1.2.16
)
FetchContent_MakeAvailable(battery-embed)

# GLFW
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4)
FetchContent_MakeAvailable(glfw)

# libsoundio
FetchContent_Declare(
  libsoundio
  GIT_REPOSITORY https://github.com/andrewrk/libsoundio.git
  # GIT_TAG 2.0.0
)
set(BUILD_EXAMPLE_PROGRAMS_BK "${BUILD_EXAMPLE_PROGRAMS}")
set(BUILD_EXAMPLE_PROGRAMS OFF CACHE INTERNAL "")
set(BUILD_TESTS_BK "${BUILD_TESTS}")
set(BUILD_TESTS OFF CACHE INTERNAL "")
set(BUILD_DYNAMIC_LIBS_BK "${BUILD_DYNAMIC_LIBS}")
set(BUILD_DYNAMIC_LIBS OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(libsoundio)
if(MSVC)
set_property(
  TARGET libsoundio_static
  PROPERTY VS_USER_PROPS "${CMAKE_CURRENT_SOURCE_DIR}/libsoundio/CompileAsCpp.props"
)
endif()
if(APPLE)
  cmake_policy(SET CMP0079 NEW)
  find_library(CORE_AUDIO CoreAudio REQUIRED)
  find_library(AUDIO_TOOLBOX AudioToolBox REQUIRED)
  target_link_libraries(
    libsoundio_static PRIVATE
    ${CORE_AUDIO}
    ${AUDIO_TOOLBOX}
  )
endif()
set(BUILD_EXAMPLE_PROGRAMS "${BUILD_EXAMPLE_PROGRAMS_BK}")
set(BUILD_TESTS "${BUILD_TESTS_BK}")
set(BUILD_DYNAMIC_LIBS "${BUILD_DYNAMIC_LIBS_BK}")

# muFFT
FetchContent_Declare(
  muFFT
  GIT_REPOSITORY https://github.com/Themaister/muFFT.git)
if(AARCH64)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
endif()
FetchContent_MakeAvailable(muFFT)

# libsamplerate
FetchContent_Declare(
  libsamplerate
  GIT_REPOSITORY https://github.com/libsndfile/libsamplerate.git
  GIT_TAG 0.2.2
)
FetchContent_MakeAvailable(libsamplerate)

# ogg
set(BUILD_TESTING_BK "${BUILD_TESTING}")
set(BUILD_TESTING OFF CACHE INTERNAL "")
FetchContent_Declare(
  ogg
  GIT_REPOSITORY https://github.com/xiph/ogg.git
  GIT_TAG v1.3.5
)
FetchContent_MakeAvailable(ogg)
set(BUILD_TESTING "${BUILD_TESTING_BK}")

# vorbis
FetchContent_Declare(
  vorbis
  GIT_REPOSITORY https://github.com/xiph/vorbis.git
  GIT_TAG v1.3.7
)
set(OGG_INCLUDE_DIR ${ogg_SOURCE_DIR}/include)
if(MSVC)
set(OGG_LIBRARY ${ogg_BINARY_DIR}/Release)
else()
set(OGG_LIBRARY ${ogg_BINARY_DIR})
endif()
FetchContent_MakeAvailable(vorbis)

# minimp3
FetchContent_Populate(
  minimp3
  GIT_REPOSITORY https://github.com/lieff/minimp3.git)

# VMA
FetchContent_Declare(
  VulkanMemoryAllocator
  GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
  GIT_TAG v3.1.0)
FetchContent_MakeAvailable(VulkanMemoryAllocator)

# VMA-Hpp
FetchContent_Populate(
  VulkanMemoryAllocator-Hpp
  GIT_REPOSITORY https://github.com/YaaZ/VulkanMemoryAllocator-Hpp.git
  GIT_TAG v3.1.0)

# stb
FetchContent_Populate(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb.git)

execute_process(COMMAND glslc ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.vert -o ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.vert.spv)
execute_process(COMMAND glslc ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.frag -o ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.frag.spv)
add_library(
    brightcpp
    window.cpp
    system.cpp

    image.cpp
    render_target.cpp
    renderer2d.cpp

    glfw/glfw.cpp

    vulkan/graphics.cpp
    vulkan/util.cpp
    vulkan/vma_impl.cpp
    vulkan/render_target.cpp
    vulkan/renderer2d.cpp
    vulkan/texture.cpp

    audio.cpp
    audio/player_normal.cpp
    audio/audio_asset_manager.cpp
    audio/resample.cpp
    audio/mixer.cpp
    audio/streaming_manager.cpp
    audio/loader/loader_selector.cpp
    audio/loader/wav.cpp
    audio/loader/ogg.cpp
    audio/loader/mp3.cpp

    libsoundio/audio.cpp
)
target_include_directories(brightcpp PUBLIC ../include)
b_embed(brightcpp shaders/shader.vert.spv)
b_embed(brightcpp shaders/shader.frag.spv)

find_package(Vulkan REQUIRED)
target_include_directories(brightcpp PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(brightcpp PRIVATE ${Vulkan_LIBRARIES})

set_target_properties(brightcpp PROPERTIES CXX_STANDARD 20)
target_link_libraries(brightcpp PRIVATE glfw)
target_link_libraries(brightcpp PRIVATE VulkanMemoryAllocator)
target_link_libraries(brightcpp PRIVATE samplerate)
target_link_libraries(brightcpp PRIVATE libsoundio_static)
target_include_directories(brightcpp PRIVATE ${libsoundio_SOURCE_DIR})
target_link_libraries(brightcpp PRIVATE vorbisfile)
target_include_directories(brightcpp PRIVATE ${vorbis_SOURCE_DIR}/include)
target_include_directories(brightcpp PRIVATE ${vulkanmemoryallocator-hpp_SOURCE_DIR}/include)
target_include_directories(brightcpp PRIVATE ${stb_SOURCE_DIR})
target_include_directories(brightcpp PRIVATE ${minimp3_SOURCE_DIR})
