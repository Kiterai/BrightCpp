include(FetchContent)

# embed
set(B_PRODUCTION_MODE ON)
FetchContent_Declare(
  battery-embed
  GIT_REPOSITORY https://github.com/batterycenter/embed.git
  GIT_TAG        v1.2.19
)
FetchContent_MakeAvailable(battery-embed)

# target
add_library(
    brightcpp
    system.cpp
)
install(TARGETS brightcpp
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
add_subdirectory(audio)
add_subdirectory(graphics)
add_subdirectory(os)

message(STATUS "Compiling shader.vert...")
add_custom_command(
  COMMAND $ENV{VULKAN_SDK}/bin/glslang
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics/shaders/shader.vert
    -V
    -o ${CMAKE_CURRENT_SOURCE_DIR}/graphics/shaders/shader.vert.spv
  DEPENDS graphics/shaders/shader.vert
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/graphics/shaders/shader.vert.spv
)
b_embed(brightcpp graphics/shaders/shader.vert.spv)

message(STATUS "Compiling shader.frag...")
add_custom_command(
  COMMAND $ENV{VULKAN_SDK}/bin/glslang
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics/shaders/shader.frag
    -V
    -o ${CMAKE_CURRENT_SOURCE_DIR}/graphics/shaders/shader.frag.spv
  DEPENDS graphics/shaders/shader.frag
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/graphics/shaders/shader.frag.spv
)
b_embed(brightcpp graphics/shaders/shader.frag.spv)

target_include_directories(brightcpp PUBLIC ../include)

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
