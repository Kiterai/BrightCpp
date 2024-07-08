#pragma once

#include <GLFW/glfw3.h>
#include <brightcpp/internal/system.hpp>
#include <memory>
#include <stdexcept>

#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define _LINE STRINGIZE(__LINE__)

#define BRIGHTCPP_GLFW_CHK_ERR(call)                                                                          \
    {                                                                                                         \
        call;                                                                                                 \
        if (const char *buf; glfwGetError(&buf) != GLFW_NO_ERROR) {                                           \
            throw std::runtime_error("error on " __FILE__ " line " _LINE ", " #call ": " + std::string(buf)); \
        }                                                                                                     \
    }

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

void setup_glfw();
void shutdown_glfw();

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE
