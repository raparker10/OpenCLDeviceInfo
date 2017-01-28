#include "stdafx.h"


// const int MAX_PLATFORMS = 32;
int num_platforms;
cl_platform_id platforms[MAX_PLATFORMS];
char platform_name[MAX_PLATFORMS][1024];

// platform devices
// const int MAX_DEVICES = 32;
cl_device_id devices[MAX_DEVICES]; // device IDs
char device_name[MAX_DEVICES][1024];
cl_uint num_devices;


struct TCLInfo clPlatformInfo[] {
	{"CL_PLATFORM_PROFILE", CL_PLATFORM_PROFILE, id_char_array, { 0 }, 0, "" },
	{ "CL_PLATFORM_VERSION", CL_PLATFORM_VERSION, id_char_array, { 0 }, 0, "" },
	{ "CL_PLATFORM_NAME", CL_PLATFORM_NAME, id_char_array, { 0 }, 0, "" },
	{ "CL_PLATFORM_VENDOR", CL_PLATFORM_VENDOR, id_char_array, { 0 }, 0, "" },
	{ "CL_PLATFORM_EXTENSIONS", CL_PLATFORM_EXTENSIONS, id_char_array, { 0 }, 0, "" },
	{ "CL_PLATFORM_HOST_TIMER_RESOLUTION", CL_PLATFORM_HOST_TIMER_RESOLUTION, id_cl_ulong, { 0 }, 0, "" },
};
int number_platform_info = sizeof(clPlatformInfo) / sizeof(clPlatformInfo[0]);

TCLInfo *find_platform_info(const int id)
{
	for (int i = 0; i < number_platform_info; ++i)
		if (clPlatformInfo[i].cl_device_info == id)
			return &clPlatformInfo[i];
	return NULL;
}
// returns number of platforms
int get_platforms(void)
{
	cl_int err;
	cl_uint num_platforms;
	unsigned int u;

	err = clGetPlatformIDs(1, NULL, &num_platforms);
	if (err < 0)
		return 0;

	err = clGetPlatformIDs(num_platforms, platforms, NULL);

	// get the platform names
	for (u = 0; u < num_platforms; ++u)
		clGetPlatformInfo(platforms[u], CL_PLATFORM_NAME, 1024, platform_name[u], NULL);

	return num_platforms;
}
void get_cl_platform_info(int iPlatform)
{
	// get the platform IDs
	for (int i = 0; i < number_platform_info; ++i)
	{
		clGetPlatformInfo(platforms[iPlatform], clPlatformInfo[i].cl_device_info, sizeof(clPlatformInfo[i].val), (void*)&clPlatformInfo[i].val, &clPlatformInfo[i].size_ret);
		decode_ocl_type(clPlatformInfo[i].type_id, clPlatformInfo[i].size_ret, clPlatformInfo[i].val, clPlatformInfo[i].s, &clPlatformInfo[i].num_lines);
	}
	//
	// parse details from certain fields that are represented by delimiter-separated lists
	//

	TCLInfo *ptr;
	char *token;
	int num_lines;
	size_t size;
	unsigned int u;
	cl_int error;

	ptr = find_platform_info(CL_PLATFORM_EXTENSIONS);
	if (ptr != NULL)
	{
		num_lines = 1;
		strcat(ptr->s, " ");
		while (token = strchr(ptr->s, ' '))
		{
			*token = '\n';
			++num_lines;
		}
		ptr->num_lines = num_lines;
	}

	// get the device information for the specified platform
	clGetDeviceIDs(platforms[iPlatform], CL_DEVICE_TYPE_ALL, sizeof(devices) / sizeof(devices[0]), devices, &num_devices);
	// get the device names so that they can be added to the menu
	for (u = 0; u < num_devices; ++u)
	{
		error = clGetDeviceInfo(devices[u], CL_DEVICE_NAME, sizeof(device_name[0]), device_name[u], &size);
	}
}

