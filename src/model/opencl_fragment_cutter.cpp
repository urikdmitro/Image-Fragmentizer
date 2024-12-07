#include <CL/opencl.hpp>
#include <sstream>
#include <fstream>
#include <easylogging++.h>
#include "model/opencl_fragment_cutter.h"
#include "model/image.h"
#include "resources.h"

const char *getErrorString(cl_int error);

bool OpenCLFragmentCutter::FindPlatform() const
{
    std::vector<cl::Platform> cl_platforms;
    cl::Platform::get(&cl_platforms);

    if (cl_platforms.size() == 0)
    {
        LOG(ERROR) << "No OpenCl platforms found";
        return false;
    }

    this->platform = *(--cl_platforms.end());

    LOG(INFO)
        << "Using platform: "
        << platform.getInfo<CL_PLATFORM_NAME>();

    return true;
}

bool OpenCLFragmentCutter::FindDevice() const
{
    std::vector<cl::Device> cl_devices;
    platform.getDevices(CL_DEVICE_TYPE_ALL, &cl_devices);

    if(cl_devices.size() == 0)
    {
        LOG(ERROR) << "No OpenCl devices found";
        return false;
    }

    if (
        cl_devices.size() == 1 &&
        cl_devices[0].getInfo<CL_DEVICE_TYPE>() == CL_DEVICE_TYPE_CPU
    ) {
        LOG(WARNING)
            << "Warning! Found only CPU device: "
            << cl_devices[0].getInfo<CL_DEVICE_NAME>();
    }

    device = *(--cl_devices.end());

    LOG(INFO)
        << "Using device: "
        << device.getInfo<CL_DEVICE_NAME>();

    return true;
}

OpenCLFragmentCutter::OpenCLFragmentCutter(std::string path_to_kernel)
    : is_opencl_initialized(true)
{
    if(!FindPlatform())
    {
        is_opencl_initialized = false;
        return;
    }
    if(!FindDevice())
    {
        is_opencl_initialized = false;
        return;
    }
    new (&context) cl::Context({device});


    std::ifstream kernel_source_file(path_to_kernel);
    std::stringstream buffer;
    buffer << kernel_source_file.rdbuf();

    if(buffer.str().length() == 0)
    {
        LOG(ERROR) << "OpenCL kernel not found";
        is_opencl_initialized = false;
        return;
    }
    else
    {
        LOG(INFO)
            << "Was read "
            << buffer.str().length()
            << " bytes of OpenCL kernel";
    }

    kernel_sources.push_back(buffer.str());
    program = cl::Program(context, kernel_sources);

    if (program.build({device}) != CL_SUCCESS)
    {
        LOG(ERROR)
            << "Error building: "
            << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);

        is_opencl_initialized = false;
    }
    else
    {
        LOG(INFO) << "Kernel was builded successful";
    }
}

#define CUT_IMAGE_TO_FRAGMENT_ERROR_CHECK(STATUS, STRING)                      \
    if (status != CL_SUCCESS)                                                  \
    {                                                                          \
        LOG(ERROR)                                                             \
            << STRING " fail: "                                                \
            << getErrorString(status);                                         \
        return;                                                                \
    }                                                                          \
    else                                                                       \
    {                                                                          \
        LOG(INFO) <<  STRING " success";                                       \
    }

void LogFragmentInfo(FragmentInfo fragment_info, const Image& image)
{
    LOG(INFO)
        << "Image width: " << image.width
        << ", height: " << image.height
        << ", channels: " << image.channels;

    std::stringstream channels_to_fragmentize_string;
    for (auto c : fragment_info.GetChannelsToFragmentize())
    {
        channels_to_fragmentize_string << static_cast<int>(c) << " ";
    }
    LOG(INFO)
        << "Channels to fragmentize: "
        << channels_to_fragmentize_string.str();
}

void OpenCLFragmentCutter::CutImageToFragment(
    FragmentInfo fragment_info,
    Image& image
) const
{
    if (!is_opencl_initialized) {
        LOG(ERROR) << "OpenCL is not initialized";
        return;
    }

    LogFragmentInfo(fragment_info, image);


    unsigned char channels_to_fragmentize[4] = {0};
    cl_int status;


    cl::Buffer image_buffer(
        context,
        CL_MEM_READ_WRITE,
        image.GetSize(),
        nullptr,
        &status
    );
    CUT_IMAGE_TO_FRAGMENT_ERROR_CHECK(status, "Create image buffer");

    cl::Buffer channels_buffer(context, CL_MEM_READ_ONLY, 4, nullptr, &status);
    CUT_IMAGE_TO_FRAGMENT_ERROR_CHECK(status, "Create channels buffer");


    cl::CommandQueue queue(context, device);

    status = queue.enqueueWriteBuffer(
        image_buffer,
        CL_TRUE,
        0,
        image.GetSize(),
        image.raw_data
    );
    CUT_IMAGE_TO_FRAGMENT_ERROR_CHECK(status, "Write image buffer");

    status = queue.enqueueWriteBuffer(
        channels_buffer,
        CL_TRUE,
        0,
        4,
        channels_to_fragmentize
    );
    CUT_IMAGE_TO_FRAGMENT_ERROR_CHECK(status, "Write channels buffer");


    uint lower_bound =
        (std::numeric_limits<std::uint8_t>::max() + 1)
        / fragment_info.fragments_count * fragment_info.fragment_number;

    uint upper_bound =
        lower_bound + (std::numeric_limits<std::uint8_t>::max() + 1)
        / fragment_info.fragments_count;

    if (fragment_info.fragments_count - 1 == fragment_info.fragment_number) {
        upper_bound = std::numeric_limits<std::uint8_t>::max();
    }

    cl::Kernel kernel(program, "cut_image_to_fragments");
    kernel.setArg(0, image_buffer);
    kernel.setArg(1, image.width);
    kernel.setArg(2, image.height);
    kernel.setArg(3, image.channels);
    kernel.setArg(4, lower_bound);
    kernel.setArg(5, upper_bound);
    kernel.setArg(6, fragment_info.nonfragment_pixel_value);
    kernel.setArg(7, channels_buffer);
    kernel.setArg(8, (uint)4);

    cl::NDRange global(image.width, image.height);

    status = queue.enqueueNDRangeKernel(
        kernel,
        cl::NullRange,
        global,
        cl::NullRange
    );
    CUT_IMAGE_TO_FRAGMENT_ERROR_CHECK(status, "Enqueue kernel");


    status = queue.finish();
    CUT_IMAGE_TO_FRAGMENT_ERROR_CHECK(status, "Finish queue");

    status = queue.enqueueReadBuffer(
        image_buffer,
        CL_TRUE,
        0,
        image.GetSize(),
        image.raw_data
    );
    CUT_IMAGE_TO_FRAGMENT_ERROR_CHECK(status, "Read fragment from buffer");
}


const char *getErrorString(cl_int error)
{
    switch(error){
        // run-time and JIT compiler errors
        case 0: return "CL_SUCCESS";
        case -1: return "CL_DEVICE_NOT_FOUND";
        case -2: return "CL_DEVICE_NOT_AVAILABLE";
        case -3: return "CL_COMPILER_NOT_AVAILABLE";
        case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
        case -5: return "CL_OUT_OF_RESOURCES";
        case -6: return "CL_OUT_OF_HOST_MEMORY";
        case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
        case -8: return "CL_MEM_COPY_OVERLAP";
        case -9: return "CL_IMAGE_FORMAT_MISMATCH";
        case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
        case -11: return "CL_BUILD_PROGRAM_FAILURE";
        case -12: return "CL_MAP_FAILURE";
        case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
        case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
        case -15: return "CL_COMPILE_PROGRAM_FAILURE";
        case -16: return "CL_LINKER_NOT_AVAILABLE";
        case -17: return "CL_LINK_PROGRAM_FAILURE";
        case -18: return "CL_DEVICE_PARTITION_FAILED";
        case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

        // compile-time errors
        case -30: return "CL_INVALID_VALUE";
        case -31: return "CL_INVALID_DEVICE_TYPE";
        case -32: return "CL_INVALID_PLATFORM";
        case -33: return "CL_INVALID_DEVICE";
        case -34: return "CL_INVALID_CONTEXT";
        case -35: return "CL_INVALID_QUEUE_PROPERTIES";
        case -36: return "CL_INVALID_COMMAND_QUEUE";
        case -37: return "CL_INVALID_HOST_PTR";
        case -38: return "CL_INVALID_MEM_OBJECT";
        case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
        case -40: return "CL_INVALID_IMAGE_SIZE";
        case -41: return "CL_INVALID_SAMPLER";
        case -42: return "CL_INVALID_BINARY";
        case -43: return "CL_INVALID_BUILD_OPTIONS";
        case -44: return "CL_INVALID_PROGRAM";
        case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
        case -46: return "CL_INVALID_KERNEL_NAME";
        case -47: return "CL_INVALID_KERNEL_DEFINITION";
        case -48: return "CL_INVALID_KERNEL";
        case -49: return "CL_INVALID_ARG_INDEX";
        case -50: return "CL_INVALID_ARG_VALUE";
        case -51: return "CL_INVALID_ARG_SIZE";
        case -52: return "CL_INVALID_KERNEL_ARGS";
        case -53: return "CL_INVALID_WORK_DIMENSION";
        case -54: return "CL_INVALID_WORK_GROUP_SIZE";
        case -55: return "CL_INVALID_WORK_ITEM_SIZE";
        case -56: return "CL_INVALID_GLOBAL_OFFSET";
        case -57: return "CL_INVALID_EVENT_WAIT_LIST";
        case -58: return "CL_INVALID_EVENT";
        case -59: return "CL_INVALID_OPERATION";
        case -60: return "CL_INVALID_GL_OBJECT";
        case -61: return "CL_INVALID_BUFFER_SIZE";
        case -62: return "CL_INVALID_MIP_LEVEL";
        case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
        case -64: return "CL_INVALID_PROPERTY";
        case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
        case -66: return "CL_INVALID_COMPILER_OPTIONS";
        case -67: return "CL_INVALID_LINKER_OPTIONS";
        case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

        // extension errors
        case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
        case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
        case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
        case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
        case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
        case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
        default: return "Unknown OpenCL error";
    }
}
