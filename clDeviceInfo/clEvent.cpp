#include "stdafx.h"

static char *event_program_test[1];
static size_t event_program_length[1];

TOutput event_output;

static const size_t MAND_SIZE = 64;
static const size_t JULIA_SIZE = 1024;
TCanvas event_mand(MAND_SIZE, MAND_SIZE);
TCanvas event_julia(JULIA_SIZE, JULIA_SIZE);
unsigned char *image_mand_buf;
unsigned char *image_julia_buf;

static cl_int error;
static cl_context context;
static const int MAX_KERNELS = 32;
static cl_kernel kernels[MAX_KERNELS];
static cl_uint num_kernels_ret;

static int platform_index;
static int device_index;

__declspec(align(64)) static cl_mem mand_img, julia_img, mand_range_mem;

__declspec(align(64)) static cl_program program;
__declspec(align(64)) static cl_command_queue cq;

float mand_x1, mand_y1, mand_x2, mand_y2;
__declspec(align(64)) static cl_float4 mand_range[1]; // x1, y1, x2, y2
__declspec(align(64)) static cl_float4 julia_range[1]; // x1, y1, x2, y2
__declspec(align(64)) static cl_float2 julia_point[1]; // (a, b)
__declspec(align(64)) static cl_float2 julia_dadb[1]; // (a, b)
__declspec(align(64)) static cl_mem julia_range_buf, julia_point_buf, julia_dadb_buf;

cl_ulong event_calc_duration;
cl_ulong event_transfer_duration;

static bool bProgramReady = false;


bool event_init(HWND hWnd, int iPlatformIndex, int iDeviceIndex)
{

	bProgramReady = false; // indicate that the program is not ready

	platform_index = iPlatformIndex;
	device_index = iDeviceIndex;

	event_output.Clear();
	event_output.appendLine("Starting Event processing test:");

	// load the program
	event_output.appendLine("Loading Event program...");
	char szProgramPath[_MAX_PATH + 1];
	static char *szProgramName = "test_event.cl";
	if (false == make_program_path(szProgramPath, _countof(szProgramPath), szProgramName))
		return false;
	event_output.appendLine(szProgramPath);

	if (false == load_program(szProgramPath, &event_program_test[0], &event_program_length[0]))
	{
		char szBuf[128];
		sprintf_s(szBuf, _countof(szBuf), "Unable to load \"%s\" from file.", szProgramName);
		event_output.appendLine(szBuf);
		return false;
	}


	//
	// create a kernel for ALL devices
	//
	event_output.appendLine("Starting Kernel creation on DEFAULT Device..");


	char function_name[128];
	char attributes[128];
	cl_uint number_args;

	cl_int build_error;


	// create a Context for the Program
	context = clCreateContext(NULL, 1, &devices[device_index], NULL, NULL, &error);
	if (error == CL_SUCCESS)
	{
		program = clCreateProgramWithSource(context, 1, (const char **)event_program_test, event_program_length, &error);
		if (error == CL_SUCCESS)
		{
			build_error = clBuildProgram(program, 1, &devices[device_index], NULL, NULL, NULL);

			// get the build log for EACH DEVICE
			char build_log[10240];
			size_t size_ret;

			// get the device name
			clGetDeviceInfo(devices[device_index], CL_DEVICE_NAME, sizeof(build_log), build_log, &size_ret);
			event_output.appendLine(build_log);

			// get the build log
			error = clGetProgramBuildInfo(program, devices[device_index], CL_PROGRAM_BUILD_LOG, 0, NULL, &size_ret);
			if (error == CL_SUCCESS){
				char *ptr = (char*)calloc(size_ret + 1, 1);
				if (ptr != NULL)
				{
					error = clGetProgramBuildInfo(program, devices[device_index], CL_PROGRAM_BUILD_LOG, size_ret, ptr, &size_ret);
					if (size_ret > 0)
					{
						event_output.appendMultiLine(ptr);
					}
					free(ptr);
				}
			}
			if (error != CL_SUCCESS)
				return false;

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

					event_mand.InitializeBitmap(hWnd, MAND_SIZE, MAND_SIZE);
					event_mand.Clear(BGR(0, 0, 0));
					image_mand_buf = (unsigned char*)event_mand.GetBuffer();

					cl_image_format mand_img_fmt;
					mand_img_fmt.image_channel_data_type = CL_UNORM_INT8;
					mand_img_fmt.image_channel_order = CL_RGBA;


					mand_img = clCreateImage2D(context, CL_MEM_WRITE_ONLY, &mand_img_fmt, MAND_SIZE, MAND_SIZE, 0, NULL, &error);
					mand_range_mem = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(mand_range), NULL, &error);

					//
					// setup julia set buffers
					//
					event_julia.InitializeBitmap(hWnd, JULIA_SIZE, JULIA_SIZE);
					event_julia.Clear(BGR(0, 0, 0));
					image_julia_buf = (unsigned char*)event_julia.GetBuffer();

					julia_img = clCreateImage2D(context, CL_MEM_WRITE_ONLY, &mand_img_fmt, JULIA_SIZE, JULIA_SIZE, 0, NULL, &error);
					julia_range_buf = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(julia_range), NULL, &error);
					julia_point_buf = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(julia_point), NULL, &error);
					julia_dadb_buf = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(julia_dadb), NULL, &error);



					//
					// creat the command queue
					//
					cq = clCreateCommandQueue(context, devices[device_index], CL_QUEUE_PROFILING_ENABLE, &error);
					if (error == CL_SUCCESS)
					{

						//
						// setup the kernel arguments
						//

						int k = 0; // mandelbrot kernel
						error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &mand_img);
						error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &mand_range_mem);

						k = 1; // julia kernel
						error = clSetKernelArg(kernels[k], 0, sizeof(cl_mem), &julia_img);
						error = clSetKernelArg(kernels[k], 1, sizeof(cl_mem), &julia_range_buf);
						error = clSetKernelArg(kernels[k], 2, sizeof(cl_mem), &julia_point_buf);
						error = clSetKernelArg(kernels[k], 3, sizeof(cl_mem), &julia_dadb_buf);

						//
						// generate a mandelbrot set for context
						//

						// fill with blue for now
						event_mand.Clear(RGB(0, 0, 255));
						k = 0;
						size_t global_size[2] = { MAND_SIZE, MAND_SIZE };

						mand_range[0].s0 = mand_x1;
						mand_range[0].s1 = mand_y1;
						mand_range[0].s2 = mand_x2;
						mand_range[0].s3 = mand_y2;
						error = clEnqueueWriteBuffer(cq, mand_range_mem, CL_TRUE, 0, sizeof(mand_range), mand_range, 0, NULL, NULL);
						error = clEnqueueNDRangeKernel(cq, kernels[k], 2, NULL, global_size, NULL, 0, NULL, NULL);
						if (error == CL_SUCCESS)
						{
							size_t origin[3] = { 0, 0 };
							size_t region[3] = { MAND_SIZE, MAND_SIZE, 1 };

							error = clEnqueueReadImage(cq, mand_img, CL_TRUE, origin, region, 0, 0, (void*)image_mand_buf, 0, NULL, NULL);
						}


						// fill with julia set image
						event_julia.Clear(RGB(255, 0, 0));

						// initialize event duiration  information
						event_calc_duration = event_transfer_duration = 0;

						bProgramReady = true;
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool event_cleanup(HWND hWnd)
{
	// only proceed if there is a valid program
	if (bProgramReady == false)
		return false;

	clReleaseCommandQueue(cq);
	clReleaseMemObject(mand_img);
	clReleaseMemObject(julia_img);

	// release each kernel
	for (int i = 0; i < num_kernels_ret; ++i)
	{
		clReleaseKernel(kernels[i]);
	}

	// release the program
	clReleaseProgram(program);

	// release the context
	clReleaseContext(context);

	event_mand.FreeCanvas();
	event_julia.FreeCanvas();

	return true;
}

bool event_update(HWND hWnd, float a, float b)
{
	cl_int error;
	int k;

	// only proceed if there is a valid program
	if (bProgramReady == false)
		return false;

	//
	// execute the kernal on the default device
	//

	size_t ret_size;

	//
	// generate a julia set
	//
	k = 1;

	__declspec(align(64)) size_t global_size[2] = { JULIA_SIZE, JULIA_SIZE };

	julia_point[0].s0 = a;
	julia_point[0].s1 = b;

	julia_range[0].s0 = -2.0;
	julia_range[0].s1 = -2.0;
	julia_range[0].s2 = 2.0;
	julia_range[0].s3 = 2.0;

	julia_dadb[0].s0 = (julia_range[0].s2 - julia_range[0].s0) / JULIA_SIZE;
	julia_dadb[0].s1 = (julia_range[0].s3 - julia_range[0].s1) / JULIA_SIZE;

	cl_ulong event_enqueue_time;
	cl_ulong event_submit_time;
	cl_ulong event_start_time;
	cl_ulong event_end_time;

	cl_event timing_event;
	
	error = clEnqueueWriteBuffer(cq, julia_range_buf, CL_TRUE, 0, sizeof(julia_range), julia_range, 0, NULL, NULL);
	error = clEnqueueWriteBuffer(cq, julia_point_buf, CL_TRUE, 0, sizeof(julia_point), julia_point, 0, NULL, NULL);
	error = clEnqueueWriteBuffer(cq, julia_dadb_buf, CL_TRUE, 0, sizeof(julia_dadb), julia_dadb, 0, NULL, NULL);
	error = clEnqueueNDRangeKernel(cq, kernels[k], 2, NULL, global_size, NULL, 0, NULL, &timing_event);
	clWaitForEvents(1, &timing_event);
	if (error == CL_SUCCESS)
	{
		event_output.appendLine("Success!");

		// get the timing information
		clGetEventProfilingInfo(timing_event, CL_PROFILING_COMMAND_QUEUED, sizeof(event_enqueue_time), &event_enqueue_time, NULL);
		clGetEventProfilingInfo(timing_event, CL_PROFILING_COMMAND_SUBMIT, sizeof(event_submit_time), &event_submit_time, NULL);
		clGetEventProfilingInfo(timing_event, CL_PROFILING_COMMAND_START, sizeof(event_start_time), &event_start_time, NULL);
		clGetEventProfilingInfo(timing_event, CL_PROFILING_COMMAND_END, sizeof(event_end_time), &event_end_time, NULL);
		event_calc_duration = event_end_time - event_enqueue_time;

		size_t origin[3] = { 0, 0 };
		size_t region[3] = { JULIA_SIZE, JULIA_SIZE, 1 };
					
		error = clEnqueueReadImage(cq, julia_img, CL_TRUE, origin, region, 0, 0, (void*)image_julia_buf, 0, NULL, &timing_event);
		// get the timing information
		clGetEventProfilingInfo(timing_event, CL_PROFILING_COMMAND_QUEUED, sizeof(event_enqueue_time), &event_enqueue_time, NULL);
		clGetEventProfilingInfo(timing_event, CL_PROFILING_COMMAND_SUBMIT, sizeof(event_submit_time), &event_submit_time, NULL);
		clGetEventProfilingInfo(timing_event, CL_PROFILING_COMMAND_START, sizeof(event_start_time), &event_start_time, NULL);
		clGetEventProfilingInfo(timing_event, CL_PROFILING_COMMAND_END, sizeof(event_end_time), &event_end_time, NULL);
		event_transfer_duration = event_end_time - event_enqueue_time;


	}
	return false;
}
