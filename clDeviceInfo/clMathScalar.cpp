#include "stdafx.h"

static char *math_program_text[1];
static size_t math_program_length[1];
TOutput math_scalar_output;

bool test_math_scalar(int iPlatformIndex, int iDeviceIndex)
{
	cl_int error;
	cl_context context;
	static const int MAX_KERNELS = 32;
	cl_kernel kernels[MAX_KERNELS];
	cl_uint num_kernels_ret;

	math_scalar_output.appendLine("Starting Scalar Math test:");

	// load the program
	math_scalar_output.appendLine("Loading Math-Scalar program...");
	char szProgramPath[_MAX_PATH + 1];
	static char *szProgramName = "test_math_scalar.cl";
	if (false == make_program_path(szProgramPath, _countof(szProgramPath), szProgramName))
		return false;
	math_scalar_output.appendLine(szProgramPath);

	if (false == load_program(szProgramPath, &math_program_text[0], &math_program_length[0]))
	{
		char szBuf[128];
		sprintf_s(szBuf, _countof(szBuf), "Unable to load \"%s\" from file.", szProgramName);
		math_scalar_output.appendLine(szBuf);
		return false;
	}


	//
	// create a kernel for ALL devices
	//
	math_scalar_output.appendLine("Starting Kernel creation on ALL Devices..");


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
				math_scalar_output.appendLine(build_log);

				// get the build log
				error = clGetProgramBuildInfo(program, devices[d], CL_PROGRAM_BUILD_LOG, sizeof(build_log), build_log, &size_ret);
				if (size_ret > 0)
				{
					math_scalar_output.appendMultiLine(build_log);
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
					float rads[NUMBER_POINTS];
					float pifrac[NUMBER_POINTS]; // 0.0-2.0
					float exponents[NUMBER_POINTS]; // 0.0-2.0
					float buf1[NUMBER_POINTS];
					float buf2[NUMBER_POINTS];
					float buf3[NUMBER_POINTS];

					// create a buffer to hold the radian data
					for (int r = 0; r < NUMBER_POINTS; ++r)
					{
						rads[r] = r * (6.28f / NUMBER_POINTS);
						pifrac[r] = r * 2.0f / NUMBER_POINTS;
						exponents[r] = r* 0.5f / NUMBER_POINTS; // 5.0 is chosen arbitarily to provide some largish results
						buf1[r] = buf2[r] = buf3[r] = 0.0f;
					}

					cl_mem rad_buf;
					cl_mem pifrac_buf;
					cl_mem exponent_buf;
					cl_mem buf1_buf;
					cl_mem buf2_buf;
					cl_mem buf3_buf;

					rad_buf = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(rads), rads, &error);
					pifrac_buf = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(pifrac), pifrac, &error);
					exponent_buf = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(exponents), exponents, &error);
					buf1_buf = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(buf1), NULL, &error);
					buf2_buf = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(buf2), NULL, &error);
					buf3_buf = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(buf3), NULL, &error);

					size_t ret_size;


					// create a buffer for kernel argument passing
					math_scalar_output.appendLine("Kernels created successfully. Starting execution...");

					for (d = 0; d < num_devices; ++d)
					{
						char buf[128];
						int k=0; // which kernel
						wsprintf(buf, "Device %d", d);
						math_scalar_output.appendLine(buf);
						// create a command queue for the currently-selected device
						cl_command_queue cq = clCreateCommandQueue(context, devices[d], 0, &error);
						if (error == CL_SUCCESS)
						{

							//
							// calculate the SIN function
							//
							math_scalar_output.appendLine("testing SIN function.");
							error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &buf1_buf);
							error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &rad_buf);
							if (error == CL_SUCCESS)
							{
								clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
								wsprintf(buf, "Kernel: %s...", function_name);
								math_scalar_output.appendLine(buf);
								size_t global_size[1] = { NUMBER_POINTS};
								error = clEnqueueNDRangeKernel(cq, kernels[k], 1, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									math_scalar_output.appendLine("Success!");
									error = clEnqueueReadBuffer(cq, buf1_buf, CL_TRUE, 0, sizeof(buf1), buf1, 0, NULL, NULL);
									wsprintf(buf, "Rear buffer returned %d", error);
									math_scalar_output.appendLine(buf);
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									math_scalar_output.appendLine(buf);
								}
							}

							//
							// calculate the cos function
							//
							k = 1;
							math_scalar_output.appendLine("testing SIN function.");
							error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &buf1_buf);
							error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &rad_buf);
							if (error == CL_SUCCESS)
							{
								clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
								wsprintf(buf, "Kernel: %s...", function_name);
								math_scalar_output.appendLine(buf);
								size_t global_size[1] = { NUMBER_POINTS };
								error = clEnqueueNDRangeKernel(cq, kernels[k], 1, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									math_scalar_output.appendLine("Success!");
									error = clEnqueueReadBuffer(cq, buf1_buf, CL_TRUE, 0, sizeof(buf1), buf1, 0, NULL, NULL);
									wsprintf(buf, "Rear buffer returned %d", error);
									math_scalar_output.appendLine(buf);
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									math_scalar_output.appendLine(buf);
								}
							}
							//
							// calculate the TAN function
							//
							k = 2;
							math_scalar_output.appendLine("testing SIN function.");
							error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &buf1_buf);
							error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &rad_buf);
							if (error == CL_SUCCESS)
							{
								clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
								wsprintf(buf, "Kernel: %s...", function_name);
								math_scalar_output.appendLine(buf);
								size_t global_size[1] = { NUMBER_POINTS };
								error = clEnqueueNDRangeKernel(cq, kernels[k], 1, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									math_scalar_output.appendLine("Success!");
									error = clEnqueueReadBuffer(cq, buf1_buf, CL_TRUE, 0, sizeof(buf1), buf1, 0, NULL, NULL);
									wsprintf(buf, "Rear buffer returned %d", error);
									math_scalar_output.appendLine(buf);
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									math_scalar_output.appendLine(buf);
								}
							}

							//
							// calculate the SINH function
							//
							k = 3;
							math_scalar_output.appendLine("testing SIN function.");
							error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &buf1_buf);
							error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &rad_buf);
							if (error == CL_SUCCESS)
							{
								clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
								wsprintf(buf, "Kernel: %s...", function_name);
								math_scalar_output.appendLine(buf);
								size_t global_size[1] = { NUMBER_POINTS };
								error = clEnqueueNDRangeKernel(cq, kernels[k], 1, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									math_scalar_output.appendLine("Success!");
									error = clEnqueueReadBuffer(cq, buf1_buf, CL_TRUE, 0, sizeof(buf1), buf1, 0, NULL, NULL);
									wsprintf(buf, "Rear buffer returned %d", error);
									math_scalar_output.appendLine(buf);
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									math_scalar_output.appendLine(buf);
								}
							}

							//
							// calculate the COSH function
							//
							k = 4;
							math_scalar_output.appendLine("testing SIN function.");
							error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &buf1_buf);
							error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &rad_buf);
							if (error == CL_SUCCESS)
							{
								clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
								wsprintf(buf, "Kernel: %s...", function_name);
								math_scalar_output.appendLine(buf);
								size_t global_size[1] = { NUMBER_POINTS };
								error = clEnqueueNDRangeKernel(cq, kernels[k], 1, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									math_scalar_output.appendLine("Success!");
									error = clEnqueueReadBuffer(cq, buf1_buf, CL_TRUE, 0, sizeof(buf1), buf1, 0, NULL, NULL);
									wsprintf(buf, "Rear buffer returned %d", error);
									math_scalar_output.appendLine(buf);
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									math_scalar_output.appendLine(buf);
								}
							}
							//
							// calculate the TANH function
							//
							k = 5;
							math_scalar_output.appendLine("testing SIN function.");
							error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &buf1_buf);
							error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &rad_buf);
							if (error == CL_SUCCESS)
							{
								clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
								wsprintf(buf, "Kernel: %s...", function_name);
								math_scalar_output.appendLine(buf);
								size_t global_size[1] = { NUMBER_POINTS };
								error = clEnqueueNDRangeKernel(cq, kernels[k], 1, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									math_scalar_output.appendLine("Success!");
									error = clEnqueueReadBuffer(cq, buf1_buf, CL_TRUE, 0, sizeof(buf1), buf1, 0, NULL, NULL);
									wsprintf(buf, "Rear buffer returned %d", error);
									math_scalar_output.appendLine(buf);
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									math_scalar_output.appendLine(buf);
								}
							}

							// pi functions

							//
							// calculate the SINPI function
							//
							k = 6;
							math_scalar_output.appendLine("testing SINPI function.");
							error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &buf1_buf);
							error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &pifrac_buf);
							if (error == CL_SUCCESS)
							{
								clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
								wsprintf(buf, "Kernel: %s...", function_name);
								math_scalar_output.appendLine(buf);
								size_t global_size[1] = { NUMBER_POINTS };
								error = clEnqueueNDRangeKernel(cq, kernels[k], 1, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									math_scalar_output.appendLine("Success!");
									error = clEnqueueReadBuffer(cq, buf1_buf, CL_TRUE, 0, sizeof(buf1), buf1, 0, NULL, NULL);
									wsprintf(buf, "Read buffer returned %d", error);
									math_scalar_output.appendLine(buf);
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									math_scalar_output.appendLine(buf);
								}
							}
							//
							// calculate the COSPI function
							//
							k = 7;
							math_scalar_output.appendLine("testing COSPI function.");
							error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &buf1_buf);
							error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &pifrac_buf);
							if (error == CL_SUCCESS)
							{
								clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
								wsprintf(buf, "Kernel: %s...", function_name);
								math_scalar_output.appendLine(buf);
								size_t global_size[1] = { NUMBER_POINTS };
								error = clEnqueueNDRangeKernel(cq, kernels[k], 1, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									math_scalar_output.appendLine("Success!");
									error = clEnqueueReadBuffer(cq, buf1_buf, CL_TRUE, 0, sizeof(buf1), buf1, 0, NULL, NULL);
									wsprintf(buf, "Rear buffer returned %d", error);
									math_scalar_output.appendLine(buf);
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									math_scalar_output.appendLine(buf);
								}
							}
							//
							// calculate the TANPI function
							//
							k = 8;
							math_scalar_output.appendLine("testing TANPI function.");
							error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &buf1_buf);
							error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &pifrac_buf);
							if (error == CL_SUCCESS)
							{
								clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
								wsprintf(buf, "Kernel: %s...", function_name);
								math_scalar_output.appendLine(buf);
								size_t global_size[1] = { NUMBER_POINTS };
								error = clEnqueueNDRangeKernel(cq, kernels[k], 1, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									math_scalar_output.appendLine("Success!");
									error = clEnqueueReadBuffer(cq, buf1_buf, CL_TRUE, 0, sizeof(buf1), buf1, 0, NULL, NULL);
									wsprintf(buf, "Read buffer returned %d", error);
									math_scalar_output.appendLine(buf);
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									math_scalar_output.appendLine(buf);
								}
							}

							//
							// calculate the NATIVE SIN function
							//
							k = 9;
							math_scalar_output.appendLine("testing NATIVE SIN function.");
							error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &buf1_buf);
							error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &rad_buf);
							if (error == CL_SUCCESS)
							{
								clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
								wsprintf(buf, "Kernel: %s...", function_name);
								math_scalar_output.appendLine(buf);
								size_t global_size[1] = { NUMBER_POINTS };
								error = clEnqueueNDRangeKernel(cq, kernels[k], 1, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									error = clEnqueueReadBuffer(cq, buf1_buf, CL_TRUE, 0, sizeof(buf1), buf1, 0, NULL, NULL);
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									math_scalar_output.appendLine(buf);
								}
							}

							//
							// calculate the NATIVE COS function
							//
							k = 10;
							math_scalar_output.appendLine("testing NATIVE COS function.");
							error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &buf1_buf);
							error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &rad_buf);
							if (error == CL_SUCCESS)
							{
								clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
								wsprintf(buf, "Kernel: %s...", function_name);
								math_scalar_output.appendLine(buf);
								size_t global_size[1] = { NUMBER_POINTS };
								error = clEnqueueNDRangeKernel(cq, kernels[k], 1, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									error = clEnqueueReadBuffer(cq, buf1_buf, CL_TRUE, 0, sizeof(buf1), buf1, 0, NULL, NULL);
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									math_scalar_output.appendLine(buf);
								}
							}
							//
							// calculate the NATIVE TAN function
							//
							k = 11;
							math_scalar_output.appendLine("testing NATIVE TAN function.");
							error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &buf1_buf);
							error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &rad_buf);
							if (error == CL_SUCCESS)
							{
								clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
								wsprintf(buf, "Kernel: %s...", function_name);
								math_scalar_output.appendLine(buf);
								size_t global_size[1] = { NUMBER_POINTS };
								error = clEnqueueNDRangeKernel(cq, kernels[k], 1, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									error = clEnqueueReadBuffer(cq, buf1_buf, CL_TRUE, 0, sizeof(buf1), buf1, 0, NULL, NULL);
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									math_scalar_output.appendLine(buf);
								}
							}

							//
							// calculate the EXP functions
							//
							k = 12;
							math_scalar_output.appendLine("testing NATIVE SIN function.");
							error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &buf1_buf);
							error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &buf2_buf);
							error = clSetKernelArg(kernels[k], 2, sizeof(cl_mem), &buf3_buf);
							error = clSetKernelArg(kernels[k], 3, sizeof(cl_mem), &exponent_buf);
							if (error == CL_SUCCESS)
							{
								clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
								wsprintf(buf, "Kernel: %s...", function_name);
								math_scalar_output.appendLine(buf);
								size_t global_size[1] = { NUMBER_POINTS };
								error = clEnqueueNDRangeKernel(cq, kernels[k], 1, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									error = clEnqueueReadBuffer(cq, buf1_buf, CL_TRUE, 0, sizeof(buf1), buf1, 0, NULL, NULL);
									error = clEnqueueReadBuffer(cq, buf2_buf, CL_TRUE, 0, sizeof(buf2), buf2, 0, NULL, NULL);
									error = clEnqueueReadBuffer(cq, buf3_buf, CL_TRUE, 0, sizeof(buf3), buf3, 0, NULL, NULL);
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									math_scalar_output.appendLine(buf);
								}
							}

							//
							// calculate the SINCOS functions
							//
							k = 13;
							math_scalar_output.appendLine("testing NATIVE SIN function.");
							error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &buf1_buf);
							error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &buf2_buf);
							error = clSetKernelArg(kernels[k], 2, sizeof(cl_mem), &rad_buf);
							if (error == CL_SUCCESS)
							{
								clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
								wsprintf(buf, "Kernel: %s...", function_name);
								math_scalar_output.appendLine(buf);
								size_t global_size[1] = { NUMBER_POINTS };
								error = clEnqueueNDRangeKernel(cq, kernels[k], 1, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									error = clEnqueueReadBuffer(cq, buf1_buf, CL_TRUE, 0, sizeof(buf1), buf1, 0, NULL, NULL);
									error = clEnqueueReadBuffer(cq, buf2_buf, CL_TRUE, 0, sizeof(buf2), buf2, 0, NULL, NULL);
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									math_scalar_output.appendLine(buf);
								}
							}


							clReleaseCommandQueue(cq);
						}
					}
					clReleaseMemObject(rad_buf);
					clReleaseMemObject(pifrac_buf);
					clReleaseMemObject(exponent_buf);
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
