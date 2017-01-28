#include "stdafx.h"

static char *math_program_text[1];
static size_t math_program_length[1];
TOutput math_integer_output;

bool test_math_integer(int iPlatformIndex, int iDeviceIndex)
{
	cl_int error;
	cl_context context;
	static const int MAX_KERNELS = 32;
	cl_kernel kernels[MAX_KERNELS];
	cl_uint num_kernels_ret;

	math_integer_output.appendLine("Starting Integer Math test:");

	// load the program
	math_integer_output.appendLine("Loading Math-Integer program...");
	char szProgramPath[_MAX_PATH + 1];
	static char *szProgramName = "test_math_integer.cl";
	if (false == make_program_path(szProgramPath, _countof(szProgramPath), szProgramName))
		return false;
	math_integer_output.appendLine(szProgramPath);

	if (false == load_program(szProgramPath, &math_program_text[0], &math_program_length[0]))
	{
		char szBuf[128];
		sprintf_s(szBuf, _countof(szBuf), "Unable to load \"%s\" from file.", szProgramName);
		math_integer_output.appendLine(szBuf);
		return false;
	}

	//
	// create a kernel for ALL devices
	//
	math_integer_output.appendLine("Starting Kernel creation on ALL Devices..");


	char function_name[128];
	char attributes[128];
	cl_uint number_args;

	// create a Context for the Program
	context = clCreateContext(NULL, num_devices, devices, NULL, NULL, &error);
	if (error == CL_SUCCESS)
	{
		cl_program program;
		program = clCreateProgramWithSource(context, 1, (const char **)math_program_text, math_program_length, &error);
		if (error == CL_SUCCESS)
		{
			cl_int build_error = clBuildProgram(program, num_devices, devices, NULL, NULL, NULL);

			// get the build log for EACH DEVICE
			char build_log[1024];
			size_t size_ret;
			int d;
			for (d = 0; d < num_devices; ++d)
			{
				// get the device name
				clGetDeviceInfo(devices[d], CL_DEVICE_NAME, sizeof(build_log), build_log, &size_ret);
				math_integer_output.appendLine(build_log);

				// get the build log
				error = clGetProgramBuildInfo(program, devices[d], CL_PROGRAM_BUILD_LOG, sizeof(build_log), build_log, &size_ret);
				if (size_ret > 0)
				{
					math_integer_output.appendMultiLine(build_log);
				}
			}

			if (build_error == CL_SUCCESS)
			{
				error = clCreateKernelsInProgram(program, MAX_KERNELS, kernels, &num_kernels_ret);
				if (error == CL_SUCCESS)
				{
					int i;

					// get information for each kernel
					for (i = 0; i < num_kernels_ret; ++i)
					{

						size_t ret_size;

						clGetKernelInfo(kernels[i], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
						clGetKernelInfo(kernels[i], CL_KERNEL_NUM_ARGS, sizeof(number_args), &number_args, &ret_size);
						clGetKernelInfo(kernels[i], CL_KERNEL_ATTRIBUTES, sizeof(attributes), attributes, &ret_size);
					}

					//
					// execute each kernel on each device
					//

					// create the input and output buffers
					const int NUMBER_POINTS = 1024;
					char ints1[NUMBER_POINTS];
					char ints2[NUMBER_POINTS];
					char ints3[NUMBER_POINTS];
					char buf1[NUMBER_POINTS];
					char buf2[NUMBER_POINTS];
					char buf3[NUMBER_POINTS];

					// create a buffer to hold the radian data
					for (int r = 0; r < NUMBER_POINTS; ++r)
					{
						ints1[r] = (char)(rand() * 1.0f / RAND_MAX * 255 - 127);
						ints2[r] = (char)(rand() * 1.0f / RAND_MAX * 255 - 127);
						ints3[r] = (char)(rand() * 1.0f / RAND_MAX * 255 - 127);
						buf1[r] = buf2[r] = buf3[r] = 0;
					}

					cl_mem ints1_buf;
					cl_mem ints2_buf;
					cl_mem ints3_buf;
					cl_mem buf1_buf;
					cl_mem buf2_buf;
					cl_mem buf3_buf;

					ints1_buf = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(ints1), ints1, &error);
					ints2_buf = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(ints2), ints2, &error);
					ints3_buf = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(ints3), ints3, &error);
					buf1_buf = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(buf1), NULL, &error);
					buf2_buf = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(buf2), NULL, &error);
					buf3_buf = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(buf3), NULL, &error);

					size_t ret_size;


					// create a buffer for kernel argument passing
					math_integer_output.appendLine("Kernels created successfully. Starting execution...");

					for (d = 0; d < num_devices; ++d)
					{
						char buf[128];
						int k = 0; // which kernel
						wsprintf(buf, "Device %d", d);
						math_integer_output.appendLine(buf);
						// create a command queue for the currently-selected device
						cl_command_queue cq = clCreateCommandQueue(context, devices[d], 0, &error);
						if (error == CL_SUCCESS)
						{

							//
							// calculate the ABS and ABS_DIFF function
							//
							k = 0;
							clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
							wsprintf(buf, "Kernel: %s...", function_name);
							math_integer_output.appendLine(buf);

							error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &buf1_buf); // abs data
							error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &buf2_buf); // abs_diff data
							error = clSetKernelArg(kernels[k], 2, sizeof(cl_mem), &ints1_buf);
							error = clSetKernelArg(kernels[k], 3, sizeof(cl_mem), &ints2_buf);
							if (error == CL_SUCCESS)
							{
								size_t global_size[1] = { NUMBER_POINTS };
								error = clEnqueueNDRangeKernel(cq, kernels[k], 1, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									math_integer_output.appendLine("Success!");
									error = clEnqueueReadBuffer(cq, buf1_buf, CL_TRUE, 0, sizeof(buf1), buf1, 0, NULL, NULL);
									error = clEnqueueReadBuffer(cq, buf2_buf, CL_TRUE, 0, sizeof(buf2), buf2, 0, NULL, NULL);
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									math_integer_output.appendLine(buf);
								}
							}

							//
							// calculate the ADD_SAT and SUB_SAT functions
							//
							k = 1;
							clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
							wsprintf(buf, "Kernel: %s...", function_name);
							math_integer_output.appendLine(buf);

							error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &buf1_buf); // add_sat data
							error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &buf2_buf); // sub_sat data
							error = clSetKernelArg(kernels[k], 2, sizeof(cl_mem), &ints1_buf);
							error = clSetKernelArg(kernels[k], 3, sizeof(cl_mem), &ints2_buf);
							if (error == CL_SUCCESS)
							{
								size_t global_size[1] = { NUMBER_POINTS };
								error = clEnqueueNDRangeKernel(cq, kernels[k], 1, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									math_integer_output.appendLine("Success!");
									error = clEnqueueReadBuffer(cq, buf1_buf, CL_TRUE, 0, sizeof(buf1), buf1, 0, NULL, NULL);
									error = clEnqueueReadBuffer(cq, buf2_buf, CL_TRUE, 0, sizeof(buf2), buf2, 0, NULL, NULL);
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									math_integer_output.appendLine(buf);
								}
							}

							//
							// calculate the HADD and RHADD functions
							//
							k = 2;
							clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
							wsprintf(buf, "Kernel: %s...", function_name);
							math_integer_output.appendLine(buf);

							error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &buf1_buf); // hadd data
							error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &buf2_buf); // rhadd data
							error = clSetKernelArg(kernels[k], 2, sizeof(cl_mem), &ints1_buf);
							error = clSetKernelArg(kernels[k], 3, sizeof(cl_mem), &ints2_buf);
							if (error == CL_SUCCESS)
							{
								size_t global_size[1] = { NUMBER_POINTS };
								error = clEnqueueNDRangeKernel(cq, kernels[k], 1, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									math_integer_output.appendLine("Success!");
									error = clEnqueueReadBuffer(cq, buf1_buf, CL_TRUE, 0, sizeof(buf1), buf1, 0, NULL, NULL);
									error = clEnqueueReadBuffer(cq, buf2_buf, CL_TRUE, 0, sizeof(buf2), buf2, 0, NULL, NULL);
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									math_integer_output.appendLine(buf);
								}
							}

							//
							// calculate the CLZ functions
							//
							k = 3;
							clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
							wsprintf(buf, "Kernel: %s...", function_name);
							math_integer_output.appendLine(buf);

							error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &buf1_buf); // clz data
							error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &ints1_buf);
							if (error == CL_SUCCESS)
							{
								size_t global_size[1] = { NUMBER_POINTS };
								error = clEnqueueNDRangeKernel(cq, kernels[k], 1, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									math_integer_output.appendLine("Success!");
									error = clEnqueueReadBuffer(cq, buf1_buf, CL_TRUE, 0, sizeof(buf1), buf1, 0, NULL, NULL);
									error = clEnqueueReadBuffer(cq, buf2_buf, CL_TRUE, 0, sizeof(buf2), buf2, 0, NULL, NULL);
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									math_integer_output.appendLine(buf);
								}
							}


							//
							// calculate the MAD_HI and MAD_SAT functions
							//
							k = 4;
							clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
							wsprintf(buf, "Kernel: %s...", function_name);
							math_integer_output.appendLine(buf);

							error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &buf1_buf); // hadd data
							error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &buf2_buf); // rhadd data
							error = clSetKernelArg(kernels[k], 2, sizeof(cl_mem), &ints1_buf);
							error = clSetKernelArg(kernels[k], 3, sizeof(cl_mem), &ints2_buf);
							error = clSetKernelArg(kernels[k], 4, sizeof(cl_mem), &ints3_buf);
							if (error == CL_SUCCESS)
							{
								size_t global_size[1] = { NUMBER_POINTS };
								error = clEnqueueNDRangeKernel(cq, kernels[k], 1, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									math_integer_output.appendLine("Success!");
									error = clEnqueueReadBuffer(cq, buf1_buf, CL_TRUE, 0, sizeof(buf1), buf1, 0, NULL, NULL);
									error = clEnqueueReadBuffer(cq, buf2_buf, CL_TRUE, 0, sizeof(buf2), buf2, 0, NULL, NULL);
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									math_integer_output.appendLine(buf);
								}
							}
							clReleaseCommandQueue(cq);
						}
					}

					clReleaseMemObject(ints1_buf);
					clReleaseMemObject(ints2_buf);
					clReleaseMemObject(ints3_buf);
					clReleaseMemObject(buf1_buf);
					clReleaseMemObject(buf2_buf);
					clReleaseMemObject(buf3_buf);

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
