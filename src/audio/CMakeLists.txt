target_sources(brightcpp PRIVATE
  audio.cpp
  oneshot_audio.cpp

  audio_asset_manager.cpp
  streaming_audio_asset_manager.cpp
  audio_player_manager.cpp
  resample.cpp
  mixer.cpp
  streaming_manager.cpp
)
add_subdirectory(libsoundio)
add_subdirectory(loader)
add_subdirectory(player)

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
