#include "stdafx.h"

static char *buffer_program_text[1];
static size_t buffer_program_length[1];
TOutput buffer_output;

bool test_buffers(int iPlatformIndex, int iDeviceIndex)
{
	cl_int error;
	cl_context context;
	static const int MAX_KERNELS = 32;
	cl_kernel kernels[MAX_KERNELS];
	cl_uint num_kernels_ret;

	buffer_output.appendLine("Starting Buffer test:");

	// load the program
	buffer_output.appendLine("Loading Buffer program...");
	char szProgramPath[_MAX_PATH + 1];
	static char *szProgramName = "test_memflags.cl";
	if (false == make_program_path(szProgramPath, _countof(szProgramPath), szProgramName))
		return false;
	buffer_output.appendLine(szProgramPath);

	if (false == load_program(szProgramPath, &buffer_program_text[0], &buffer_program_length[0]))
	{
		char szBuf[128];
		sprintf_s(szBuf, _countof(szBuf), "Unable to load \"%s\" from file.", szProgramName);
		buffer_output.appendLine(szBuf);
		return false;
	}

	//
	// create a kernel for ALL devices
	//
	buffer_output.appendLine("Starting Kernel creation on ALL Devices..");


	char function_name[128];
	char attributes[128];
	cl_uint number_args;

	// create a Context for the Program
	context = clCreateContext(NULL, num_devices, devices, NULL, NULL, &error);
	if (error == CL_SUCCESS)
	{
		cl_program program;
		program = clCreateProgramWithSource(context, 1, (const char **)buffer_program_text, buffer_program_length, &error);
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

					// create a buffer for kernel argument passing
					char msg[16];
					cl_mem msg_buffer;
					buffer_output.appendLine("Kernels created successfully. Starting execution...");

					for (d = 0; d < num_devices; ++d)
					{
						char buf[128];
						wsprintf(buf, "Device %d", d);
						buffer_output.appendLine(buf);
						// create a command queue for the currently-selected device
						cl_command_queue cq = clCreateCommandQueue(context, devices[d], 0, &error);
						if (error == CL_SUCCESS)
						{
							
							//
							// test a read-write buffer
							//
							buffer_output.appendLine("testing WRITE to a READ_WRITE buffer. Should SUCCEED..");
							msg_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(msg), NULL, &error);
							error = clSetKernelArg(kernels[0], 0, sizeof(cl_mem), &msg_buffer);
							if (error == CL_SUCCESS)
							{
								size_t ret_size;
								clGetKernelInfo(kernels[0], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
								wsprintf(buf, "Kernel: %s...", function_name);
								buffer_output.appendLine(buf);
								error = clEnqueueTask(cq, kernels[0], 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									buffer_output.appendLine("Success!");
									clEnqueueReadBuffer(cq, msg_buffer, CL_TRUE, 0, sizeof(msg), &msg, 0, NULL, NULL);
									buffer_output.appendLine("Expected result: \"Hello World!!!\".  Actual result:");
									buffer_output.appendLine(msg);
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									buffer_output.appendLine(buf);
								}
							}
							clReleaseMemObject(msg_buffer);

							//
							// test a write to a write-only buffer
							//
							buffer_output.appendLine("testing WRITE to a WRITE_ONLY buffer. Should SUCCEED..");
							msg_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(msg), NULL, &error);
							error = clSetKernelArg(kernels[1], 0, sizeof(cl_mem), &msg_buffer);
							if (error == CL_SUCCESS)
							{
								size_t ret_size;
								clGetKernelInfo(kernels[1], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
								wsprintf(buf, "Kernel: %s...", function_name);
								buffer_output.appendLine(buf);
								error = clEnqueueTask(cq, kernels[1], 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									buffer_output.appendLine("Success!");
									clEnqueueReadBuffer(cq, msg_buffer, CL_TRUE, 0, sizeof(msg), &msg, 0, NULL, NULL);
									buffer_output.appendLine("Expected result: \"Hello World!!!\".  Actual result:");
									buffer_output.appendLine(msg);
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									buffer_output.appendLine(buf);
								}
							}
							clReleaseMemObject(msg_buffer);

							//
							// test a write to a read-only buffer
							//
							buffer_output.appendLine("testing WRITE to a READ_ONLY buffer. Should FAIL..");
							msg_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(msg), NULL, &error);
							error = clSetKernelArg(kernels[2], 0, sizeof(cl_mem), &msg_buffer);
							if (error == CL_SUCCESS)
							{
								size_t ret_size;
								clGetKernelInfo(kernels[2], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
								wsprintf(buf, "Kernel: %s...", function_name);
								buffer_output.appendLine(buf);
								error = clEnqueueTask(cq, kernels[2], 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									buffer_output.appendLine("Success!");
									clEnqueueReadBuffer(cq, msg_buffer, CL_TRUE, 0, sizeof(msg), &msg, 0, NULL, NULL);
									buffer_output.appendLine("Expected result: \"Hello World!!!\".  Actual result:");
									buffer_output.appendLine(msg);
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									buffer_output.appendLine(buf);
								}
							}
							clReleaseMemObject(msg_buffer);

							//
							// test adding two vectors
							//
							buffer_output.appendLine("Testing adding a constant to each element in a vector.");
							float source_vec[16] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };
							float dest_vec[16];
							cl_mem source_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(source_vec), source_vec, &error);
							cl_mem dest_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(dest_vec), NULL, &error);

							error = clSetKernelArg(kernels[6], 0, sizeof(cl_mem), &dest_buffer);
							error = clSetKernelArg(kernels[6], 1, sizeof(cl_mem), &source_buffer);
							if (error == CL_SUCCESS)
							{
								size_t ret_size;
								clGetKernelInfo(kernels[6], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
								wsprintf(buf, "Kernel: %s...", function_name);
								buffer_output.appendLine(buf);
								size_t global_size[1] = { 16 };
								size_t local_size[1] = { 1 };
								error = clEnqueueNDRangeKernel(cq, kernels[6], 1, NULL, global_size, NULL, 0, NULL, NULL);
//								error = clEnqueueTask(cq, kernels[2], 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									buffer_output.appendLine("Success!");
									clEnqueueReadBuffer(cq, dest_buffer, CL_TRUE, 0, sizeof(dest_vec), dest_vec, 0, NULL, NULL);
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									buffer_output.appendLine(buf);
								}
							}
							clReleaseMemObject(dest_buffer);
							clReleaseMemObject(source_buffer);

							//
							// test a 2-dimensional buffer
							//
							buffer_output.appendLine("Testing filling a 2-d buffer");
							const int SIZE_X = 2048;
							const int SIZE_Y = 2048;
							cl_uint *dest_vec2d=(cl_uint*)malloc(SIZE_X * SIZE_Y * sizeof(cl_uint));
							if (dest_vec2d != NULL)
							{

								cl_image_format img_fmt;
								img_fmt.image_channel_order = CL_RGBA;
								img_fmt.image_channel_data_type = CL_UNSIGNED_INT8;

								//							dest_buffer = clCreateImage2D(context, CL_MEM_WRITE_ONLY, &img_fmt, SIZE_X, SIZE_Y, 0, (void*)dest_vec2d, &error);
								dest_buffer = clCreateImage2D(context, CL_MEM_WRITE_ONLY, &img_fmt, SIZE_X, SIZE_Y, 0, NULL, &error);

								error = clSetKernelArg(kernels[7], 0, sizeof(cl_mem), &dest_buffer);
								if (error == CL_SUCCESS)
								{
									size_t ret_size;
									clGetKernelInfo(kernels[7], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
									wsprintf(buf, "Kernel: %s...", function_name);
									buffer_output.appendLine(buf);
									size_t global_size[2] = { SIZE_X, SIZE_Y };
									error = clEnqueueNDRangeKernel(cq, kernels[7], 2, NULL, global_size, NULL, 0, NULL, NULL);
									if (error == CL_SUCCESS)
									{
										buffer_output.appendLine("Success!");
										clEnqueueReadBuffer(cq, dest_buffer, CL_TRUE, 0, sizeof(dest_vec2d), dest_vec2d, 0, NULL, NULL);
									}
									else
									{
										wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
										buffer_output.appendLine(buf);
									}
								}
								clReleaseMemObject(dest_buffer);
								free(dest_vec2d);
							}


							buffer_output.appendLine("Testing filling a 2-d buffer");
							cl_uint *src_vec2d = (cl_uint*)malloc(SIZE_X * SIZE_Y * sizeof(cl_uint));
							dest_vec2d = (cl_uint*)malloc(SIZE_X * SIZE_Y * sizeof(cl_uint));
							if (src_vec2d != NULL)
							{
								cl_image_format img_fmt;
								img_fmt.image_channel_order = CL_RGBA;
								img_fmt.image_channel_data_type = CL_UNSIGNED_INT8;

								for (int i = 0; i < SIZE_X * SIZE_Y; ++i)
								{
									src_vec2d[i] = (i << 24) + ((i * 2) << 16) + ((i * 3) << 8) + i;
									dest_vec2d[i] = 0;
								}

								//							dest_buffer = clCreateImage2D(context, CL_MEM_WRITE_ONLY, &img_fmt, SIZE_X, SIZE_Y, 0, (void*)dest_vec2d, &error);
								cl_mem dest_bufferc = clCreateImage2D(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, &img_fmt, SIZE_X, SIZE_Y, 0, (void*)dest_vec2d, &error);
								cl_mem src_bufferc = clCreateImage2D(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, &img_fmt, SIZE_X, SIZE_Y, 0, (void*)src_vec2d, &error);

								error = clSetKernelArg(kernels[8], 0, sizeof(cl_mem), &src_bufferc);
								error = clSetKernelArg(kernels[8], 1, sizeof(cl_mem), &dest_bufferc);
								if (error == CL_SUCCESS)
								{
									size_t ret_size;
									clGetKernelInfo(kernels[8], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
									wsprintf(buf, "Kernel: %s...", function_name);
									buffer_output.appendLine(buf);
									size_t global_size[2] = { SIZE_X, SIZE_Y };
									error = clEnqueueNDRangeKernel(cq, kernels[8], 2, NULL, global_size, NULL, 0, NULL, NULL);
									//								error = clEnqueueTask(cq, kernels[2], 0, NULL, NULL);
									if (error == CL_SUCCESS)
									{
										buffer_output.appendLine("Success!");
										size_t origin[3] = { 0, 0, 0 };
										size_t region[3] = { SIZE_X, SIZE_Y, 1 };
										clEnqueueReadImage(cq, dest_bufferc, CL_TRUE, origin, region, 0, 0, dest_vec2d, 0, NULL, NULL);
									}
									else
									{
										wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
										buffer_output.appendLine(buf);
									}
								}
								clReleaseMemObject(src_bufferc);
								clReleaseMemObject(dest_bufferc);
								free(src_vec2d);
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
