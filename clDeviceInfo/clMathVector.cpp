#include "stdafx.h"

static char *math_program_text[1];
static size_t math_program_length[1];
TOutput math_vector_output;

bool test_math_vector(int iPlatformIndex, int iDeviceIndex)
{
	cl_int error;
	cl_context context;
	static const int MAX_KERNELS = 32;
	cl_kernel kernels[MAX_KERNELS];
	cl_uint num_kernels_ret;

	math_vector_output.appendLine("Starting Vector Math test:");

	// load the program
	math_vector_output.appendLine("Loading Math-Vector program...");
	char szProgramPath[_MAX_PATH + 1];
	static char *szProgramName = "test_math_vector.cl";
	if (false == make_program_path(szProgramPath, _countof(szProgramPath), szProgramName))
		return false;
	math_vector_output.appendLine(szProgramPath);

	if (false == load_program(szProgramPath, &math_program_text[0], &math_program_length[0]))
	{
		char szBuf[128];
		sprintf_s(szBuf, _countof(szBuf), "Unable to load \"%s\" from file.", szProgramName);
		math_vector_output.appendLine(szBuf);
		return false;
	}


	//
	// create a kernel for ALL devices
	//
	math_vector_output.appendLine("Starting Kernel creation on ALL Devices..");


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
			char build_log[10240];
			size_t size_ret;
			int d;
			for (d = 0; d < (int)num_devices; ++d)
			{
				// get the device name
				clGetDeviceInfo(devices[d], CL_DEVICE_NAME, sizeof(build_log), build_log, &size_ret);
				math_vector_output.appendLine(build_log);

				// get the build log
				error = clGetProgramBuildInfo(program, devices[d], CL_PROGRAM_BUILD_LOG, 0, NULL, &size_ret);
				if (error == CL_SUCCESS){
					char *ptr = (char*)calloc(size_ret + 1, 1);
					if (ptr != NULL)
					{
						error = clGetProgramBuildInfo(program, devices[d], CL_PROGRAM_BUILD_LOG, size_ret, ptr, &size_ret);
						if (size_ret > 0)
						{
							math_vector_output.appendMultiLine(ptr);
						}
						free(ptr);
					}
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
					char char16_in[16] = { 'H', 'e', 'l', 'l', 'o', ' ', 'T', 'h', 'e', 'r', 'e', ' ', 'B', 'o', 'b', '\0' },
						char16_out[16] = { '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' };

					short int short8_in[8] = { 0, 1, 2, 3, 127, 126, 125, 124 },
						short8_out[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

					int int4_in[4] = { 1, 2, MAXINT - 1, MAXINT },
						int4_out[4] = { 0, 0, 0, 0 };

					cl_uint8 uint8_in = { 0, 0, 0, 0, 0, 0, 0, 0 };

					cl_long2 long2_in = { 0, MAXLONG },
						long2_out = { 0, 0 };

					float float4_in[4] = { 1.0, 2.0, CL_MAXFLOAT - 1, CL_MAXFLOAT },
						float4_out[4] = { 0, 0, 0, 0 };

					cl_float4 float4_in_test = { 9.0f, 8.0f, 7.0f, 6.0f };
					cl_float8 float8_out = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

					cl_float4 vec_in1, vec_in2;
					cl_float float_out1, float_out2;
					cl_float4 float4_out1;


					cl_mem char16_in_buf, char16_out_buf,
						short8_in_buf, short8_out_buf,
						int4_in_buf, int4_out_buf,
						long2_in_buf, long2_out_buf,
						float4_in_buf, float4_out_buf,
						vec_in_buf1, vec_in_buf2,
						float_out_buf1, float_out_buf2,
						float4_out_buf1,
						float8_out_buf,
						uint8_in_buf,
						float4_in_test_buf;

					char16_in_buf = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(char16_in), char16_in, &error);
					short8_in_buf = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(short8_in), short8_in, &error);
					int4_in_buf = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int4_in), int4_in, &error);
					uint8_in_buf = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(uint8_in), &uint8_in, &error);
					long2_in_buf = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(long2_in), &long2_in, &error);
					float4_in_buf = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float4_in), float4_in, &error);
					float4_in_test_buf = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float4_in_test), &float4_in_test, &error);
					vec_in_buf1 = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(vec_in1), &vec_in1, &error);
					vec_in_buf2 = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(vec_in2), &vec_in2, &error);

					char16_out_buf = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(char16_out), NULL, &error);
					short8_out_buf = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(short8_out), NULL, &error);
					int4_out_buf = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int4_out), NULL, &error);
					long2_out_buf = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(long2_out), NULL, &error);
					float4_out_buf = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float4_out), NULL, &error);
					float8_out_buf = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float8_out), NULL, &error);
					float_out_buf1 = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float_out1), NULL, &error);
					float_out_buf2 = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float_out2), NULL, &error);
					float4_out_buf1 = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float4_out1), NULL, &error);

					size_t ret_size;

					// create a buffer for kernel argument passing
					math_vector_output.appendLine("Kernels created successfully. Starting execution...");

					for (d = 0; d < num_devices; ++d)
					{
						char buf[128];
						int k = 0; // which kernel
						wsprintf(buf, "Device %d", d);
						math_vector_output.appendLine(buf);
						// create a command queue for the currently-selected device
						cl_command_queue cq = clCreateCommandQueue(context, devices[d], 0, &error);
						if (error == CL_SUCCESS)
						{

							//
							// Do some vector manipulation
							//
							k = 0;
							clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
							wsprintf(buf, "Kernel: %s...", function_name);
							math_vector_output.appendLine(buf);

							error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &char16_out_buf);
							error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &short8_out_buf);
							error = clSetKernelArg(kernels[k], 2, sizeof(cl_mem), &int4_out_buf);
							error = clSetKernelArg(kernels[k], 3, sizeof(cl_mem), &long2_out_buf);
							error = clSetKernelArg(kernels[k], 4, sizeof(cl_mem), &float4_out_buf);

							error = clSetKernelArg(kernels[k], 5, sizeof(cl_mem), &char16_in_buf);
							error = clSetKernelArg(kernels[k], 6, sizeof(cl_mem), &short8_in_buf);
							error = clSetKernelArg(kernels[k], 7, sizeof(cl_mem), &int4_in_buf);
							error = clSetKernelArg(kernels[k], 8, sizeof(cl_mem), &long2_in_buf);
							error = clSetKernelArg(kernels[k], 9, sizeof(cl_mem), &float4_in_buf);
							if (error == CL_SUCCESS)
							{
								size_t global_size[1] = { 1 };
								error = clEnqueueNDRangeKernel(cq, kernels[k], 1, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									math_vector_output.appendLine("Success!");
									error = clEnqueueReadBuffer(cq, char16_out_buf, CL_TRUE, 0, sizeof(char16_out),		char16_out, 0, NULL, NULL);
									error = clEnqueueReadBuffer(cq, short8_out_buf, CL_TRUE, 0, sizeof(short8_out),		short8_out, 0, NULL, NULL);
									error = clEnqueueReadBuffer(cq, int4_out_buf, CL_TRUE, 0,	sizeof(int4_out),		int4_out, 0, NULL, NULL);
									error = clEnqueueReadBuffer(cq, long2_out_buf, CL_TRUE, 0,	sizeof(long2_out),		&long2_out, 0, NULL, NULL);
									error = clEnqueueReadBuffer(cq, float4_out_buf, CL_TRUE, 0, sizeof(float4_out),		float4_out, 0, NULL, NULL);
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									math_vector_output.appendLine(buf);
								}
							}

							//
							// compute the DOT PRODUCT and CROSS PRODUCT
							//
							k = 1;
							clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
							wsprintf(buf, "Kernel: %s...", function_name);
							math_vector_output.appendLine(buf);

							vec_in1.s0 = 1.23f;	vec_in1.s1 = 2.34f;	vec_in1.s2 = 3.45f;	vec_in1.s3 = 4.56f;
							vec_in2.s0 = 9.0f;	vec_in2.s1 = 8.0f;	vec_in2.s2 = 7.0f;	vec_in2.s3 = 6.0f;

							error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &float4_out_buf1);
							error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &float_out_buf2);
							error = clSetKernelArg(kernels[k], 2, sizeof(vec_in1), &vec_in1);
							error = clSetKernelArg(kernels[k], 3, sizeof(vec_in2), &vec_in2);


							if (error == CL_SUCCESS)
							{
								size_t global_size[1] = { 1 };
								error = clEnqueueNDRangeKernel(cq, kernels[k], 1, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									math_vector_output.appendLine("Success!");
									error = clEnqueueReadBuffer(cq, float4_out_buf1, CL_TRUE, 0, sizeof(float4_out1), &float4_out1, 0, NULL, NULL);
									error = clEnqueueReadBuffer(cq, float_out_buf2, CL_TRUE, 0, sizeof(float_out2), &float_out2, 0, NULL, NULL);
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									math_vector_output.appendLine(buf);
								}
							}

							//
							// compute the DISTANCE (float), LENGTH (float), and NORMALIZE (vector)
							//
							k = 2;
							clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
							wsprintf(buf, "Kernel: %s...", function_name);
							math_vector_output.appendLine(buf);

							vec_in1.s0 = 1.23f;	vec_in1.s1 = 2.34f;	vec_in1.s2 = 3.45f;	vec_in1.s3 = 4.56f;
							vec_in2.s0 = 9.0f;	vec_in2.s1 = 8.0f;	vec_in2.s2 = 7.0f;	vec_in2.s3 = 6.0f;

							error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &float_out_buf1); // distance
							error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &float_out_buf2); // length
							error = clSetKernelArg(kernels[k], 2, sizeof(cl_mem), &float4_out_buf1); // NORMALIZE (vector 1)
							error = clSetKernelArg(kernels[k], 3, sizeof(vec_in1), &vec_in1);
							error = clSetKernelArg(kernels[k], 4, sizeof(vec_in2), &vec_in2);


							if (error == CL_SUCCESS)
							{
								size_t global_size[1] = { 1 };
								error = clEnqueueNDRangeKernel(cq, kernels[k], 1, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									math_vector_output.appendLine("Success!");
									error = clEnqueueReadBuffer(cq, float_out_buf1, CL_TRUE, 0, sizeof(float_out1), &float_out1, 0, NULL, NULL); // distance
									error = clEnqueueReadBuffer(cq, float_out_buf2, CL_TRUE, 0, sizeof(float_out2), &float_out2, 0, NULL, NULL); // length
									error = clEnqueueReadBuffer(cq, float4_out_buf1, CL_TRUE, 0, sizeof(float4_out1), &float4_out1, 0, NULL, NULL); // normalize
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									math_vector_output.appendLine(buf);
								}
							}


							//
							// compute a SHUFFLE operation
							//
							k = 3;
							clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
							wsprintf(buf, "Kernel: %s...", function_name);
							math_vector_output.appendLine(buf);

							uint8_in.s0 = 3;
							uint8_in.s1 = 1;
							uint8_in.s2 = 3;
							uint8_in.s3 = 0;
							uint8_in.s4 = 2;
							uint8_in.s5 = 1;
							uint8_in.s6 = 0;
							uint8_in.s7 = 2;

							float4_in_test.s0 = 1.23f;
							float4_in_test.s1 = 2.34f;
							float4_in_test.s2 = 3.45f;
							float4_in_test.s3 = 4.56f;



							if (error == CL_SUCCESS)
							{
								size_t global_size[1] = { 1 };
								error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &float8_out_buf); // result
								error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &uint8_in_buf); // mask
								error = clSetKernelArg(kernels[k], 2, sizeof(cl_mem), &float4_in_test_buf); // input

								error = clEnqueueWriteBuffer(cq, float4_in_test_buf, CL_TRUE, 0, sizeof(float4_in_test), &float4_in_test, 0, NULL, NULL);
								error = clEnqueueWriteBuffer(cq, uint8_in_buf, CL_TRUE, 0, sizeof(uint8_in), &uint8_in, 0, NULL, NULL);


								error = clEnqueueNDRangeKernel(cq, kernels[k], 1, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									math_vector_output.appendLine("Success!");
									error = clEnqueueReadBuffer(cq, float8_out_buf, CL_TRUE, 0, sizeof(float8_out), &float8_out, 0, NULL, NULL); // output
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									math_vector_output.appendLine(buf);
								}
							}


							clReleaseCommandQueue(cq);
						}
					}

					clReleaseMemObject(char16_in_buf); 
					clReleaseMemObject(char16_out_buf);
					clReleaseMemObject(short8_in_buf); 
					clReleaseMemObject(short8_out_buf);
					clReleaseMemObject(int4_in_buf); 
					clReleaseMemObject(int4_out_buf);
					clReleaseMemObject(long2_in_buf); 
					clReleaseMemObject(long2_out_buf);
					clReleaseMemObject(float4_in_buf); 
					clReleaseMemObject(float4_out_buf);

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
