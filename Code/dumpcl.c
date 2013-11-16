#include <stdio.h>
#include <stdlib.h>
#include <OpenCL/opencl.h>

int main()
{
    cl_uint num_devices, i;
    char buf[128];
    cl_device_id *devices;
    clGetDeviceIDs(NULL, CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
    devices = calloc(sizeof(cl_device_id), num_devices);
    clGetDeviceIDs(NULL, CL_DEVICE_TYPE_ALL, num_devices, devices, NULL);
    for (i = 0; i < num_devices; i++)
    {
        clGetDeviceInfo(devices[i], CL_DEVICE_NAME, 128, buf, NULL);
        fprintf(stdout, "Device %s supports ", buf);
        clGetDeviceInfo(devices[i], CL_DEVICE_VERSION, 128, buf, NULL);
        fprintf(stdout, "%s\n", buf);
    }
    free(devices);
    return 0;
}
