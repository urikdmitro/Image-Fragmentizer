#ifndef MODEL_CHANNELS_MASK_H_
#define MODEL_CHANNELS_MASK_H_

namespace ChannelsMask
{
    enum T
    {
        kNone = 0b0000,
        kR    = 0b0001,
        kG    = 0b0010,
        kB    = 0b0100,
        kA    = 0b1000,
        kRGB  = 0b0111,
        kRGBA = 0b1111,
    };
};

#endif // MODEL_CHANNELS_MASK_H_
