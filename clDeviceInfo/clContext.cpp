#include "stdafx.h"


struct TCLInfo clContextInfo[] {
	{"CL_CONTEXT_REFERENCE_COUNT", CL_CONTEXT_REFERENCE_COUNT, id_cl_uint, { 0 }, 0, "" },
	{ "CL_CONTEXT_DEVICES", CL_CONTEXT_DEVICES, id_cl_device_id_array, { 0 }, 0, "" },
	{ "CL_CONTEXT_PROPERTIES", CL_CONTEXT_PROPERTIES, id_cl_context_properties_array, { 0 }, 0, "" },
	{ "CL_CONTEXT_NUM_DEVICES", CL_CONTEXT_NUM_DEVICES, id_cl_uint, { 0 }, 0, "" },
	//	{ "CL_CONTEXT_PLATFORM", CL_CONTEXT_PLATFORM, id_char_array, { 0 }, 0, "" },
	//	{ "CL_CONTEXT_INTEROP_USER_SYNC", CL_CONTEXT_INTEROP_USER_SYNC, id_char_array, { 0 }, 0, "" },
};
int number_context_info = sizeof(clContextInfo) / sizeof(clContextInfo[0]);

struct TCLInfo clContextFromTypeInfo[] {
	{"CL_CONTEXT_REFERENCE_COUNT", CL_CONTEXT_REFERENCE_COUNT, id_cl_uint, { 0 }, 0, "" },
	{ "CL_CONTEXT_DEVICES", CL_CONTEXT_DEVICES, id_cl_device_id_array, { 0 }, 0, "" },
	{ "CL_CONTEXT_PROPERTIES", CL_CONTEXT_PROPERTIES, id_cl_context_properties_array, { 0 }, 0, "" },
	{ "CL_CONTEXT_NUM_DEVICES", CL_CONTEXT_NUM_DEVICES, id_cl_uint, { 0 }, 0, "" },
	//	{ "CL_CONTEXT_PLATFORM", CL_CONTEXT_PLATFORM, id_char_array, { 0 }, 0, "" },
	//	{ "CL_CONTEXT_INTEROP_USER_SYNC", CL_CONTEXT_INTEROP_USER_SYNC, id_char_array, { 0 }, 0, "" },
};
int number_context_from_type_info = sizeof(clContextFromTypeInfo) / sizeof(clContextFromTypeInfo[0]);

TOutput context_output;


//
// Context tests
//

void test_contexts(int iPlatformIndex, int iDeviceIndex)
{
	cl_context context;
	cl_int error;
	cl_uint num_devices;
	size_t size_returned;
	cl_device_id device_ids[MAX_DEVICES];
	cl_device_type device_types[MAX_DEVICES];
	int i;
	char buf[128];

	context_output.appendLine("Starting Context tests...");

	//
	// create a context with an explicitly identified device
	//
	context_output.appendLine("Creating a Context with an explicitly identified device...");
	context = clCreateContext(NULL, 1, &devices[iDeviceIndex], NULL, NULL, &error);
	if (error == CL_SUCCESS)
	{
		context_output.appendLine("SUCCESS");
		// get the number of devices
		clGetContextInfo(context, CL_CONTEXT_NUM_DEVICES, sizeof(num_devices), &num_devices, &size_returned);
		clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(device_ids), device_ids, &size_returned);
		wsprintf(buf, "Number devices: %d", num_devices);
		context_output.appendLine(buf);
		clReleaseContext(context);
	}
	else
	{
		wsprintf(buf, "Error %d", error);
		context_output.appendLine(buf);
	}


	//
	// create a context with all devices
	//
	context_output.appendLine("Creating a Context with all devices...");
	context = clCreateContext(NULL, ::num_devices, ::devices, NULL, NULL, &error);
	if (error == CL_SUCCESS)
	{
		context_output.appendLine("SUCCESS");
		// get the number of devices
		clGetContextInfo(context, CL_CONTEXT_NUM_DEVICES, sizeof(num_devices), &num_devices, &size_returned);
		clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(device_ids), device_ids, &size_returned);
		wsprintf(buf, "Number devices: %d", num_devices);
		context_output.appendLine(buf);
		clReleaseContext(context);
	}
	else
	{
		wsprintf(buf, "Error %d", error);
		context_output.appendLine(buf);
	}

	//
	// The following examples create context from a device type, but here they always use the first platform
	//

	cl_context_properties props[3] = {
		(cl_context_properties)CL_CONTEXT_PLATFORM,
		(cl_context_properties)platforms[0],
		(cl_context_properties)0,
	};

	//
	// create a context from all CPU devices
	//
	context_output.appendLine("Creating a Context with all CPU devices...");
	context = clCreateContextFromType(props, CL_DEVICE_TYPE_CPU, NULL, NULL, &error);
	if (error == CL_SUCCESS)
	{
		context_output.appendLine("SUCCESS");
		// get the number of devices
		clGetContextInfo(context, CL_CONTEXT_NUM_DEVICES, sizeof(num_devices), &num_devices, &size_returned);
		clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(device_ids), device_ids, &size_returned);
		wsprintf(buf, "Number devices: %d", num_devices);
		context_output.appendLine(buf);
		buf[0] = 0;
		for (i = 0; i < num_devices; ++i)
		{
			char typenum[128];
			if (i > 0)
				strcat(buf, ", ");
			clGetDeviceInfo(device_ids[i], CL_DEVICE_TYPE, sizeof(device_types), &device_types[i], &size_returned);
			wsprintf(typenum, "%d", device_types[i]);
			strcat(buf, typenum);
		}
		context_output.appendLine(buf);
		clReleaseContext(context);
	}
	else
	{
		wsprintf(buf, "Error %d", error);
		context_output.appendLine(buf);
	}

	//
	// create a context from all GPU devices
	//
	context_output.appendLine("Creating a Context with all CPU devices...");
	context = clCreateContextFromType(props, CL_DEVICE_TYPE_GPU, NULL, NULL, &error);
	if (error == CL_SUCCESS)
	{
		context_output.appendLine("SUCCESS");
		// get the number of devices
		clGetContextInfo(context, CL_CONTEXT_NUM_DEVICES, sizeof(num_devices), &num_devices, &size_returned);
		clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(device_ids), device_ids, &size_returned);
		wsprintf(buf, "Number devices: %d", num_devices);
		context_output.appendLine(buf);
		buf[0] = 0;

		for (i = 0; i < num_devices; ++i)
		{
			char typenum[128];
			if (i > 0)
				strcat(buf, ", ");

			clGetDeviceInfo(device_ids[i], CL_DEVICE_TYPE, sizeof(device_types), &device_types[i], &size_returned);
			wsprintf(typenum, "%d", device_types[i]);
			strcat(buf, typenum);
		}
		context_output.appendLine(buf);

		clReleaseContext(context);
	}
	else
	{
		wsprintf(buf, "Error %d", error);
		context_output.appendLine(buf);
	}

	//
	// create a context from ALL devices
	//
	context_output.appendLine("Creating a Context with ALL devices...");
	context = clCreateContextFromType(props, CL_DEVICE_TYPE_ALL, NULL, NULL, &error);
	if (error == CL_SUCCESS)
	{
		context_output.appendLine("SUCCESS");
		// get the number of devices
		clGetContextInfo(context, CL_CONTEXT_NUM_DEVICES, sizeof(num_devices), &num_devices, &size_returned);
		clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(device_ids), device_ids, &size_returned);
		wsprintf(buf, "Number devices: %d", num_devices);
		context_output.appendLine(buf);
		buf[0] = 0;

		for (i = 0; i < num_devices; ++i)
		{
			char typenum[128];
			if (i > 0)
				strcat(buf, ", ");

			clGetDeviceInfo(device_ids[i], CL_DEVICE_TYPE, sizeof(device_types), &device_types[i], &size_returned);
			wsprintf(typenum, "%d", device_types[i]);
			strcat(buf, typenum);
		}
		context_output.appendLine(buf);

		clReleaseContext(context);
	}
	else
	{
		wsprintf(buf, "Error %d", error);
		context_output.appendLine(buf);
	}

	//
	// create a context from the DEFAULT device
	//
	context_output.appendLine("Creating a Context with the DEFAULT device...");
	context = clCreateContextFromType(props, CL_DEVICE_TYPE_DEFAULT, NULL, NULL, &error);
	if (error == CL_SUCCESS)
	{
		context_output.appendLine("SUCCESS");
		// get the number of devices
		clGetContextInfo(context, CL_CONTEXT_NUM_DEVICES, sizeof(num_devices), &num_devices, &size_returned);
		clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(device_ids), device_ids, &size_returned);
		wsprintf(buf, "Number devices: %d", num_devices);
		context_output.appendLine(buf);
		buf[0] = 0;

		for (i = 0; i < num_devices; ++i)
		{
			char typenum[128];
			if (i > 0)
				strcat(buf, ", ");

			clGetDeviceInfo(device_ids[i], CL_DEVICE_TYPE, sizeof(device_types), &device_types[i], &size_returned);
			wsprintf(typenum, "%d", device_types[i]);
			strcat(buf, typenum);

		}
		context_output.appendLine(buf);

		clReleaseContext(context);
	}
	else
	{
		wsprintf(buf, "Error %d", error);
		context_output.appendLine(buf);
	}


	//
	// create a context from the ACCELERATOR device
	//
	context_output.appendLine("Creating a Context with all ACCELERATOR devices...");
	context = clCreateContextFromType(props, CL_DEVICE_TYPE_ACCELERATOR, NULL, NULL, &error);
	if (error == CL_SUCCESS)
	{
		context_output.appendLine("SUCCESS");
		// get the number of devices
		clGetContextInfo(context, CL_CONTEXT_NUM_DEVICES, sizeof(num_devices), &num_devices, &size_returned);
		clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(device_ids), device_ids, &size_returned);
		wsprintf(buf, "Number devices: %d", num_devices);
		context_output.appendLine(buf);
		buf[0] = 0;

		for (i = 0; i < num_devices; ++i)
		{
			char typenum[128];
			if (i > 0)
				strcat(buf, ", ");

			clGetDeviceInfo(device_ids[i], CL_DEVICE_TYPE, sizeof(device_types), &device_types[i], &size_returned);
			wsprintf(typenum, "%d", device_types[i]);
			strcat(buf, typenum);

		}
		context_output.appendLine(buf);

		clReleaseContext(context);
	}
	else
	{
		wsprintf(buf, "Error %d", error);
		context_output.appendLine(buf);
	}



}