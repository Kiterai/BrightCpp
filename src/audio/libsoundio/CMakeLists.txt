target_sources(brightcpp PRIVATE
  audio.cpp
)

# libsoundio
# NOTE: temporary fix for latest macOS and iOS
# NOTE: temporary fix for high cpu usage
FetchContent_Declare(
  libsoundio
  GIT_REPOSITORY https://github.com/Kiterai/libsoundio.git
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
  PROPERTY VS_USER_PROPS "${CMAKE_CURRENT_SOURCE_DIR}/CompileAsCpp.props"
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

find_package(PulseAudio)
if(PULSEAUDIO_FOUND)
  message(STATUS "PulseAudio Found")
  cmake_policy(SET CMP0079 NEW)
  target_link_libraries(libsoundio_static PRIVATE ${PULSEAUDIO_LIBRARY})
endif()
