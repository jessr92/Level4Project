#include "OpenCLUtils.h"

const char *clErrorString(cl_int err)
{
    const char *errString = NULL;

    switch (err)
    {
    case CL_SUCCESS:
        errString = "Success";
        break;

    case CL_DEVICE_NOT_FOUND:
        errString = "Device not found";
        break;

    case CL_DEVICE_NOT_AVAILABLE:
        errString = "Device not available";
        break;

    case CL_COMPILER_NOT_AVAILABLE:
        errString = "Compiler not available";
        break;

    case CL_MEM_OBJECT_ALLOCATION_FAILURE:
        errString = "Memory object allocation failure";
        break;

    case CL_OUT_OF_RESOURCES:
        errString = "Out of resources";
        break;

    case CL_OUT_OF_HOST_MEMORY:
        errString = "Out of host memory";
        break;

    case CL_PROFILING_INFO_NOT_AVAILABLE:
        errString = "Profiling info not available";
        break;

    case CL_MEM_COPY_OVERLAP:
        errString = "Memory copy overlap";
        break;

    case CL_IMAGE_FORMAT_MISMATCH:
        errString = "Image format mismatch";
        break;

    case CL_IMAGE_FORMAT_NOT_SUPPORTED:
        errString = "Image format not supported";
        break;

    case CL_BUILD_PROGRAM_FAILURE:
        errString = "Build program failure";
        break;

    case CL_MAP_FAILURE:
        errString = "Map failure";
        break;

#ifdef CL_VERSION_1_1
    case CL_MISALIGNED_SUB_BUFFER_OFFSET:
        errString = "Misaligned sub buffer offset";
        break;

    case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
        errString = "Exec status error for events in wait list";
        break;
#endif

#ifdef CL_VERSION_1_2
    case CL_COMPILE_PROGRAM_FAILURE:
        errString = "Compile program failure";
        break;

    case CL_LINKER_NOT_AVAILABLE:
        errString = "Linker not available";
        break;

    case CL_LINK_PROGRAM_FAILURE:
        errString = "Link program failure";
        break;

    case CL_DEVICE_PARTITION_FAILED:
        errString = "Device partition failed";
        break;
#endif

    case CL_INVALID_VALUE:
        errString = "Invalid value";
        break;

    case CL_INVALID_DEVICE_TYPE:
        errString = "Invalid device type";
        break;

    case CL_INVALID_PLATFORM:
        errString = "Invalid platform";
        break;

    case CL_INVALID_DEVICE:
        errString = "Invalid device";
        break;

    case CL_INVALID_CONTEXT:
        errString = "Invalid context";
        break;

    case CL_INVALID_QUEUE_PROPERTIES:
        errString = "Invalid queue properties";
        break;

    case CL_INVALID_COMMAND_QUEUE:
        errString = "Invalid command queue";
        break;

    case CL_INVALID_HOST_PTR:
        errString = "Invalid host pointer";
        break;

    case CL_INVALID_MEM_OBJECT:
        errString = "Invalid memory object";
        break;

    case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
        errString = "Invalid image format descriptor";
        break;

    case CL_INVALID_IMAGE_SIZE:
        errString = "Invalid image size";
        break;

    case CL_INVALID_SAMPLER:
        errString = "Invalid sampler";
        break;

    case CL_INVALID_BINARY:
        errString = "Invalid binary";
        break;

    case CL_INVALID_BUILD_OPTIONS:
        errString = "Invalid build options";
        break;

    case CL_INVALID_PROGRAM:
        errString = "Invalid program";
        break;

    case CL_INVALID_PROGRAM_EXECUTABLE:
        errString = "Invalid program executable";
        break;

    case CL_INVALID_KERNEL_NAME:
        errString = "Invalid kernel name";
        break;

    case CL_INVALID_KERNEL_DEFINITION:
        errString = "Invalid kernel definition";
        break;

    case CL_INVALID_KERNEL:
        errString = "Invalid kernel";
        break;

    case CL_INVALID_ARG_INDEX:
        errString = "Invalid argument index";
        break;

    case CL_INVALID_ARG_VALUE:
        errString = "Invalid argument value";
        break;

    case CL_INVALID_ARG_SIZE:
        errString = "Invalid argument size";
        break;

    case CL_INVALID_KERNEL_ARGS:
        errString = "Invalid kernel arguments";
        break;

    case CL_INVALID_WORK_DIMENSION:
        errString = "Invalid work dimension";
        break;

    case CL_INVALID_WORK_GROUP_SIZE:
        errString = "Invalid work group size";
        break;

    case CL_INVALID_WORK_ITEM_SIZE:
        errString = "invalid work item size";
        break;

    case CL_INVALID_GLOBAL_OFFSET:
        errString = "Invalid global offset";
        break;

    case CL_INVALID_EVENT_WAIT_LIST:
        errString = "Invalid event wait list";
        break;

    case CL_INVALID_EVENT:
        errString = "Invalid event";
        break;

    case CL_INVALID_OPERATION:
        errString = "Invalid operation";
        break;

    case CL_INVALID_GL_OBJECT:
        errString = "Invalid OpenGL object";
        break;

    case CL_INVALID_BUFFER_SIZE:
        errString = "Invalid buffer size";
        break;

    case CL_INVALID_MIP_LEVEL:
        errString = "Invalid MIP level";
        break;

    case CL_INVALID_GLOBAL_WORK_SIZE:
        errString = "Invalid global work size";
        break;

#ifdef CL_VERSION_1_1
    case CL_INVALID_PROPERTY:
        errString = "Invalid property";
        break;
#endif

#ifdef CL_VERSION_1_2
    case CL_INVALID_IMAGE_DESCRIPTOR:
        errString = "Invalid image descriptor";
        break;

    case CL_INVALID_COMPILER_OPTIONS:
        errString = "Invalid compiler options";
        break;

    case CL_INVALID_LINKER_OPTIONS:
        errString = "Invalid linker options";
        break;

    case CL_INVALID_DEVICE_PARTITION_COUNT:
        errString = "Invalid device partition count";
        break;
#endif
    }
    return errString;
}
