#include "model/benchmarker.h"
#include "benchmarker.h"

Benchmarker::Benchmarker(Fragmentizer &fragmentizer)
    : fragmentizer(fragmentizer)
{ }

void Benchmarker::RunFragmentation(
    int fragments_count,
    ChannelsMask::T channels_mask
)
{
    for(int i = 0; i < fragments_count; i++)
    {
        fragmentizer.GetFragment(
            FragmentInfo(
                fragments_count,
                i,
                channels_mask,
                255
            )
        );
    }
}

std::chrono::milliseconds Benchmarker::RunFullBenchmark(
    int fragments_count,
    bool use_cache,
    ChannelsMask::T channels_mask
)
{
    bool was_cache_enabled = fragmentizer.IsCacheEnable();

    if(use_cache)
    {
        fragmentizer.EnableCache();
    }
    else
    {
        fragmentizer.DisableCache();
    }


    auto start = std::chrono::high_resolution_clock::now();

    RunFragmentation(fragments_count, channels_mask);

    auto end = std::chrono::high_resolution_clock::now();


    if(was_cache_enabled)
    {
        fragmentizer.EnableCache();
    }
    else
    {
        fragmentizer.DisableCache();
    }

    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
}
