# BrightCpp

A lightweight and cross-platform game/tools development library for C++

- works on Windows, macOS, Linux
- never takes over your `main()` function
- no need for an explicit initialization function call like `library_init()`
- error handling is primarily performed using exceptions rather than return values (following the conventions of C++ standard libraries)
- supports multi-window application development

## Getting Started

### Use by FetchContent(CMake)

### Use by manual link(CMake)

### Use by manual link(GCC)

### Use by manual link(Visual Studio)

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
