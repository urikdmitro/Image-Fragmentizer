#ifndef RESOURCES_H_
#define RESOURCES_H_

#include <string>

namespace Resources
{
    std::string GetAssetsFolderPath();
    std::string GetAssetFullPath(std::string path_to_asset);
};

#endif // RESOURCES_H_
