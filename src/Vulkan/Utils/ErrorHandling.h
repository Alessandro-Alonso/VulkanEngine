#pragma once

#include <string>
#include <vulkan/vulkan.h>
#include <sstream>

// If any code gives and error, it will execute this. Finds the type of error, the file, the line and tells the guy or woman.
std::string GetVkResultString(VkResult result);

// This checks vulkan errors
#define VK_CHECK(x) \
    do { \
        VkResult res = (x); \
        if (res != VK_SUCCESS) { \
            throw std::runtime_error( \
                "Vulkan error: " + GetVkResultString(res) + \
                " at " + __FILE__ + ":" + std::to_string(__LINE__) \
            ); \
        } \
    } while (0)

// This checks general errors
#define GE_CHECK(condition, message) \
    do { \
        if (!(condition)) { \
            std::ostringstream oss; \
            oss << (message) << " at " << __FILE__ << ":" << __LINE__; \
            throw std::runtime_error(oss.str()); \
        } \
    } while (0)