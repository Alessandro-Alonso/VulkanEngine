#pragma once

#include <string>
#include <filesystem>

// Esto es simplemente un header para funciones relacionadas con el sistema de archivos,
// como obtener la ruta del ejecutable y la ruta de los recursos.
namespace Filesystem {

    std::filesystem::path getExecutableDirectory();

    std::filesystem::path getResourcePath(const std::string& relativePath);

}