#include "model/image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <cstdlib>
#include <iostream>

Image::Image()
    : raw_data(nullptr)
    , width(0)
    , height(0)
    , channels(0)
{}

Image::Image(std::string path_to_image)
{
    raw_data = stbi_load(
        path_to_image.c_str(),
        reinterpret_cast<int *>(&width),
        reinterpret_cast<int *>(&height),
        reinterpret_cast<int *>(&channels),
        0
    );
}

Image::Image(const Image &image)
    : width(image.width)
    , height(image.height)
    , channels(image.channels)
{
    this->raw_data = (std::uint8_t*)STBI_MALLOC(width * height * channels);
    std::copy(
        image.raw_data,
        image.raw_data + (width * height * channels),
        this->raw_data
    );
}

Image::Image(Image &&image)
{
    this->raw_data = image.raw_data;
    this->width = image.width;
    this->height = image.height;
    this->channels = image.channels;

    image.raw_data = nullptr;
    image.width = 0;
    image.height = 0;
    image.channels = 0;
}

Image &Image::operator=(const Image &image)
{
    if(this != &image)
    {
        stbi_image_free(this->raw_data);

        this->width = image.width;
        this->height = image.height;
        this->channels = image.channels;

        this->raw_data = (std::uint8_t*)STBI_MALLOC(width * height * channels);
        std::copy(
            image.raw_data,
            image.raw_data + (width * height * channels),
            this->raw_data
        );
    }
    return *this;
}

Image &Image::operator=(Image &&image)
{
    if(this != &image)
    {
        stbi_image_free(this->raw_data);

        this->raw_data = image.raw_data;
        this->width = image.width;
        this->height = image.height;
        this->channels = image.channels;

        image.raw_data = nullptr;
        image.width = 0;
        image.height = 0;
        image.channels = 0;
    }

    return *this;
}

Image::~Image()
{
    stbi_image_free(raw_data);
}

size_t Image::GetSize() const
{
    return width * height * channels;
}
