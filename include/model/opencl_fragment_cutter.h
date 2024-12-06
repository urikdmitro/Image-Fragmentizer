#ifndef MODEL_OPENCL_FRAGMENT_CUTTER_H_
#define MODEL_OPENCL_FRAGMENT_CUTTER_H_

#include "fragment_cutter.h"
#include <CL/opencl.hpp>
#include <utility>

class OpenCLFragmentCutter : public IFragmentCutter
{
private:
    cl::Program::Sources kernel_sources;
    mutable cl::Platform platform;
    mutable cl::Device device;
    cl::Context context;
    cl::Program program;

    bool is_opencl_initialized;

    bool FindPlatform() const;
    bool FindDevice() const;

public:
    OpenCLFragmentCutter(std::string path_to_kernel);

    virtual void CutImageToFragment(
        FragmentInfo fragment_info,
        Image& image
    ) const override;
};

#endif // MODEL_OPENCL_FRAGMENT_CUTTER_H_