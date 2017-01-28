// clDeviceInfo.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

//
// device capability functions and data
//
int get_platforms(void); // returns number of platforms
void get_cl_platform_info(int i);
// void get_cl_device_caps(int iDeviceType);
void get_cl_device_caps(int iDeviceIndex);
void display_device_caps(HWND hWnd, HDC hdc, TCLInfo *info, int iCount);
TCLInfo *find_device_info(const int id);
void display_output(HWND hWnd, HDC hdc, TOutput &context_output);

static bool bDeviceInfoAvailable = false;
static bool bPlatformInfoAvailable = true;
enum DisplayMode {
	DISPLAY_MODE_NONE,
	DISPLAY_MODE_PLATFORM,
	DISPLAY_MODE_DEVICE,
	DISPLAY_MODE_CONTEXT,
	DISPLAY_MODE_PROGRAM,
	DISPLAY_MODE_KERNEL,
	DISPLAY_MODE_BUFFER,
	DISPLAY_MODE_MATH_SCALAR,
	DISPLAY_MODE_MATH_INTEGER,
	DISPLAY_MODE_MATH_VECTOR,
	DISPLAY_MODE_IMAGE,
	DISPLAY_MODE_EVENT,
};
DisplayMode display_mode = DISPLAY_MODE_NONE;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name


int current_platform_index;
int current_device_index;

static const int TIMING_BUFFER_SIZE = 1024 * 8;
static cl_ulong timing_buffer[TIMING_BUFFER_SIZE];
int timing_buffer_index = 0;
cl_ulong avg_time;

bool bEventProgramReady = false;

void timing_buffer_insert(cl_ulong value)
{
	timing_buffer[timing_buffer_index % TIMING_BUFFER_SIZE] = value;

	avg_time = 0;
	timing_buffer_index++;
	for (int i = 0; i < min(timing_buffer_index, TIMING_BUFFER_SIZE); ++i)
	{
		avg_time += timing_buffer[i];
	}
	avg_time = avg_time / min(timing_buffer_index, TIMING_BUFFER_SIZE);
}
struct TCLInfo clDeviceInfo[] {
	{ "CL_DEVICE_TYPE", CL_DEVICE_TYPE, id_cl_device_type, { 0 }, 0, "" },
	{ "CL_DEVICE_VENDOR_ID", CL_DEVICE_VENDOR_ID, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_MAX_COMPUTE_UNITS", CL_DEVICE_MAX_COMPUTE_UNITS, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS", CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_MAX_WORK_GROUP_SIZE", CL_DEVICE_MAX_WORK_GROUP_SIZE, id_size_t, { 0 }, 0, "" },
	{ "CL_DEVICE_MAX_WORK_ITEM_SIZES", CL_DEVICE_MAX_WORK_ITEM_SIZES, id_size_t_array, { 0 }, 0, "" },
	{ "CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR", CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT", CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT", CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG", CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT", CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE", CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_MAX_CLOCK_FREQUENCY", CL_DEVICE_MAX_CLOCK_FREQUENCY, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_ADDRESS_BITS", CL_DEVICE_ADDRESS_BITS, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_MAX_READ_IMAGE_ARGS", CL_DEVICE_MAX_READ_IMAGE_ARGS, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_MAX_WRITE_IMAGE_ARGS", CL_DEVICE_MAX_WRITE_IMAGE_ARGS, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_MAX_MEM_ALLOC_SIZE", CL_DEVICE_MAX_MEM_ALLOC_SIZE, id_cl_ulong, { 0 }, 0, "" },
	{ "CL_DEVICE_IMAGE2D_MAX_WIDTH", CL_DEVICE_IMAGE2D_MAX_WIDTH, id_size_t, { 0 }, 0, "" },
	{ "CL_DEVICE_IMAGE2D_MAX_HEIGHT", CL_DEVICE_IMAGE2D_MAX_HEIGHT, id_size_t, { 0 }, 0, "" },
	{ "CL_DEVICE_IMAGE3D_MAX_WIDTH", CL_DEVICE_IMAGE3D_MAX_WIDTH, id_size_t, { 0 }, 0, "" },
	{ "CL_DEVICE_IMAGE3D_MAX_HEIGHT", CL_DEVICE_IMAGE3D_MAX_HEIGHT, id_size_t, { 0 }, 0, "" },
	{ "CL_DEVICE_IMAGE3D_MAX_DEPTH", CL_DEVICE_IMAGE3D_MAX_DEPTH, id_size_t, { 0 }, 0, "" },
	{ "CL_DEVICE_IMAGE_SUPPORT", CL_DEVICE_IMAGE_SUPPORT, id_cl_bool, { 0 }, 0, "" },
	{ "CL_DEVICE_MAX_PARAMETER_SIZE", CL_DEVICE_MAX_PARAMETER_SIZE, id_size_t, { 0 }, 0, "" },
	{ "CL_DEVICE_MAX_SAMPLERS", CL_DEVICE_MAX_SAMPLERS, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_MEM_BASE_ADDR_ALIGN", CL_DEVICE_MEM_BASE_ADDR_ALIGN, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE", CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_SINGLE_FP_CONFIG", CL_DEVICE_SINGLE_FP_CONFIG, id_cl_device_fp_config, { 0 }, 0, "" },
	{ "CL_DEVICE_GLOBAL_MEM_CACHE_TYPE", CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, id_cl_device_mem_cache_type, { 0 }, 0, "" },
	{ "CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE", CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_GLOBAL_MEM_CACHE_SIZE", CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, id_cl_ulong, { 0 }, 0, "" },
	{ "CL_DEVICE_GLOBAL_MEM_SIZE", CL_DEVICE_GLOBAL_MEM_SIZE, id_cl_ulong, { 0 }, 0, "" },
	{ "CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE", CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, id_cl_ulong, { 0 }, 0, "" },
	{ "CL_DEVICE_MAX_CONSTANT_ARGS", CL_DEVICE_MAX_CONSTANT_ARGS, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_LOCAL_MEM_TYPE", CL_DEVICE_LOCAL_MEM_TYPE, id_cl_device_local_mem_type, { 0 }, 0, "" },
	{ "CL_DEVICE_LOCAL_MEM_SIZE", CL_DEVICE_LOCAL_MEM_SIZE, id_cl_ulong, { 0 }, 0, "" },
	{ "CL_DEVICE_ERROR_CORRECTION_SUPPORT", CL_DEVICE_ERROR_CORRECTION_SUPPORT, id_cl_bool, { 0 }, 0, "" },
	{ "CL_DEVICE_PROFILING_TIMER_RESOLUTION", CL_DEVICE_PROFILING_TIMER_RESOLUTION, id_size_t, { 0 }, 0, "" },
	{ "CL_DEVICE_ENDIAN_LITTLE", CL_DEVICE_ENDIAN_LITTLE, id_cl_bool, { 0 }, 0, "" },
	{ "CL_DEVICE_AVAILABLE", CL_DEVICE_AVAILABLE, id_cl_bool, { 0 }, 0, "" },
	{ "CL_DEVICE_COMPILER_AVAILABLE", CL_DEVICE_COMPILER_AVAILABLE, id_cl_bool, { 0 }, 0, "" },
	{ "CL_DEVICE_EXECUTION_CAPABILITIES", CL_DEVICE_EXECUTION_CAPABILITIES, id_cl_device_exec_capabilities, { 0 }, 0, "" },
	{ "CL_DEVICE_QUEUE_ON_HOST_PROPERTIES", CL_DEVICE_QUEUE_ON_HOST_PROPERTIES, id_cl_command_queue_properties, { 0 }, 0, "" },
	{ "CL_DEVICE_NAME", CL_DEVICE_NAME, id_char_array, { 0 }, 0, "" },
	{ "CL_DEVICE_VENDOR", CL_DEVICE_VENDOR, id_char_array, { 0 }, 0, "" },
	{ "CL_DRIVER_VERSION", CL_DRIVER_VERSION, id_char_array, { 0 }, 0, "" },
	{ "CL_DEVICE_PROFILE", CL_DEVICE_PROFILE, id_char_array, { 0 }, 0, "" },
	{ "CL_DEVICE_VERSION", CL_DEVICE_VERSION, id_char_array, { 0 }, 0, "" },
	{ "CL_DEVICE_EXTENSIONS", CL_DEVICE_EXTENSIONS, id_char_array, { 0 }, 0, "" },
	{ "CL_DEVICE_PLATFORM", CL_DEVICE_PLATFORM, id_cl_platform_id, { 0 }, 0, "" },
	{ "CL_DEVICE_DOUBLE_FP_CONFIG", CL_DEVICE_DOUBLE_FP_CONFIG, id_cl_device_fp_config, { 0 }, 0, "" },
	/*,0x1033,reserved,for,CL_DEVICE_HALF_FP_CONFIG,*/
	{ "CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF", CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF, id_cl_uint, { 0 }, 0, "" },
	//	{CL_DEVICE_HOST_UNIFIED_MEMORY, id_cl_ulong,/*,deprecated,*/,
	{ "CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR", CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT", CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_NATIVE_VECTOR_WIDTH_INT", CL_DEVICE_NATIVE_VECTOR_WIDTH_INT, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG", CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT", CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE", CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF", CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_OPENCL_C_VERSION", CL_DEVICE_OPENCL_C_VERSION, id_char_array, { 0 }, 0, "" },
	{ "CL_DEVICE_LINKER_AVAILABLE", CL_DEVICE_LINKER_AVAILABLE, id_cl_bool, { 0 }, 0, "" },
	{ "CL_DEVICE_BUILT_IN_KERNELS", CL_DEVICE_BUILT_IN_KERNELS, id_char_array, { 0 }, 0, "" },
	{ "CL_DEVICE_IMAGE_MAX_BUFFER_SIZE", CL_DEVICE_IMAGE_MAX_BUFFER_SIZE, id_size_t, { 0 }, 0, "" },
	{ "CL_DEVICE_IMAGE_MAX_ARRAY_SIZE", CL_DEVICE_IMAGE_MAX_ARRAY_SIZE, id_size_t, { 0 }, 0, "" },
	{ "CL_DEVICE_PARENT_DEVICE", CL_DEVICE_PARENT_DEVICE, id_cl_device_id, { 0 }, 0, "" },
	{ "CL_DEVICE_PARTITION_MAX_SUB_DEVICES", CL_DEVICE_PARTITION_MAX_SUB_DEVICES, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_PARTITION_PROPERTIES", CL_DEVICE_PARTITION_PROPERTIES, id_cl_device_partition_property_array, { 0 }, 0, "" },
	{ "CL_DEVICE_PARTITION_AFFINITY_DOMAIN", CL_DEVICE_PARTITION_AFFINITY_DOMAIN, id_cl_device_affinity_domain, { 0 }, 0, "" },
	{ "CL_DEVICE_PARTITION_TYPE", CL_DEVICE_PARTITION_TYPE, id_cl_device_partition_property_array, { 0 }, 0, "" },
	{ "CL_DEVICE_REFERENCE_COUNT", CL_DEVICE_REFERENCE_COUNT, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_PREFERRED_INTEROP_USER_SYNC", CL_DEVICE_PREFERRED_INTEROP_USER_SYNC, id_cl_bool, { 0 }, 0, "" },
	{ "CL_DEVICE_PRINTF_BUFFER_SIZE", CL_DEVICE_PRINTF_BUFFER_SIZE, id_size_t, { 0 }, 0, "" },
	{ "CL_DEVICE_IMAGE_PITCH_ALIGNMENT", CL_DEVICE_IMAGE_PITCH_ALIGNMENT, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT", CL_DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_MAX_READ_WRITE_IMAGE_ARGS", CL_DEVICE_MAX_READ_WRITE_IMAGE_ARGS, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_MAX_GLOBAL_VARIABLE_SIZE", CL_DEVICE_MAX_GLOBAL_VARIABLE_SIZE, id_size_t, { 0 }, 0, "" },
	{ "CL_DEVICE_QUEUE_ON_DEVICE_PROPERTIES", CL_DEVICE_QUEUE_ON_DEVICE_PROPERTIES, id_cl_command_queue_properties, { 0 }, 0, "" },
	{ "CL_DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE", CL_DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE", CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_MAX_ON_DEVICE_QUEUES", CL_DEVICE_MAX_ON_DEVICE_QUEUES, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_MAX_ON_DEVICE_EVENTS", CL_DEVICE_MAX_ON_DEVICE_EVENTS, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_SVM_CAPABILITIES", CL_DEVICE_SVM_CAPABILITIES, id_cl_device_svm_capabilities, { 0 }, 0, "" },
	{ "CL_DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE", CL_DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE, id_size_t, { 0 }, 0, "" },
	{ "CL_DEVICE_MAX_PIPE_ARGS", CL_DEVICE_MAX_PIPE_ARGS, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS", CL_DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_PIPE_MAX_PACKET_SIZE", CL_DEVICE_PIPE_MAX_PACKET_SIZE, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_PREFERRED_PLATFORM_ATOMIC_ALIGNMENT", CL_DEVICE_PREFERRED_PLATFORM_ATOMIC_ALIGNMENT, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_PREFERRED_GLOBAL_ATOMIC_ALIGNMENT", CL_DEVICE_PREFERRED_GLOBAL_ATOMIC_ALIGNMENT, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_PREFERRED_LOCAL_ATOMIC_ALIGNMENT", CL_DEVICE_PREFERRED_LOCAL_ATOMIC_ALIGNMENT, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_IL_VERSION", CL_DEVICE_IL_VERSION, id_char_array, { 0 }, 0, "" },
	{ "CL_DEVICE_MAX_NUM_SUB_GROUPS", CL_DEVICE_MAX_NUM_SUB_GROUPS, id_cl_uint, { 0 }, 0, "" },
	{ "CL_DEVICE_SUB_GROUP_INDEPENDENT_FORWARD_PROGRESS", CL_DEVICE_SUB_GROUP_INDEPENDENT_FORWARD_PROGRESS, id_cl_bool, { 0 }, 0, "" }
};

//
// scroll control
//
static int iElements = sizeof(clDeviceInfo) / sizeof(clDeviceInfo[0]);
static int iVPosition = 0;
static int iHeightLines = 0;
static int iLineHeight = 0;

//
// Event testing variables
//

static int MAND_X = 10;
static int MAND_Y = 10;
static int JULIA_X = 200;
static int JULIA_Y = 10;

static float fMandelbrot_a1 = -2.0f;
static float fMandelbrot_b1 = -2.0f;
static float fMandelbrot_a2 = 2.0f;
static float fMandelbrot_b2 = 2.0f;
static float fMandelbrot_da = (fMandelbrot_a2 - fMandelbrot_a1);
static float fMandelbrot_db = (fMandelbrot_b2 - fMandelbrot_b1);
static int iMousePointX = 0;
static int iMousePointY = 0;
static float fMouseValueX = 0.0f;
static float fMouseValueY = 0.0f;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CLDEVICEINFO, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLDEVICEINFO));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLDEVICEINFO));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_CLDEVICEINFO);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW| WS_HSCROLL | WS_VSCROLL,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void exit_display_mode(DisplayMode oldDisplayMode)
{
	switch (oldDisplayMode)
	{
	case DISPLAY_MODE_EVENT:
		event_cleanup(NULL);
		break;
	}
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TEXTMETRIC tm;
	SCROLLINFO si;
	SIZE size;
	HMENU hMenu, hSubMenu;
	int i;
	unsigned int u;
	char buf[128];

	switch (message)
	{
	case WM_CREATE:
		hdc = GetDC(hWnd);
		GetTextMetrics(hdc, &tm);
		GetWindowExtEx(hdc, &size);
		ReleaseDC(hWnd, hdc);
		iLineHeight = tm.tmHeight;
		iHeightLines = ((LPCREATESTRUCT)(lParam))->cy / iLineHeight;

		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_POS;
		si.nMin = 0;
		si.nMax = iHeightLines;
		si.nPos = 0;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

		// get the platform names and update the program menu
		num_platforms = get_platforms();
		hMenu = GetMenu(hWnd);
		hSubMenu = GetSubMenu(hMenu, 1);
		DeleteMenu(hMenu, IDM_PLATFORM_CAPABILITIES, MF_BYCOMMAND);
		for (i = 0; i < num_platforms; ++i)
			AppendMenu(hSubMenu, MF_ENABLED | MF_STRING, IDM_PLATFORM_CAPABILITIES + i + 1, platform_name[i]);
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;

		case IDM_PLATFORM_CAPABILITIES:
			//get_cl_platform_capabilities();

			break;

		case IDM_DEVICE_CAPABILITIES + 1:
		case IDM_DEVICE_CAPABILITIES + 2:
		case IDM_DEVICE_CAPABILITIES + 3:
		case IDM_DEVICE_CAPABILITIES + 4:
		case IDM_DEVICE_CAPABILITIES + 5:
		case IDM_DEVICE_CAPABILITIES + 6:
		case IDM_DEVICE_CAPABILITIES + 7:
		case IDM_DEVICE_CAPABILITIES + 8:
			current_device_index = wmId - IDM_DEVICE_CAPABILITIES - 1;
			get_cl_device_caps(current_device_index);
			bDeviceInfoAvailable = true;
			display_mode = DISPLAY_MODE_DEVICE;
			InvalidateRect(hWnd, NULL, TRUE);
			break;

/*		case IDM_CAPS_CPU:
			get_cl_device_caps(CL_DEVICE_TYPE_CPU);
			bDeviceInfoAvailable = true;
			display_mode = DISPLAY_MODE_DEVICE;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
*/

		case IDM_PLATFORM_CAPABILITIES + 1:
		case IDM_PLATFORM_CAPABILITIES + 2:
		case IDM_PLATFORM_CAPABILITIES + 3:
		case IDM_PLATFORM_CAPABILITIES + 4:
		case IDM_PLATFORM_CAPABILITIES + 5:
		case IDM_PLATFORM_CAPABILITIES + 6:
		case IDM_PLATFORM_CAPABILITIES + 7:
		case IDM_PLATFORM_CAPABILITIES + 8:
			// get platform information
			current_platform_index = wmId - IDM_PLATFORM_CAPABILITIES - 1;
			current_device_index = 0;
			get_cl_platform_info(current_platform_index);

			// update the menu to reflect the platform's devices
			hMenu = GetMenu(hWnd);
			hSubMenu = GetSubMenu(hMenu, 2);
			while (DeleteMenu(hSubMenu, 0, MF_BYPOSITION));
			for (u = 0; u < num_devices; ++u)
				AppendMenu(hSubMenu, MF_ENABLED | MF_STRING, IDM_DEVICE_CAPABILITIES + u + 1, device_name[u]);


			bPlatformInfoAvailable = true;
			display_mode = DISPLAY_MODE_PLATFORM;
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case IDM_TEST_CONTEXT:
			exit_display_mode(display_mode);
			test_contexts(current_platform_index, current_device_index);
			display_mode = DISPLAY_MODE_CONTEXT;
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case IDM_TEST_PROGRAM:
			exit_display_mode(display_mode);
			test_programs(current_platform_index, current_device_index);
				display_mode = DISPLAY_MODE_PROGRAM;
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case IDM_TEST_KERNEL:
			exit_display_mode(display_mode);
			test_kernels(current_platform_index, current_device_index);
			display_mode = DISPLAY_MODE_KERNEL;
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case IDM_TEST_BUFFER:
			exit_display_mode(display_mode);
			test_buffers(current_platform_index, current_device_index);
			display_mode = DISPLAY_MODE_BUFFER;
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case IDM_TEST_SCALAR_MATH:
			exit_display_mode(display_mode);
			test_math_scalar(current_platform_index, current_device_index);
			display_mode = DISPLAY_MODE_MATH_SCALAR;
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case IDM_TEST_INTEGER_MATH:
			exit_display_mode(display_mode);
			test_math_integer(current_platform_index, current_device_index);
			display_mode = DISPLAY_MODE_MATH_INTEGER;
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case IDM_TEST_VECTOR_MATH:
			exit_display_mode(display_mode);
			test_math_vector(current_platform_index, current_device_index);
			display_mode = DISPLAY_MODE_MATH_VECTOR;
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case IDM_TEST_IMAGE:
			exit_display_mode(display_mode);
			test_image(hWnd, current_platform_index, current_device_index);
			display_mode = DISPLAY_MODE_IMAGE;
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case IDM_TEST_EVENT:
			exit_display_mode(display_mode);
			mand_x1 = -2.0f, mand_y1 = -2.0f, mand_x2 = 2.0f, mand_y2 = 2.0f;

			display_mode = DISPLAY_MODE_EVENT;
			bEventProgramReady = event_init(hWnd, current_platform_index, current_device_index);
			event_update(hWnd, 0.0f, 0.0f);

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_SIZE:
		iHeightLines = HIWORD(lParam) / iLineHeight;

		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE;
		si.nMin = 0;
		si.nMax = iHeightLines;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
		break;

	case WM_VSCROLL:
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		GetScrollInfo(hWnd, SB_VERT, &si);

		switch (LOWORD(wParam))
		{
		case SB_BOTTOM:
			break;

		case SB_LINEUP:
			--iVPosition;
			break;

		case SB_LINEDOWN:
			++iVPosition;
			break;

		case SB_TOP:
			iVPosition = 0;
			break;

		case SB_PAGEUP:
			break;

		case SB_PAGEDOWN:
			break;
		}
		iVPosition = max(0, min(iVPosition, iElements - 1));
		si.nPos = iVPosition;
		si.fMask = SIF_POS;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		if (display_mode == DISPLAY_MODE_DEVICE)
		{
			if (bDeviceInfoAvailable)
				display_device_caps(hWnd, hdc, clDeviceInfo, sizeof(clDeviceInfo) / sizeof(clDeviceInfo[0]));
		}
		else if (display_mode == DISPLAY_MODE_PLATFORM)
		{
			if (bPlatformInfoAvailable)
			{
				display_device_caps(hWnd, hdc, clPlatformInfo, number_platform_info);
			}
		}
		else if (display_mode == DISPLAY_MODE_CONTEXT)
		{
			if (context_output.getCount() > 0)
			{
				display_output(hWnd, hdc, context_output);
			}
		}
		else if (display_mode == DISPLAY_MODE_PROGRAM)
		{
			if (program_output.getCount() > 0)
			{
				display_output(hWnd, hdc, program_output);
			}
		}
		else if (display_mode == DISPLAY_MODE_KERNEL)
		{
			if (kernel_output.getCount() > 0)
			{
				display_output(hWnd, hdc, kernel_output);
			}
		}
		else if (display_mode == DISPLAY_MODE_BUFFER)
		{
			if (buffer_output.getCount() > 0)
			{
				display_output(hWnd, hdc, buffer_output);
			}
		}
		else if (display_mode == DISPLAY_MODE_MATH_SCALAR)
		{
			if (math_scalar_output.getCount() > 0)
			{
				display_output(hWnd, hdc, math_scalar_output);
			}
		}
		else if (display_mode == DISPLAY_MODE_MATH_INTEGER)
		{
			if (math_integer_output.getCount() > 0)
			{
				display_output(hWnd, hdc, math_integer_output);
			}
		}
		else if (display_mode == DISPLAY_MODE_MATH_VECTOR)
		{
			if (math_vector_output.getCount() > 0)
			{
				display_output(hWnd, hdc, math_vector_output);
			}
		}
		else if (display_mode == DISPLAY_MODE_IMAGE)
		{
			if (image_output.getCount() > 0)
			{
				display_output(hWnd, hdc, image_output);
				image_img.DrawImage(hdc, 10, 10 - iVPosition * 32);
			}
		}
		else if (display_mode == DISPLAY_MODE_EVENT)
		{
			if (false == bEventProgramReady)
				display_output(hWnd, hdc, event_output);
			else {
				event_mand.DrawImage(hdc, MAND_X, MAND_Y);
				event_julia.DrawImage(hdc, JULIA_X, JULIA_Y);
			}
		}
		EndPaint(hWnd, &ps);
		break;

	case WM_MOUSEMOVE:
		switch (display_mode)
		{
		case DISPLAY_MODE_EVENT:
			iMousePointX = max(min(LOWORD(lParam), MAND_X + event_mand.GetWidth()), MAND_X) - MAND_X;
			iMousePointY = max(min(HIWORD(lParam), MAND_Y + event_mand.GetHeight()), MAND_Y) - MAND_Y;

			fMandelbrot_da = (fMandelbrot_a2 - fMandelbrot_a1) / event_mand.GetWidth();
			fMandelbrot_db = (fMandelbrot_b2 - fMandelbrot_b1) / event_mand.GetHeight();

			fMouseValueX = fMandelbrot_a1 + iMousePointX * fMandelbrot_da;
			fMouseValueY = fMandelbrot_b1 + iMousePointY * fMandelbrot_db;

			HDC hdc = GetDC(hWnd);
			sprintf(buf, "(%d, %d)        ", iMousePointX, iMousePointY);
			TextOut(hdc, MAND_X, MAND_Y + event_mand.GetHeight(), buf, lstrlen(buf));

			sprintf(buf, "a=%f        ", fMouseValueX);
			TextOut(hdc, MAND_X, MAND_Y + event_mand.GetHeight() + 32, buf, lstrlen(buf));
			sprintf(buf, "b=%f        ", fMouseValueY);
			TextOut(hdc, MAND_X, MAND_Y + event_mand.GetHeight() + 32 * 2, buf, lstrlen(buf));

//			event_update(hWnd, fMouseValueX, fMouseValueY);
			event_update(hWnd, 0.0f, 0.75f);

			sprintf(buf, "Calc=%lu ns        ", event_calc_duration);
			TextOut(hdc, MAND_X, MAND_Y + event_mand.GetHeight() + 32 * 3, buf, lstrlen(buf));
			sprintf(buf, "Tx=%lu ns       ", event_transfer_duration);
			TextOut(hdc, MAND_X, MAND_Y + event_mand.GetHeight() + 32 * 4, buf, lstrlen(buf));

			timing_buffer_insert(event_calc_duration);
			sprintf(buf, "avg=%lu ns        ", avg_time);
			TextOut(hdc, MAND_X, MAND_Y + event_mand.GetHeight() + 32 * 5, buf, lstrlen(buf));

			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;

	case WM_DESTROY:
		exit_display_mode(display_mode);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

TCLInfo *find_device_info(const int id)
{
	for (int i = 0; i < sizeof(clDeviceInfo) / sizeof(clDeviceInfo[0]); ++i)
		if (clDeviceInfo[i].cl_device_info == id)
			return &clDeviceInfo[i];
	return NULL;
}

void display_output(HWND hWnd, HDC hdc, TOutput &context_output)
{
	int iMaxWidth = 0;
	SIZE size;
	int i;
	int iLine = iVPosition;

	// draw the names
	for (i = iVPosition; i < context_output.getCount(); ++i)
	{
		TextOut(hdc, 5, iLineHeight * (iLine - iVPosition), context_output.getLine(i), lstrlen(context_output.getLine(i)));
		GetTextExtentPoint32(hdc, context_output.getLine(i), lstrlen(context_output.getLine(i)), &size);
		if (size.cx > iMaxWidth)
			iMaxWidth = size.cx;
		iLine++;
	}
}

void display_device_caps(HWND hWnd, HDC hdc, TCLInfo *info, int iCount)
{
	int iMaxWidth = 0;
	SIZE size;
	int i;
	int iLine = iVPosition;

	// draw the names
	for (i = iVPosition; i < iCount; ++i)
	{
		TextOut(hdc, 5, iLineHeight * (iLine - iVPosition), info[i].cl_device_info_name, lstrlen(info[i].cl_device_info_name));
		GetTextExtentPoint32(hdc, info[i].cl_device_info_name, lstrlen(info[i].cl_device_info_name), &size);
		if (size.cx > iMaxWidth)
			iMaxWidth = size.cx;
		iLine += info[i].num_lines;
	}
	// draw the device information
	iLine = iVPosition;
	for (i = iVPosition; i < iCount; ++i)
	{
		char buf[1024];
		strcpy(buf, info[i].s);
		char *next = strtok(buf, "\n");
		if (next == NULL)
			next = buf;
		for (int j = 0; j < info[i].num_lines; ++j)
		{
			TextOut(hdc, iMaxWidth + 10, iLineHeight * (iLine - iVPosition + j), next, lstrlen(next));
			if (info[i].num_lines > 1)
				next = strtok(NULL, "\n");
		}
		iLine += info[i].num_lines;
	}
}

// CL_DEVICE_TYPE_GPU
void get_cl_device_caps(int iDeviceIndex)
{
			for (int i = 0; i < sizeof(clDeviceInfo) / sizeof(clDeviceInfo[0]); ++i)
			{
				clGetDeviceInfo(devices[iDeviceIndex], clDeviceInfo[i].cl_device_info, sizeof(clDeviceInfo[i].val), (void*)&clDeviceInfo[i].val, &clDeviceInfo[i].size_ret);
				decode_ocl_type(clDeviceInfo[i].type_id, clDeviceInfo[i].size_ret, clDeviceInfo[i].val, clDeviceInfo[i].s, &clDeviceInfo[i].num_lines);
			}
//			clReleaseDevice(device);

	//
	// parse details from certain fields that are represented by delimiter-separated lists
	//
	
			TCLInfo *ptr;
	char *token;
	int num_lines;
	
	ptr = find_device_info(CL_DEVICE_EXTENSIONS);
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

	ptr = find_device_info(CL_DEVICE_BUILT_IN_KERNELS);
	if (ptr != NULL)
	{
		num_lines = 1;
		strcat(ptr->s, ";");
		while (token = strchr(ptr->s, ';'))
		{
			*token = '\n';
			++num_lines;
		}
		ptr->num_lines = num_lines;
	}
}


