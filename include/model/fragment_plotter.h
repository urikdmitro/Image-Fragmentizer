#ifndef MODEL_FRAGMENT_PLOTTER_H_
#define MODEL_FRAGMENT_PLOTTER_H_

#include "fragmentizer.h"
#include <vector>

struct IntensityGraph
{
    std::vector<double> x;
    std::vector<double> main;
    std::vector<double> r;
    std::vector<double> g;
    std::vector<double> b;
};

class FragmentPlotter
{
private:
    Fragmentizer &fragmentizer;

public:
    FragmentPlotter(Fragmentizer& fragmentizer);

    IntensityGraph GetIntensity(const Image &image, int nonfragment_value);
    IntensityGraph GetDensity(
        const Image &image,
        int nonfragment_value,
        const IntensityGraph &image_intensity
    );
    std::vector<double> GetDensity(int fragments_count);
};

#endif // MODEL_FRAGMENT_PLOTTER_H_
