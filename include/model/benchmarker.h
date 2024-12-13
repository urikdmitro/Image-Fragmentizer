#ifndef MODEL_BENCHMARKER_H_
#define MODEL_BENCHMARKER_H_

#include "fragmentizer.h"
#include <chrono>

class Benchmarker
{
private:
    Fragmentizer &fragmentizer;
    void RunFragmentation(int fragments_count, ChannelsMask::T channels_mask);

public:
    Benchmarker(Fragmentizer &fragmentizer);

    std::chrono::milliseconds RunFullBenchmark(
        int fragments_count,
        bool use_cache = false,
        ChannelsMask::T channels_mask = ChannelsMask::kRGB
    );
};

#endif // MODEL_BENCHMARKER_H_
