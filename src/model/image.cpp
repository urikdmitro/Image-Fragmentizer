#include "model/image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Image::Image(std::string path_to_image)
{
    raw_data = stbi_load(path_to_image.c_str(),
                         reinterpret_cast<int *>(&width),
                         reinterpret_cast<int *>(&height),
                         reinterpret_cast<int *>(&channels),
                         0);
}

Image::Image(const Image &raw_image)
    : width(raw_image.width), height(raw_image.height), channels(raw_image.channels)
{
    this->raw_data = (uint8_t*)STBI_MALLOC(width * height * channels);
    std::copy(raw_image.raw_data,
              raw_image.raw_data + (width * height * channels),
              this->raw_data);
}

Image::Image(Image &&raw_image)
{
    this->raw_data = raw_image.raw_data;
    this->width = raw_image.width;
    this->height = raw_image.height;
    this->channels = raw_image.channels;

    raw_image.raw_data = nullptr;
    raw_image.width = 0;
    raw_image.height = 0;
    raw_image.channels = 0;
}

Image &Image::operator=(const Image &raw_image)
{
    this->width = raw_image.width;
    this->height = raw_image.height;
    this->channels = raw_image.channels;

    this->raw_data = (uint8_t*)STBI_MALLOC(width * height * channels);
    std::copy(raw_image.raw_data,
              raw_image.raw_data + (width * height * channels),
              this->raw_data);

    return *this;
}

Image &Image::operator=(Image &&raw_image)
{
    this->raw_data = raw_image.raw_data;
    this->width = raw_image.width;
    this->height = raw_image.height;
    this->channels = raw_image.channels;

    raw_image.raw_data = nullptr;
    raw_image.width = 0;
    raw_image.height = 0;
    raw_image.channels = 0;

    return *this;
}

Image::~Image()
{
    stbi_image_free(raw_data);
}