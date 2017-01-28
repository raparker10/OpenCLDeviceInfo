// platform information
const int MAX_PLATFORMS=32;
extern int num_platforms;
extern cl_platform_id platforms[MAX_PLATFORMS];
extern char platform_name[MAX_PLATFORMS][1024];
extern int current_platform_index;
extern struct TCLInfo clPlatformInfo[];
extern int number_platform_info;


// platform devices
const int MAX_DEVICES = 32;
extern cl_device_id devices[MAX_DEVICES]; // device IDs
extern char device_name[MAX_DEVICES][1024];
extern cl_uint num_devices;
