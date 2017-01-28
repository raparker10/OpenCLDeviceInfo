
bool event_init(HWND hWnd, int iPlatformIndex, int iDeviceIndex);
bool event_update(HWND hWnd, float a, float b);
bool event_cleanup(HWND hWnd);

extern TOutput event_output;
extern TCanvas event_mand;
extern TCanvas event_julia;

extern float mand_x1, mand_y1, mand_x2, mand_y2;

extern cl_ulong event_calc_duration;
extern cl_ulong event_transfer_duration;
