#include "stdafx.h"

static char *kernel_program_text[1];
static size_t kernel_program_length[1];
TOutput kernel_output;

bool test_kernels(int iPlatformIndex, int iDeviceIndex)
{
	cl_int error;
	cl_context context;
	static const int MAX_KERNELS = 32;
	cl_kernel kernels[MAX_KERNELS];
	cl_uint num_kernels_ret;

	kernel_output.appendLine("Staring Kernel test:");

	// load the program
	kernel_output.appendLine("Loading Kernel program...");
	char szProgramPath[_MAX_PATH + 1];
	static char *szProgramName = "test_program.cl";
	if (false == make_program_path(szProgramPath, _countof(szProgramPath), szProgramName))
		return false;
	kernel_output.appendLine(szProgramPath);

	if (false == load_program(szProgramPath, &kernel_program_text[0], &kernel_program_length[0]))
	{
		char szBuf[128];
		sprintf_s(szBuf, _countof(szBuf), "Unable to load \"%s\" from file.", szProgramName);
		kernel_output.appendLine(szBuf);
		return false;
	}

	//
	// create a Program and Kernel for the currently-selected device
	//
	kernel_output.appendLine("Starting kernel creation on current Device..");

	// create a Context for the Program
	context = clCreateContext(NULL, 1, &devices[iDeviceIndex], NULL, NULL, &error);
	if (error == CL_SUCCESS)
	{
		cl_program program;
		program = clCreateProgramWithSource(context, 1, (const char **)kernel_program_text, kernel_program_length, &error);
		if (error == CL_SUCCESS)
		{
			cl_int build_error = clBuildProgram(program, 1, &devices[iDeviceIndex], NULL, NULL, NULL);

			// always get the build log
			char build_log[1024];
			size_t size_ret;
			error = clGetProgramBuildInfo(program, devices[iDeviceIndex], CL_PROGRAM_BUILD_LOG, sizeof(build_log), build_log, &size_ret);

			if (build_error == CL_SUCCESS)
			{
				error = clCreateKernelsInProgram(program, MAX_KERNELS, kernels, &num_kernels_ret);
				if (error == CL_SUCCESS)
				{
					int i;

					// get information for eack kernel
					char function_name[128];
					char attributes[128];
					cl_uint number_args;
					size_t ret_size;
					for (i = 0; i < num_kernels_ret; ++i)
					{
						clGetKernelInfo(kernels[i], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
						clGetKernelInfo(kernels[i], CL_KERNEL_NUM_ARGS, sizeof(number_args), &number_args, &ret_size);
						clGetKernelInfo(kernels[i], CL_KERNEL_ATTRIBUTES, sizeof(attributes), attributes, &ret_size);
					}

					// create a buffer for kernel argument passing
					char msg[16];
					cl_mem msg_buffer;
					msg_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(msg), NULL, &error);

					// create a command queue for the currently-selected device
					cl_command_queue cq = clCreateCommandQueue(context, devices[iDeviceIndex], 0, &error);
					if (error == CL_SUCCESS)
					{
						kernel_output.appendLine("Kernels created successfully. Starting execution...");

						//
						// execute the first kernel, hello_kernel
						//
						error = clSetKernelArg(kernels[0], 0, sizeof(cl_mem), &msg_buffer);
						if (error == CL_SUCCESS)
						{
							kernel_output.appendLine("Kernel 0...");
							error = clEnqueueTask(cq, kernels[0], 0, NULL, NULL);
							clEnqueueReadBuffer(cq, msg_buffer, CL_TRUE, 0, sizeof(msg), &msg, 0, NULL, NULL);
							kernel_output.appendLine("Expected result: \"Hello World!!!\".  Actual result:");
							kernel_output.appendLine(msg);

						}

						// 
						// execute the second kernel, goodbye_kernel
						//
						error = clSetKernelArg(kernels[1], 0, sizeof(cl_mem), &msg_buffer);
						if (error == CL_SUCCESS)
						{
							kernel_output.appendLine("Kernel 1...");
							error = clEnqueueTask(cq, kernels[1], 0, NULL, NULL);
							clEnqueueReadBuffer(cq, msg_buffer, CL_TRUE, 0, sizeof(msg), &msg, 0, NULL, NULL);
							kernel_output.appendLine("Expected result: \"Hello World!!!\".  Actual result:");
							kernel_output.appendLine(msg);
						}
						clReleaseCommandQueue(cq);
					}
					clReleaseMemObject(msg_buffer);

					// release each kernel
					for (i = 0; i < num_kernels_ret; ++i)
					{
						clReleaseKernel(kernels[i]);
					}
				}
			}
			// release the program
			clReleaseProgram(program);
		}
		// release the context
		clReleaseContext(context);
	}

	//
	// create a kernel for ALL devices
	//
	kernel_output.appendLine("Starting Kernel creation on ALL Devices..");

	// create a Context for the Program
	context = clCreateContext(NULL, num_devices, devices, NULL, NULL, &error);
	if (error == CL_SUCCESS)
	{
		cl_program program;
		program = clCreateProgramWithSource(context, 1, (const char **)kernel_program_text, kernel_program_length, &error);
		if (error == CL_SUCCESS)
		{
			cl_int build_error = clBuildProgram(program, num_devices, devices, NULL, NULL, NULL);

			// get the build log for EACH DEVICE
			char build_log[1024];
			size_t size_ret;
			int d;
			for (d = 0; d < num_devices; ++d)
			{
				// always get the build log
				error = clGetProgramBuildInfo(program, devices[d], CL_PROGRAM_BUILD_LOG, sizeof(build_log), build_log, &size_ret);
			}

			if (build_error == CL_SUCCESS)
			{
				error = clCreateKernelsInProgram(program, MAX_KERNELS, kernels, &num_kernels_ret);
				if (error == CL_SUCCESS)
				{
					int i;

					// get information for eack kernel
					for (i = 0; i < num_kernels_ret; ++i)
					{
						char function_name[128];
						char attributes[128];
						cl_uint number_args;

						size_t ret_size;

						clGetKernelInfo(kernels[i], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
						clGetKernelInfo(kernels[i], CL_KERNEL_NUM_ARGS, sizeof(number_args), &number_args, &ret_size);
						clGetKernelInfo(kernels[i], CL_KERNEL_ATTRIBUTES, sizeof(attributes), attributes, &ret_size);
					}

					//
					// execute each kernel on each device
					//

					// create a buffer for kernel argument passing
					char msg[16];
					cl_mem msg_buffer;
					msg_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(msg), NULL, &error);

					kernel_output.appendLine("Kernels created successfully. Starting execution...");

					for (d = 0; d < num_devices; ++d)
					{
						char buf[32];
						wsprintf(buf, "Device %d", d);
						kernel_output.appendLine(buf);

						// create a command queue for the currently-selected device
						cl_command_queue cq = clCreateCommandQueue(context, devices[d], 0, &error);
						if (error == CL_SUCCESS)
						{

							//
							// execute the first kernel, hello_kernel
							//
							error = clSetKernelArg(kernels[0], 0, sizeof(cl_mem), &msg_buffer);
							if (error == CL_SUCCESS)
							{
								kernel_output.appendLine("Kernel 0...");
								error = clEnqueueTask(cq, kernels[0], 0, NULL, NULL);
								clEnqueueReadBuffer(cq, msg_buffer, CL_TRUE, 0, sizeof(msg), &msg, 0, NULL, NULL);
								kernel_output.appendLine("Expected result: \"Hello World!!!\".  Actual result:");
								kernel_output.appendLine(msg);

							}

							// 
							// execute the second kernel, goodbye_kernel
							//
							error = clSetKernelArg(kernels[1], 0, sizeof(cl_mem), &msg_buffer);
							if (error == CL_SUCCESS)
							{
								error = clEnqueueTask(cq, kernels[1], 0, NULL, NULL);
								clEnqueueReadBuffer(cq, msg_buffer, CL_TRUE, 0, sizeof(msg), &msg, 0, NULL, NULL);
								kernel_output.appendLine("Expected result: \"Goodbye World!\".  Actual result:");
								kernel_output.appendLine(msg);
							}
							clReleaseCommandQueue(cq);
						}
					}

					// release each kernel
					for (i = 0; i < num_kernels_ret; ++i)
					{
						clReleaseKernel(kernels[i]);
					}
				}
			}
			// release the program
			clReleaseProgram(program);
		}
		// release the context
		clReleaseContext(context);
	}
	return false;
}
