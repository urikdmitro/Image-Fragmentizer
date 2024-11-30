#ifndef IMAGE_H_
#define IMAGE_H_

#include <stdint.h>
#include <string>

struct Image
{
    uint8_t *raw_data;
    uint32_t width;
    uint32_t height;
    uint32_t channels;

    Image(std::string path_to_image);
    Image(const Image &raw_image);
    Image(Image &&raw_image);
    Image &operator=(const Image &raw_image);
    Image &operator=(Image &&raw_image);
    ~Image();
};

#endif // IMAGE_H_