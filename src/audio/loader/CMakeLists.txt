target_sources(brightcpp PRIVATE
  loader_selector.cpp
  wav.cpp
  ogg.cpp
  mp3.cpp
)

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
