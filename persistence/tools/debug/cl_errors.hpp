//
// file : unix_errors.hpp
// in : file:///home/tim/projects/nsched/nsched/tools/debug/unix_errors.hpp
//
// created by : Timothée Feuillet on linux.site
// date: 04/08/2014 14:24:51
//
//
// Copyright (C) 2014 Timothée Feuillet
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#ifndef __N_14838165221451961813_480336325__UNIX_ERRORS_HPP__
# define __N_14838165221451961813_480336325__UNIX_ERRORS_HPP__

#include <CL/cl.h>

#include <string>

namespace neam
{
  namespace debug
  {
    namespace errors
    {
      namespace internal
      {
        struct cl_error_entry
        {
          cl_int code;
          std::string code_name;
          std::string function;
          std::string description;
        };

        // thx to http://streamcomputing.eu/blog/2013-04-28/opencl-1-2-error-codes/
        // and sed. (sed 's/^\(.*\)\t\(.*\)\t\(.*\)$/{\1, "\2", "\3"},/gi')

#define TP_MK_ENTRY(code, func, descr)  {code, #code, func, descr}
        cl_error_entry cl_error_table[] =
        {
          TP_MK_ENTRY(CL_SUCCESS, "*", "Everything is GOOD !"),
          TP_MK_ENTRY(CL_DEVICE_NOT_FOUND, "clGetDeviceIDs", "no OpenCL devices that matched device_type were found."),
          TP_MK_ENTRY(CL_DEVICE_NOT_AVAILABLE, "clCreateContext", "a device in devices is currently not available even though the device was returned by clGetDeviceIDs."),
          TP_MK_ENTRY(CL_COMPILER_NOT_AVAILABLE, "clBuildProgram", "program is created with clCreateProgramWithSource and a compiler is not available i.e. CL_DEVICE_COMPILER_AVAILABLE specified in the table of OpenCL Device Queries for clGetDeviceInfo is set to CL_FALSE."),
          TP_MK_ENTRY(CL_MEM_OBJECT_ALLOCATION_FAILURE, "", "there is a failure to allocate memory for buffer object."),
          TP_MK_ENTRY(CL_OUT_OF_RESOURCES, "", "there is a failure to allocate resources required by the OpenCL implementation on the device."),
          TP_MK_ENTRY(CL_OUT_OF_HOST_MEMORY, "", "there is a failure to allocate resources required by the OpenCL implementation on the host."),
          TP_MK_ENTRY(CL_PROFILING_INFO_NOT_AVAILABLE, "clGetEventProfilingInfo", "the CL_QUEUE_PROFILING_ENABLE flag is not set for the command-queue, if the execution status of the command identified by event is not CL_COMPLETE or if event is a user event object."),
          TP_MK_ENTRY(CL_MEM_COPY_OVERLAP, "clEnqueueCopyBuffer, clEnqueueCopyBufferRect, clEnqueueCopyImage", "src_buffer and dst_buffer are the same buffer or subbuffer object and the source and destination regions overlap or if src_buffer and dst_buffer are different sub-buffers of the same associated buffer object and they overlap. The regions overlap if src_offset ≤ to dst_offset ≤ to src_offset + size – 1, or if dst_offset ≤ to src_offset ≤ to dst_offset + size – 1."),
          TP_MK_ENTRY(CL_IMAGE_FORMAT_MISMATCH, "clEnqueueCopyImage", "src_image and dst_image do not use the same image format."),
          TP_MK_ENTRY(CL_IMAGE_FORMAT_NOT_SUPPORTED, "clCreateImage", "the image_format is not supported."),
          TP_MK_ENTRY(CL_BUILD_PROGRAM_FAILURE, "clBuildProgram", "there is a failure to build the program executable."),
          TP_MK_ENTRY(CL_MAP_FAILURE, "clEnqueueMapBuffer, clEnqueueMapImage", " if there is a failure to map the requested region into the host address space. This error cannot occur for image objects created with CL_MEM_USE_HOST_PTR or CL_MEM_ALLOC_HOST_PTR."),
          TP_MK_ENTRY(CL_MISALIGNED_SUB_BUFFER_OFFSET, "", "a sub-buffer object is specified as the value for an argument that is a buffer object and the offset specified when the sub-buffer object is created is not aligned to CL_DEVICE_MEM_BASE_ADDR_ALIGN value for device associated with queue."),
          TP_MK_ENTRY(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST, "", "the execution status of any of the events in event_list is a negative integer value."),
//         TP_MK_ENTRY(CL_COMPILE_PROGRAM_FAILURE, "clCompileProgram", "there is a failure to compile the program source. This error will be returned if clCompileProgram does not return until the compile has completed."),
//         TP_MK_ENTRY(CL_LINKER_NOT_AVAILABLE, "clLinkProgram", "a linker is not available i.e. CL_DEVICE_LINKER_AVAILABLE specified in the table of allowed values for param_name for clGetDeviceInfo is set to CL_FALSE."),
//         TP_MK_ENTRY(CL_LINK_PROGRAM_FAILURE, "clLinkProgram", "there is a failure to link the compiled binaries and/or libraries."),
//         TP_MK_ENTRY(CL_DEVICE_PARTITION_FAILED, "clCreateSubDevices", " if the partition name is supported by the implementation but in_device could not be further partitioned."),
//         TP_MK_ENTRY(CL_KERNEL_ARG_INFO_NOT_AVAILABLE, "clGetKernelArgInfo", "the argument information is not available for kernel."}
          TP_MK_ENTRY(CL_INVALID_VALUE, "clGetDeviceIDs, clCreateContext", "This depends on the function: two or more coupled parameters had errors."),
          TP_MK_ENTRY(CL_INVALID_DEVICE_TYPE, "clGetDeviceIDs", "an invalid device_type is given"),
          TP_MK_ENTRY(CL_INVALID_PLATFORM, "clGetDeviceIDs", "an invalid platform was given"),
          TP_MK_ENTRY(CL_INVALID_DEVICE, "clCreateContext, clBuildProgram", "devices contains an invalid device or are not associated with the specified platform."),
          TP_MK_ENTRY(CL_INVALID_CONTEXT, "", "context is not a valid context."),
          TP_MK_ENTRY(CL_INVALID_QUEUE_PROPERTIES, "clCreateCommandQueue", "specified command-queue-properties are valid but are not supported by the device."),
          TP_MK_ENTRY(CL_INVALID_COMMAND_QUEUE, "", "command_queue is not a valid command-queue."),
          TP_MK_ENTRY(CL_INVALID_HOST_PTR, "clCreateImage, clCreateBuffer", "This flag is valid only if host_ptr is not NULL. If specified, it indicates that the application wants the OpenCL implementation to allocate memory for the memory object and copy the data from memory referenced by host_ptr.CL_MEM_COPY_HOST_PTR and CL_MEM_USE_HOST_PTR are mutually exclusive.CL_MEM_COPY_HOST_PTR can be used with CL_MEM_ALLOC_HOST_PTR to initialize the contents of the cl_mem object allocated using host-accessible (e.g. PCIe) memory."),
          TP_MK_ENTRY(CL_INVALID_MEM_OBJECT, "", "memobj is not a valid OpenCL memory object."),
          TP_MK_ENTRY(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR, "", "the OpenGL/DirectX texture internal format does not map to a supported OpenCL image format."),
          TP_MK_ENTRY(CL_INVALID_IMAGE_SIZE, "", "an image object is specified as an argument value and the image dimensions (image width, height, specified or compute row and/or slice pitch) are not supported by device associated with queue."),
          TP_MK_ENTRY(CL_INVALID_SAMPLER, "clGetSamplerInfo, clReleaseSampler, clRetainSampler, clSetKernelArg", "sampler is not a valid sampler object."),
          TP_MK_ENTRY(CL_INVALID_BINARY, "clCreateProgramWithBinary, clBuildProgram", "The provided binary is unfit for the selected device. If program is created with clCreateProgramWithBinary and devices isted in device_list do not have a valid program binary loaded."),
          TP_MK_ENTRY(CL_INVALID_BUILD_OPTIONS, "clBuildProgram", "the build options specified by options are invalid."),
          TP_MK_ENTRY(CL_INVALID_PROGRAM, "", "program is a not a valid program object."),
          TP_MK_ENTRY(CL_INVALID_PROGRAM_EXECUTABLE, "", "there is no successfully built program executable available for device associated with command_queue."),
          TP_MK_ENTRY(CL_INVALID_KERNEL_NAME, "clCreateKernel", "kernel_name is not found in program."),
          TP_MK_ENTRY(CL_INVALID_KERNEL_DEFINITION, "clCreateKernel", "the function definition for __kernel function given by kernel_name such as the number of arguments, the argument types are not the same for all devices for which the program executable has been built."),
          TP_MK_ENTRY(CL_INVALID_KERNEL, "", "kernel is not a valid kernel object."),
          TP_MK_ENTRY(CL_INVALID_ARG_INDEX, "clSetKernelArg, clGetKernelArgInfo", "arg_index is not a valid argument index."),
          TP_MK_ENTRY(CL_INVALID_ARG_VALUE, "clSetKernelArg, clGetKernelArgInfo", "arg_value specified is not a valid value."),
          TP_MK_ENTRY(CL_INVALID_ARG_SIZE, "clSetKernelArg", "arg_size does not match the size of the data type for an argument that is not a memory object or if the argument is a memory object and arg_size != sizeof(cl_mem) or if arg_size is zero and the argument is declared with the __local qualifier or if the argument is a sampler and arg_size != sizeof(cl_sampler)."),
          TP_MK_ENTRY(CL_INVALID_KERNEL_ARGS, "", "the kernel argument values have not been specified."),
          TP_MK_ENTRY(CL_INVALID_WORK_DIMENSION, "", "work_dim is not a valid value (i.e. a value between 1 and 3)."),
          TP_MK_ENTRY(CL_INVALID_WORK_GROUP_SIZE, "", "local_work_size is specified and number of work-items specified by global_work_size is not evenly divisable by size of work-group given by local_work_size or does not match the work-group size specified for kernel using the __attribute__ ((reqd_work_group_size(X, Y, Z))) qualifier in program source.if local_work_size is specified and the total number of work-items in the work-group computed as local_work_size[0] *… local_work_size[work_dim - 1] is greater than the value specified by CL_DEVICE_MAX_WORK_GROUP_SIZE in the table of OpenCL Device Queries for clGetDeviceInfo.if local_work_size is NULL and the __attribute__ ((reqd_work_group_size(X, Y, Z))) qualifier is used to declare the work-group size for kernel in the program source."),
          TP_MK_ENTRY(CL_INVALID_WORK_ITEM_SIZE, "", "the number of work-items specified in any of local_work_size[0], … local_work_size[work_dim - 1] is greater than the corresponding values specified by CL_DEVICE_MAX_WORK_ITEM_SIZES[0], …. CL_DEVICE_MAX_WORK_ITEM_SIZES[work_dim - 1]."),
          TP_MK_ENTRY(CL_INVALID_GLOBAL_OFFSET, "", "the value specified in global_work_size + the corresponding values in global_work_offset for any dimensions is greater than the sizeof(size_t) for the device on which the kernel execution will be enqueued."),
          TP_MK_ENTRY(CL_INVALID_EVENT_WAIT_LIST, "", "event_wait_list is NULL and num_events_in_wait_list > 0, or event_wait_list is not NULL and num_events_in_wait_list is 0, or if event objects in event_wait_list are not valid events."),
          TP_MK_ENTRY(CL_INVALID_EVENT, "", "event objects specified in event_list are not valid event objects."),
          TP_MK_ENTRY(CL_INVALID_OPERATION, "", "interoperability is specified by setting CL_CONTEXT_ADAPTER_D3D9_KHR, CL_CONTEXT_ADAPTER_D3D9EX_KHR or CL_CONTEXT_ADAPTER_DXVA_KHR to a non-NULL value, and interoperability with another graphics API is also specified. (only if the cl_khr_dx9_media_sharing extension is supported)."),
          TP_MK_ENTRY(CL_INVALID_GL_OBJECT, "", "texture is not a GL texture object whose type matches texture_target, if the specified miplevel of texture is not defined, or if the width or height of the specified miplevel is zero."),
          TP_MK_ENTRY(CL_INVALID_BUFFER_SIZE, "clCreateBuffer, clCreateSubBuffer", "size is 0.Implementations may return CL_INVALID_BUFFER_SIZE if size is greater than the CL_DEVICE_MAX_MEM_ALLOC_SIZE value specified in the table of allowed values for param_name for clGetDeviceInfo for all devices in context."),
          TP_MK_ENTRY(CL_INVALID_MIP_LEVEL, "OpenGL-functions", "miplevel is greater than zero and the OpenGL implementation does not support creating from non-zero mipmap levels."),
          TP_MK_ENTRY(CL_INVALID_GLOBAL_WORK_SIZE, "", "global_work_size is NULL, or if any of the values specified in global_work_size[0], …global_work_size [work_dim - 1] are 0 or exceed the range given by the sizeof(size_t) for the device on which the kernel execution will be enqueued."),
          TP_MK_ENTRY(CL_INVALID_PROPERTY, "clCreateContext", "Vague error, depends on the function"),
//         TP_MK_ENTRY(CL_INVALID_IMAGE_DESCRIPTOR, "clCreateImage", "values specified in image_desc are not valid or if image_desc is NULL."),
//         TP_MK_ENTRY(CL_INVALID_COMPILER_OPTIONS, "clCompileProgram", "the compiler options specified by options are invalid."),
//         TP_MK_ENTRY(CL_INVALID_LINKER_OPTIONS, "clLinkProgram", "the linker options specified by options are invalid."),
//         TP_MK_ENTRY(CL_INVALID_DEVICE_PARTITION_COUNT, "clCreateSubDevices", "the partition name specified in properties is CL_DEVICE_PARTITION_BY_COUNTS and the number of sub-devices requested exceeds CL_DEVICE_PARTITION_MAX_SUB_DEVICES or the total number of compute units requested exceeds CL_DEVICE_PARTITION_MAX_COMPUTE_UNITS for in_device, or the number of compute units requested for one or more sub-devices is less than zero or the number of sub-devices requested exceeds CL_DEVICE_PARTITION_MAX_COMPUTE_UNITS for in_device."),
          TP_MK_ENTRY(-1001, "clGetPlatformIDs", "no platforms, could not load nvidia driver"),
        };
#undef TP_MK_ENTRY

      } // namespace internal
      // for opencl errors
      template<typename T>
      struct cl_errors
      {
        static bool is_error(long code)
        {
          if (code == CL_SUCCESS)
            return false;
          for (auto &it : internal::cl_error_table)
          {
            if (it.code == code)
              return true;
          }
          return false;
        }

        static bool exists(long code)
        {
          if (code == 0)
            return true;
          return is_error();
        }

        static std::string get_code_name(long code)
        {
          for (auto &it : internal::cl_error_table)
          {
            if (it.code == code)
              return it.code_name;
          }
          return "UNKNOW CODE";
        }

        static std::string get_description(long code)
        {
          for (auto &it : internal::cl_error_table)
          {
            if (it.code == code)
              return it.description;
          }
          return "unknow error";
        }
      };
    } // namespace errors
  } // namespace debug
} // namespace neam

#endif /*__N_14838165221451961813_480336325__UNIX_ERRORS_HPP__*/

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 

