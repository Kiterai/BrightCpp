include(FetchContent)

# embed
set(B_PRODUCTION_MODE ON)
FetchContent_Declare(
  battery-embed
  GIT_REPOSITORY https://github.com/batterycenter/embed.git
  GIT_TAG        v1.2.16
)
FetchContent_MakeAvailable(battery-embed)

# target
add_library(
    brightcpp
    window.cpp
    system.cpp

    image.cpp
    render_target.cpp
    renderer2d.cpp

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

    audio/libsoundio/audio.cpp
)
add_subdirectory(graphics)
add_subdirectory(os)

target_include_directories(brightcpp PUBLIC ../include)

# libsoundio
# NOTE: temporary fix for latest macOS and iOS
FetchContent_Declare(
  libsoundio
  GIT_REPOSITORY https://github.com/Kiterai/libsoundio.git
  GIT_TAG ios-build
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
  PROPERTY VS_USER_PROPS "${CMAKE_CURRENT_SOURCE_DIR}/audio/libsoundio/CompileAsCpp.props"
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
set(BUILD_TESTING_BK "${BUILD_TESTING}")
set(BUILD_TESTING OFF CACHE INTERNAL "")
FetchContent_Declare(
  libsamplerate
  GIT_REPOSITORY https://github.com/libsndfile/libsamplerate.git
  GIT_TAG 0.2.2
)
FetchContent_MakeAvailable(libsamplerate)
target_link_libraries(brightcpp PRIVATE samplerate)
set(BUILD_TESTING "${BUILD_TESTING_BK}")

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

find_package(PulseAudio)
if(PULSEAUDIO_FOUND)
  message(STATUS "PulseAudio Found")
  cmake_policy(SET CMP0079 NEW)
  target_link_libraries(libsoundio_static PRIVATE ${PULSEAUDIO_LIBRARY})
endif()

set_target_properties(brightcpp PROPERTIES CXX_STANDARD 20)

function(brightcpp_ios_declare_app app_target app_name app_id)
message(STATUS "iOS Application setting launched")
message(STATUS "  App Name: ${app_name}")
message(STATUS "  Bundle ID: ${app_id}")
set_target_properties(${app_target} PROPERTIES
  MACOSX_BUNDLE_BUNDLE_NAME ${app_name}
  MACOSX_BUNDLE_GUI_IDENTIFIER ${app_id}
    
  XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "YES"
  XCODE_ATTRIBUTE_CLANG_ENABLE_MODULES "YES"
  XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@executable_path/Frameworks"
  XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Automatic" # already default value
  XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
  XCODE_EMBED_FRAMEWORKS "${Vulkan_MoltenVK_LIBRARY};${Vulkan_LIBRARIES};${Vulkan_Layer_VALIDATION};"
  XCODE_EMBED_FRAMEWORKS_CODE_SIGN_ON_COPY		"YES"
  XCODE_EMBED_FRAMEWORKS_REMOVE_HEADERS_ON_COPY	"YES"
  XCODE_ATTRIBUTE_SKIP_INSTALL NO
  XCODE_ATTRIBUTE_INSTALL_PATH "$(LOCAL_APPS_DIR)"
  XCODE_ATTRIBUTE_DEAD_CODE_STRIPPING NO
  XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"

  OSX_ARCHITECTURES arm64
  XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH YES
#  RESOURCE "${RESOURCE_FILES}"
)
endfunction()
function(brightcpp_ios_set_version_info app_version app_version_short)
set_target_properties(${app_target} PROPERTIES
  MACOSX_BUNDLE_SHORT_VERSION_STRING ${app_version}
  MACOSX_BUNDLE_BUNDLE_VERSION ${app_version_short}
)
endfunction()
function(brightcpp_ios_set_icon app_icon)
set_target_properties(${app_target} PROPERTIES
  XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME ${app_icon}
)
endfunction()
