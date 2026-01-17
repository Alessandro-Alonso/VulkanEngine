#include "vulkan/vulkan_core.h"
#include "RenderError.h"

#include <sstream>
#include <string>
#include <vulkan/vulkan.h>

// If any code gives and error, it will execute this. Finds the type of error, the file, the line and tells the guy or woman. The ONLY TWO genders.
std::string GetVkResultString(VkResult result);

namespace NETAEngine {

    [[noreturn]] void logAndAbort(const RenderResult& result);
    const char*       getRenderErrorName(RenderErrorType type);

    // Esto es la misma macro, pero para calls que nunca deberia de dar fallos si son correctos.
    // Si lo hace, es un error de programacion. Abortamos inmediatamente con un contexto.
    // Deberia de usar esto haciendo inicializaciones: creando devices, pipelines, render passes, etc...
    #define VK_CHECK(call)                                                              \
        do {                                                                            \
            VkResult _result = (call);                                                  \
            if (_result != VK_SUCCESS) {                                                \
                std::cerr << "\n[FATAL Vulkan Error]\n"                                 \
                          << "  Call   : " << #call << "\n"                             \
                          << "  Result : " << GetVkResultString(_result) << "\n"        \
                          << "  File   : " << __FILE__ << ":" << __LINE__ << "\n";      \
                std::abort();                                                           \
            }                                                                           \
        } while(0)

    #define VK_CALL(call) \
        NETAEngine::categorize((call), #call, __FILE__, __LINE__)

    // This checks general errors
    // To do: Voy a necesitar cambiar esto en el futuro.
    // En verdad lo dejo asi. Me suda la polla el check general.
    #define GE_CHECK(condition, message) \
        do { \
            if (!(condition)) { \
                std::ostringstream oss; \
                oss << (message) << " at " << __FILE__ << ":" << __LINE__; \
                throw std::runtime_error(oss.str()); \
            } \
        } while (0)

}