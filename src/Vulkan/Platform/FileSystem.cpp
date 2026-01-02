#include "FileSystem.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <limits.h>
#include <unistd.h>
#endif

std::filesystem::path Filesystem::getExecutableDirectory() {
#ifdef _WIN32
    char path[MAX_PATH];
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    return std::filesystem::path(path).parent_path();
#else
    char path[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
    if (count != -1) {
        path[count] = '\0';
        return std::filesystem::path(path).parent_path();
    }
    return std::filesystem::current_path();
#endif
}

std::filesystem::path Filesystem::getResourcePath(const std::string& relativePath) {
    return getExecutableDirectory() / relativePath;
}