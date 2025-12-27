// This whole file is written by AI. I dind't want to waste my time writting this, and focus more on the internal parts of vulkan.
// Este archivo entero fue escrito por la AI. No queria tirar mi tiempo escribiendo esto. Queria meterle mas focus a las partes 
// internas de vulkan. Lo dejo aqui para quien lo vea, sepa que no es mi codigo.

#include "FileSystem.h"
#include "Vulkan\Utils\ErrorHandling.h"
#include <stdexcept>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#if defined(__linux__)
#include <unistd.h>
#endif

namespace Platform {

    std::string GetExecutableDirectory() {

#ifdef __APPLE__
        char buffer[1024];
        uint32_t size = sizeof(buffer);
        int result = _NSGetExecutablePath(buffer, &size);
        GE_CHECK(result == 0, "Failed to get executable path on macOS");

        // If buffer was too small, resize and retry
        if (size > sizeof(buffer)) {
            std::vector<char> bigBuffer(size);
            result = _NSGetExecutablePath(bigBuffer.data(), &size);
            GE_CHECK(result == 0, "Failed to get executable path on macOS (retry)");
            return std::filesystem::path(bigBuffer.data()).parent_path().string();
        }

        return std::filesystem::path(buffer).parent_path().string();

#elif defined(_WIN32)
        char buffer[1024];
        DWORD size = GetModuleFileNameA(NULL, buffer, sizeof(buffer));

        // Error Handling.
        GE_CHECK(size != 0, "Failed to get executable path on Windows");

        return std::filesystem::path(buffer).parent_path().string();

#elif defined(__linux__)
        char buffer[1024];
        ssize_t count = readlink("/proc/self/exe", buffer, sizeof(buffer));

        // Error Handling.
        GE_CHECK(count != -1, "Failed to get executable path on Linux");

        buffer[count] = '\0';
        return std::filesystem::path(buffer).parent_path().string()

#else
        return ".";
#endif
    }

}
