#include "FileSystem.h"

#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#include <limits.h>
#else
#include <limits.h>
#include <unistd.h>
#endif

// Estas funciones son para obtener la ruta del ejecutable y la ruta de los recursos,
// lo cual es necesario para cargar los shaders y otros recursos de la aplicacion,
// independientemente de donde se ejecute el programa, ya que la ruta de los recursos
// es relativa a la ruta del ejecutable, y no a la ruta de donde se ejecute el programa.
std::filesystem::path Filesystem::getExecutableDirectory() {

// Si esta en windows, se obtiene la ruta del ejecutable usando GetModuleFileNameA,
// y se devuelve la ruta del directorio padre, que es donde se encuentran los recursos.
#ifdef _WIN32
    char path[MAX_PATH];
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    return std::filesystem::path(path).parent_path();

// En macOS, se obtiene la ruta del ejecutable usando _NSGetExecutablePath,
// y se devuelve la ruta del directorio padre, que es donde se encuentran los recursos.
#elif __APPLE__
    char path[PATH_MAX];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0) {
        return std::filesystem::path(path).parent_path();
    }
    return std::filesystem::current_path();

// En Linux, se obtiene la ruta del ejecutable leyendo el enlace simbolico /proc/self/exe,
// y se devuelve la ruta del directorio padre, que es donde se encuentran los recursos.
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