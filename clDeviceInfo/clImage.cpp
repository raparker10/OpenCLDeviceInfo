#include "stdafx.h"

static char *image_program_test[1];
static size_t image_program_length[1];
TOutput image_output;
TCanvas image_img(64, 64);

bool test_image(HWND hWnd, int iPlatformIndex, int iDeviceIndex)
{
	cl_int error;
	cl_context context;
	static const int MAX_KERNELS = 32;
	cl_kernel kernels[MAX_KERNELS];
	cl_uint num_kernels_ret;
	const size_t IMAGE_SMALL_WIDTH = 8;
	const size_t IMAGE_SMALL_HEIGHT = 8;
	const size_t IMAGE_LARGE_WIDTH = 16;
	const size_t IMAGE_LARGE_HEIGHT = 16;

	image_output.appendLine("Starting Vector Math test:");

	// load the program
	image_output.appendLine("Loading Image program...");
	char szProgramPath[_MAX_PATH + 1];
	static char *szProgramName = "test_image.cl";
	if (false == make_program_path(szProgramPath, _countof(szProgramPath), szProgramName))
		return false;
	image_output.appendLine(szProgramPath);

	if (false == load_program(szProgramPath, &image_program_test[0], &image_program_length[0]))
	{
		char szBuf[128];
		sprintf_s(szBuf, _countof(szBuf), "Unable to load \"%s\" from file.", szProgramName);
		image_output.appendLine(szBuf);
		return false;
	}

	//
	// create a kernel for ALL devices
	//
	image_output.appendLine("Starting Kernel creation on ALL Devices..");


	char function_name[128];
	char attributes[128];
	cl_uint number_args;

	// create a Context for the Program
	context = clCreateContext(NULL, num_devices, devices, NULL, NULL, &error);
	if (error == CL_SUCCESS)
	{
		cl_program program;
		program = clCreateProgramWithSource(context, 1, (const char **)image_program_test, image_program_length, &error);
		if (error == CL_SUCCESS)
		{
			cl_int build_error = clBuildProgram(program, num_devices, devices, NULL, NULL, NULL);

			// get the build log for EACH DEVICE
			char build_log[10240];
			size_t size_ret;
			int d;
			for (d = 0; d < num_devices; ++d)
			{
				// get the device name
				clGetDeviceInfo(devices[d], CL_DEVICE_NAME, sizeof(build_log), build_log, &size_ret);
				image_output.appendLine(build_log);

				// get the build log
				error = clGetProgramBuildInfo(program, devices[d], CL_PROGRAM_BUILD_LOG, 0, NULL, &size_ret);
				if (error == CL_SUCCESS){
					char *ptr = (char*)calloc(size_ret + 1, 1);
					if (ptr != NULL)
					{
						error = clGetProgramBuildInfo(program, devices[d], CL_PROGRAM_BUILD_LOG, size_ret, ptr, &size_ret);
						if (size_ret > 0)
						{
							image_output.appendMultiLine(ptr);
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

					size_t ret_size;

					// create a buffer for kernel argument passing
					image_output.appendLine("Kernels created successfully. Starting execution...");

					// get the list of supported image types
					const int MAX_IMAGE_FORMATS = 128;
					cl_image_format fmts[MAX_IMAGE_FORMATS];
					cl_uint num_fmts;
					error = clGetSupportedImageFormats(context, CL_MEM_WRITE_ONLY, CL_MEM_OBJECT_IMAGE2D, MAX_IMAGE_FORMATS, fmts, &num_fmts);
					if (error == CL_SUCCESS)
					{
						//
						// write the supported image formats
						//
						for (int f = 0; f < num_fmts; ++f)
						{
							char *order, *type;
							char fmtbuf[128];

							switch ((unsigned int)fmts[f].image_channel_order) {
							case CL_R: order = "CL_R"; break;
							case CL_A: order = "CL_A"; break;
							case CL_INTENSITY: order = "CL_INTENSITY"; break;
							case CL_LUMINANCE: order = "CL_LUMINANCE"; break;
							case CL_RG: order = "CL_RG"; break;
							case CL_RGB: order = "CL_RGB"; break;
							case CL_RGBA: order = "CL_RGBA"; break;
							case CL_ARGB: order = "CL_ARGB"; break;
							case CL_BGRA: order = "CL_BGRA"; break;
							}

							switch ((unsigned int)fmts[f].image_channel_data_type)
							{
							case CL_SNORM_INT8: type = "CL_SNORM_INT8"; break;
							case CL_SNORM_INT16: type = "CL_SNORM_INT16"; break;
							case CL_UNORM_INT8: type = "CL_UNORM_INT8"; break;
							case CL_UNORM_INT16: type = "CL_UNORM_INT16"; break;
							case CL_UNORM_SHORT_565: type = "CL_UNORM_SHORT_565"; break;
							case CL_UNORM_SHORT_555: type = "CL_UNORM_SHORT_555"; break;
							case CL_UNORM_INT_101010: type = "CL_UNORM_INT_101010"; break;
							case CL_SIGNED_INT8: type = "CL_SIGNED_INT8"; break;
							case CL_SIGNED_INT16: type = "CL_SIGNED_INT16"; break;
							case CL_SIGNED_INT32: type = "CL_SIGNED_INT32"; break;
							case CL_UNSIGNED_INT8: type = "CL_UNSIGNED_INT8"; break;
							case CL_UNSIGNED_INT16: type = "CL_UNSIGNED_INT16"; break;
							case CL_UNSIGNED_INT32: type = "CL_UNSIGNED_INT32"; break;
							case CL_HALF_FLOAT: type = "CL_HALF_FLOAT"; break;
							case CL_FLOAT: type = "CL_FLOAT"; break;
							}

							strcpy(fmtbuf, order);
							strcat(fmtbuf, " | ");
							strcat(fmtbuf, type);
							image_output.appendLine(fmtbuf);
						}
					}

					//
					// create the CL_INSIGNED_INT8 input and output buffers
					// 
					cl_mem image_in, image_out, image_out_large;
					cl_image_format image_in_fmt, image_out_fmt;
					cl_uchar img_in_data[IMAGE_SMALL_HEIGHT * IMAGE_SMALL_WIDTH];

					image_out_fmt.image_channel_data_type = CL_UNSIGNED_INT8;
					image_out_fmt.image_channel_order = CL_RGBA;

					image_in_fmt.image_channel_data_type = CL_UNSIGNED_INT8;
					image_in_fmt.image_channel_order = CL_RGBA;

					// write a test pattern into the input image
					memset(img_in_data, 0x0A5, sizeof(img_in_data));

					image_out_large = clCreateImage2D(context, CL_MEM_WRITE_ONLY, &image_out_fmt, IMAGE_LARGE_WIDTH, IMAGE_LARGE_HEIGHT, 0, NULL, &error);
					image_out = clCreateImage2D(context, CL_MEM_WRITE_ONLY, &image_out_fmt, IMAGE_SMALL_WIDTH, IMAGE_SMALL_HEIGHT, 0, NULL, &error);
					image_in = clCreateImage2D(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, &image_in_fmt, IMAGE_SMALL_WIDTH, IMAGE_SMALL_HEIGHT, 0, img_in_data, &error);

					//
					// create the CL_UNORM_INT8 input and output buffers
					// 
					cl_mem nimage_in, nimage_out, nimage_out_large;
					cl_image_format nimage_in_fmt, nimage_out_fmt;
					cl_uchar nimg_in_data[IMAGE_SMALL_HEIGHT * IMAGE_SMALL_WIDTH * sizeof(int)];

					nimage_out_fmt.image_channel_data_type = CL_UNORM_INT8;
					nimage_out_fmt.image_channel_order = CL_RGBA;

					nimage_in_fmt.image_channel_data_type = CL_UNORM_INT8;
					nimage_in_fmt.image_channel_order = CL_RGBA;

					// write a test pattern into the input image
					memset(nimg_in_data, 0x0A5, sizeof(nimg_in_data));

					nimage_out_large = clCreateImage2D(context, CL_MEM_WRITE_ONLY, &nimage_out_fmt, IMAGE_LARGE_WIDTH, IMAGE_LARGE_HEIGHT, 0, NULL, &error);
					nimage_out = clCreateImage2D(context, CL_MEM_WRITE_ONLY, &nimage_out_fmt, IMAGE_SMALL_WIDTH, IMAGE_SMALL_HEIGHT, 0, NULL, &error);
					nimage_in = clCreateImage2D(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, &nimage_in_fmt, IMAGE_SMALL_WIDTH, IMAGE_SMALL_HEIGHT, 0, nimg_in_data, &error);

					//
					// create the FRACTAL CL_UNORM_INT8 input and output buffers
					// 
					cl_mem image_mand;
					cl_image_format image_mand_fmt;
					const size_t IMAGE_MAND_SIZE = 640;

					image_mand_fmt.image_channel_data_type = CL_UNORM_INT8;
					image_mand_fmt.image_channel_order = CL_RGBA;

					image_mand = clCreateImage2D(context, CL_MEM_WRITE_ONLY, &image_mand_fmt, IMAGE_MAND_SIZE, IMAGE_MAND_SIZE, 0, NULL, &error);

					for (d = 0; d < num_devices; ++d)
					{
						if (d == 1)
							break;

						char buf[128];
						int k = 0; // which kernel
						wsprintf(buf, "Device %d", d);
						image_output.appendLine(buf);
						// create a command queue for the currently-selected device
						cl_command_queue cq = clCreateCommandQueue(context, devices[d], 0, &error);
						if (error == CL_SUCCESS)
						{

							//
							// Fill an image buffer (on the device) and transfer its contents to the host
							//
							k = 0;
							clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
							wsprintf(buf, "Kernel: %s...", function_name);
							image_output.appendLine(buf);
							char imgbuf[IMAGE_LARGE_HEIGHT * IMAGE_LARGE_WIDTH * sizeof(int)];

							error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &image_out);
							if (error == CL_SUCCESS)
							{
								size_t global_size[2] = { IMAGE_SMALL_WIDTH, IMAGE_SMALL_HEIGHT };
								error = clEnqueueNDRangeKernel(cq, kernels[k], 2, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									image_output.appendLine("Success!");
									size_t origin[3] = { 0, 0 };
									size_t region[3] = { IMAGE_SMALL_WIDTH, IMAGE_SMALL_HEIGHT, 1 };
									error = clEnqueueReadImage(cq, image_out, CL_TRUE, origin, region, 0, 0, (void*)imgbuf, 0, NULL, NULL);
									if (error == CL_SUCCESS)
									{
										image_output.appendLine("Image retrieved successfully");
									}
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									image_output.appendLine(buf);
								}
							}

							//
							// copy an image
							//
							k = 1;
							clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
							wsprintf(buf, "Kernel: %s...", function_name);
							image_output.appendLine(buf);

							error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &image_out);
							error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &image_in);
							if (error == CL_SUCCESS)
							{
								size_t global_size[2] = { IMAGE_SMALL_WIDTH, IMAGE_SMALL_HEIGHT };
								error = clEnqueueNDRangeKernel(cq, kernels[k], 2, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									image_output.appendLine("Success!");
									size_t origin[3] = { 0, 0 };
									size_t region[3] = { IMAGE_SMALL_WIDTH, IMAGE_SMALL_HEIGHT, 1 };
									error = clEnqueueReadImage(cq, image_out, CL_TRUE, origin, region, 0, 0, (void*)imgbuf, 0, NULL, NULL);
									if (error == CL_SUCCESS)
									{
										image_output.appendLine("Image retrieved successfully");
									}
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									image_output.appendLine(buf);
								}
							}

							//
							// scale an image with bilinear interpolation
							//
							k = 2;
							unsigned char nimgbuf[IMAGE_LARGE_HEIGHT * IMAGE_LARGE_WIDTH * sizeof(int)];
							clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
							wsprintf(buf, "Kernel: %s...", function_name);
							image_output.appendLine(buf);

							// put a test pattern into the source image
							for (int x = 0; x < sizeof(nimg_in_data) / sizeof(nimg_in_data[0]); ++x)
							{
								nimg_in_data[x] = (x % 127) * 2;
							}
							size_t copy_origin[3] = { 0, 0, 0 };
							size_t copy_region[3] = { IMAGE_SMALL_WIDTH, IMAGE_SMALL_HEIGHT, 1 };
							error = clEnqueueWriteImage(cq, nimage_in, CL_TRUE, copy_origin, copy_region, 0, 0, nimg_in_data, 0, NULL, NULL);
							memset(nimg_in_data, 0, sizeof(nimg_in_data));
							error = clEnqueueReadImage(cq, nimage_in,  CL_TRUE, copy_origin, copy_region, 0, 0, nimg_in_data, 0, NULL, NULL);

							error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &nimage_out_large);
							error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &nimage_in);
							if (error == CL_SUCCESS)
							{
								size_t global_size[2] = { IMAGE_LARGE_WIDTH, IMAGE_LARGE_HEIGHT };
								error = clEnqueueNDRangeKernel(cq, kernels[k], 2, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									image_output.appendLine("Success!");
									size_t origin[3] = { 0, 0 };
									size_t region[3] = { IMAGE_LARGE_WIDTH, IMAGE_LARGE_HEIGHT, 1 };
									memset(nimgbuf, 0, sizeof(nimgbuf));
									error = clEnqueueReadImage(cq, nimage_out_large, CL_TRUE, origin, region, 0, 0, (void*)nimgbuf, 0, NULL, NULL);
									if (error == CL_SUCCESS)
									{
										image_output.appendLine("Image retrieved successfully");
									}
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									image_output.appendLine(buf);
								}
							}
							

							//
							// generate a mandelbrot set
							//
							k = 3;
							clGetKernelInfo(kernels[k], CL_KERNEL_FUNCTION_NAME, sizeof(function_name), function_name, &ret_size);
							wsprintf(buf, "Kernel: %s...", function_name);
							image_output.appendLine(buf);

							image_img.InitializeBitmap(hWnd, IMAGE_MAND_SIZE, IMAGE_MAND_SIZE);
							image_img.Clear(BGR(255, 0, 0));
							unsigned char *image_mand_buf = (unsigned char*)image_img.GetBuffer();

							error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &image_mand);
							if (error == CL_SUCCESS)
							{
								size_t global_size[2] = { IMAGE_MAND_SIZE, IMAGE_MAND_SIZE };
								error = clEnqueueNDRangeKernel(cq, kernels[k], 2, NULL, global_size, NULL, 0, NULL, NULL);
								if (error == CL_SUCCESS)
								{
									image_output.appendLine("Success!");
									size_t origin[3] = { 0, 0 };
									size_t region[3] = { IMAGE_MAND_SIZE, IMAGE_MAND_SIZE, 1 };
									error = clEnqueueReadImage(cq, image_mand, CL_TRUE, origin, region, 0, 0, (void*)image_mand_buf, 0, NULL, NULL);
									if (error == CL_SUCCESS)
									{
										image_output.appendLine("Image retrieved successfully");
									}
								}
								else
								{
									wsprintf(buf, "Error: %d running kernel (clEnqueueTask).", error);
									image_output.appendLine(buf);
								}
							}
							
							
							clReleaseCommandQueue(cq);
						}
					}

					clReleaseMemObject(image_out);

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
