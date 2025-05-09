include(FetchContent)

add_library(
    brightcpp
    window.cpp
    system.cpp

    image.cpp
    render_target.cpp
    renderer2d.cpp

    glfw/glfw.cpp

    key/key.cpp
    key/key_windows.cpp
    key/key_macos.cpp
    key/key_x11.cpp

    vulkan/graphics.cpp
    vulkan/util.cpp
    vulkan/vma_impl.cpp
    vulkan/render_target.cpp
    vulkan/renderer2d.cpp
    vulkan/texture.cpp

    audio.cpp
    oneshot_audio.cpp
    audio/audio_asset_manager.cpp
    audio/streaming_audio_asset_manager.cpp
    audio/audio_player_manager.cpp
    audio/resample.cpp
    audio/mixer.cpp
    audio/streaming_manager.cpp
    audio/player/player_normal.cpp
    audio/player/player_streaming.cpp
    audio/loader/loader_selector.cpp
    audio/loader/wav.cpp
    audio/loader/ogg.cpp
    audio/loader/mp3.cpp

    libsoundio/audio.cpp
)
target_include_directories(brightcpp PUBLIC ../include)

# embed
set(B_PRODUCTION_MODE ON)
FetchContent_Declare(
  battery-embed
  GIT_REPOSITORY https://github.com/batterycenter/embed.git
  GIT_TAG        v1.2.16
)
FetchContent_MakeAvailable(battery-embed)

execute_process(COMMAND glslc ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.vert -o ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.vert.spv)
execute_process(COMMAND glslc ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.frag -o ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.frag.spv)
b_embed(brightcpp shaders/shader.vert.spv)
b_embed(brightcpp shaders/shader.frag.spv)

# GLFW
if(NOT IOS)
message(STATUS "GLFW enabled")
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

# libsoundio
# NOTE: temporary fix for latest macOS
FetchContent_Declare(
  libsoundio
  GIT_REPOSITORY https://github.com/harukasan/libsoundio.git
  GIT_TAG fix/build_macos_14_0
)
# FetchContent_Declare(
#   libsoundio
#   GIT_REPOSITORY https://github.com/andrewrk/libsoundio.git
#   # GIT_TAG 2.0.0
# )
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
if(IOS)
# -Wno-implicit-int-conversion
endif()
set(BUILD_EXAMPLE_PROGRAMS "${BUILD_EXAMPLE_PROGRAMS_BK}")
set(BUILD_TESTS "${BUILD_TESTS_BK}")
set(BUILD_DYNAMIC_LIBS "${BUILD_DYNAMIC_LIBS_BK}")
target_link_libraries(brightcpp PRIVATE libsoundio_static)
target_include_directories(brightcpp PRIVATE ${libsoundio_SOURCE_DIR})

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
target_link_libraries(brightcpp PRIVATE samplerate)

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
target_link_libraries(brightcpp PRIVATE vorbisfile)
target_include_directories(brightcpp PRIVATE ${vorbis_SOURCE_DIR}/include)

# minimp3
FetchContent_Populate(
  minimp3
  GIT_REPOSITORY https://github.com/lieff/minimp3.git)
target_include_directories(brightcpp PRIVATE ${minimp3_SOURCE_DIR})

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
message(STATUS "iOS platform")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(Vulkan REQUIRED COMPONENTS MoltenVK)
target_link_libraries(brightcpp PRIVATE
  "-framework UIKit"
  "-framework QuartzCore"
)
target_link_libraries(brightcpp PUBLIC Vulkan::Vulkan)
target_link_options(brightcpp PRIVATE -rpath @executable_path/Frameworks)
else()
find_package(Vulkan REQUIRED)
target_include_directories(brightcpp PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(brightcpp PRIVATE ${Vulkan_LIBRARIES})
endif()

find_package(X11)
if(X11_FOUND)
  message(STATUS "X11 Found")
  target_link_libraries(brightcpp PRIVATE ${X11_LIBRARIES})
endif()

find_package(PulseAudio)
if(PULSEAUDIO_FOUND)
  message(STATUS "PulseAudio Found")
  cmake_policy(SET CMP0079 NEW)
  target_link_libraries(libsoundio_static PRIVATE ${PULSEAUDIO_LIBRARY})
endif()

set_target_properties(brightcpp PROPERTIES CXX_STANDARD 20)
