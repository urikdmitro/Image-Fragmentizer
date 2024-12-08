#ifndef IMAGE_H_
#define IMAGE_H_

#include <cstdint>
#include <string>

struct Image
{
    std::uint8_t *raw_data;
    std::uint32_t width;
    std::uint32_t height;
    std::uint32_t channels;

    Image();
    Image(std::string path_to_image);
    ~Image();

    Image(const Image &image);
    Image(Image &&image);

    Image &operator=(const Image &image);
    Image &operator=(Image &&image);

    size_t GetSize() const;
};

#endif // IMAGE_H_