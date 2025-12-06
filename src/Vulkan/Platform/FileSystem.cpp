#include "FileSystem.h"
#include <stdexcept>
#include <filesystem>

// I don't fucking care, but im saying this. This Whole fucking file is made by my good fellow friend called CHATGPT. Thank you. Maybe next time i use Grok.

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
        if (_NSGetExecutablePath(buffer, &size) == 0 ) {
            return std::filesystem::path(buffer).parent_path().string();
        } else {
            throw std::runtime_error("Failed to get executable path");
        }

    #elif defined(_WIN32)
        char buffer[1024];
        DWORD size = GetModuleFileNameA(NULL, buffer, sizeof(buffer));
        if (size == 0) {
            throw std::runtime_error("Failed to get executable path");
        }
        return std::filesystem::path(buffer).parent_path().string();

    #elif defined(__linux__)
        char buffer[1024];
        ssize_t count = readlink("/proc/self/exe", buffer, sizeof(buffer));
        if (count == -1) {
            throw std::runtime_error("Failed to get executable path");
        }
        buffer[count] = '\0';
        return std::filesystem::path(buffer).parent_path().string();
    #else
        return ".";
    #endif
    }

}