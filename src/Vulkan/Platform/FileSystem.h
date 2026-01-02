#pragma once

#include <string>
#include <filesystem>

namespace Filesystem {

    std::filesystem::path getExecutableDirectory();

    std::filesystem::path getResourcePath(const std::string& relativePath);

}