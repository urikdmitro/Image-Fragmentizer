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

    Image();
    Image(std::string path_to_image);
    ~Image();

    Image(const Image &image);
    Image(Image &&image);

    Image &operator=(const Image &image);
    Image &operator=(Image &&image);
};

#endif // IMAGE_H_