#pragma once

#include "vulkan/vulkan_core.h"

namespace NETAEngine {

    // Esto son las categorias de las cosas que pueden ir mal al nivel de Vulkan.
    // La diferencia es importante porque cada una requiere una respuesta distinta.
    // Cuanto mas detallado me de el error, mas facil va a ser arreglarlo. 
    // PD: Me cago en los putisimos muertos del error: "symbol(s) not found for architecture arm64".
    // Que putisimo asco de puto error basura de mierda que no me da informacion de nada.
    enum class RenderErrorType {
        
        SwapChainOutOfDate,   // El resize normal/minimizar
        SwapChainSuboptimal,  // Funcinoa pero deberia de recrearlo pronto

        OutOfDeviceMemory,    // GPU VRAM sin memoria
        OutOfHostMemory,      // Sistema RAM sin memoria

        DeviceLost,           // La GPU ha crasheado
        SurfaceLost,          // La ventana se ha destruido mientras se estaba usando
        InitializationFailed, // Drivers/Hardware ha fallado

        InvalidHandle,        // Null or destroy handle ha pasado
        ValidationFailed,     // Vulkan validacion de layers ha encontrado un bug

        Unknown               // No tiene ni puta idea de que es el error.
    };

    struct RenderResult {
        RenderErrorType type;
        VkResult        vkResult;
        const char*     callSite;
        const char*     file;
        int             line;

        [[nodiscard]] bool ok()     const { return type == RenderErrorType::SwapChainOutOfDate ? false : vkResult == VK_SUCCESS; }
        [[nodiscard]] bool isRecoverable() const {
            return type == RenderErrorType::SwapChainOutOfDate ||
                   type == RenderErrorType::SwapChainSuboptimal;
        }
        [[nodiscard]] bool isFatal() const {
            return type == RenderErrorType::DeviceLost ||
                   type == RenderErrorType::SurfaceLost ||
                   type == RenderErrorType::InitializationFailed;
        }

        static RenderResult success() {
            return { RenderErrorType::Unknown, VK_SUCCESS, nullptr, nullptr, 0};
        }
    };

    inline RenderResult categorize(VkResult result, const char* call, const char* file, int line) {
        RenderErrorType type;

        switch (result) {
            case VK_SUCCESS:                        return RenderResult::success();
            case VK_SUBOPTIMAL_KHR:                 type = RenderErrorType::SwapChainSuboptimal;    break;
            case VK_ERROR_OUT_OF_DATE_KHR:          type = RenderErrorType::SwapChainOutOfDate;     break;
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:     type = RenderErrorType::OutOfDeviceMemory;      break;
            case VK_ERROR_OUT_OF_HOST_MEMORY:       type = RenderErrorType::OutOfHostMemory;        break;
            case VK_ERROR_DEVICE_LOST:              type = RenderErrorType::DeviceLost;             break;
            case VK_ERROR_SURFACE_LOST_KHR:         type = RenderErrorType::SurfaceLost;            break;
            case VK_ERROR_INITIALIZATION_FAILED:    type = RenderErrorType::InitializationFailed;   break;
            default:                                type = RenderErrorType::Unknown;   
        }

        return { type, result, call, file, line };
    }
}