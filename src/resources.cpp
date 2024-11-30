#include "resources.h"
#include <string>


const std::string kAssetsFolder(
    "/home/dmitro/Documents/Projects/C/Image/assets"
);

std::string Resources::GetAssetsFolderPath()
{
    return kAssetsFolder;
}

std::string Resources::GetAssetFullPath(std::string path_to_asset)
{
    return kAssetsFolder + path_to_asset;
}
