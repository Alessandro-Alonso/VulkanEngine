#include "ErrorHandling.h"
#include "Vulkan/Utils/RenderError.h"
#include <iostream>

[[nodiscard]] std::string GetVkResultString(VkResult result) {
    switch (result) {
        case VK_SUCCESS:                     return "VK_SUCCESS";
        case VK_NOT_READY:                   return "VK_NOT_READY";
        case VK_TIMEOUT:                     return "VK_TIMEOUT";
        case VK_ERROR_OUT_OF_HOST_MEMORY:    return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:  return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST:           return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_LAYER_NOT_PRESENT:     return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT:   return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER:   return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_OUT_OF_DATE_KHR:       return "VK_ERROR_OUT_OF_DATE_KHR";
        case VK_SUBOPTIMAL_KHR:              return "VK_SUBOPTIMAL_KHR";
        case VK_ERROR_SURFACE_LOST_KHR:      return "VK_ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_UNKNOWN:               return "VK_ERROR_UNKNOWN";
        default:
            return "VK_UNKNOWN (" + std::to_string(static_cast<int32_t>(result)) + ")";
    }
}

namespace NETAEngine {

    const char* getRenderErrorName(RenderErrorType type) {
        switch (type) {
            case RenderErrorType::SwapChainOutOfDate:     return "SwapChainOutOfDate";
            case RenderErrorType::SwapChainSuboptimal:    return "SwapChainSuboptimal";
            case RenderErrorType::OutOfDeviceMemory:      return "OutOfDeviceMemory";
            case RenderErrorType::OutOfHostMemory:        return "OutOfHostMemory";
            case RenderErrorType::DeviceLost:             return "DeviceLost";
            case RenderErrorType::SurfaceLost:            return "SurfaceLost";
            case RenderErrorType::InitializationFailed:   return "InitializationFailed";
            case RenderErrorType::InvalidHandle:          return "InvalidHandle";
            case RenderErrorType::ValidationFailed:       return "ValidationFailed";
            default:                                      return "Unknown";
        }
    }

    [[noreturn]] void logAndAbort(const RenderResult& result) {
        std::cerr << "\n========================================\n"
                  << "[FATAL RENDER ERROR]\n"
                  << "  Type   : " << getRenderErrorName(result.type) << "\n"
                  << "  Vulkan : " << GetVkResultString(result.vkResult) << "\n"
                  << "  Call   : " << (result.callSite ? result.callSite : "unknown") << "\n"
                  << "  File   : " << (result.file ? result.file : "unknown")
                  <<           ":" << result.line << "\n"
                  << "========================================\n";

        std::abort();
    }

}