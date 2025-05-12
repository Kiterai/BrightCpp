# BrightCpp

A lightweight and cross-platform game/tools development library for C++

- works on Windows, macOS, Linux
- never takes over your `main()` function
- no need for an explicit initialization function call like `library_init()`
- error handling is primarily performed using exceptions rather than return values (following the conventions of C++ standard libraries)
- supports multi-window application development

## Getting Started

### Prerequisites

- [CMake](https://cmake.org/) is required to build & install.
- [Vulkan SDK](https://vulkan.lunarg.com/) is required to install.
    - on macOS, you need to run `source setup-env.sh` to load environment variables.

### Your first code

This is the simplest code to use BrightCpp.

```cpp
#include <brightcpp/brightcpp.hpp>
#include <iostream>

int main() {
    try {
        bgt::window wnd;

        while (bgt::frame_update()) {
            
        }
    } catch (std::exception &e) {
        std::cerr << "error: " << e.what() << std::endl;
    }
}
```

Since BrightCpp uses exceptions to handle errors, we recommend you to wrap whole code in `try` block if you aren't developping throwaway codes.

### Use by FetchContent(CMake)

BrightCpp can be load by FetchContent in CMake.

```cmake
include(FetchContent)

FetchContent_Declare(
  brightcpp
  GIT_REPOSITORY https://github.com/Kiterai/BrightCpp.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(brightcpp)
```

then, you can easily link:

```cmake
add_executable(app main.cpp)
target_link_libraries(app PRIVATE brightcpp)
```

## Build

### dependency

- CMake
- Vulkan SDK >= 1.3.283
- X11 development package(only on Linux)

### Command

```
git clone https://github.com/Kiterai/BrightCpp.git
cd BrightCpp
cmake . -B build
cmake --build ./build
```
