
#define BGR(b,g,r)          ((COLORREF)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16)))

class TCanvas {
	// drawing surface for the mandelbrot set
	HBITMAP l_hbmpCanvas;
	BITMAPINFO l_bmi;
	PBITMAPINFO l_pbmi;
	VOID *l_ppvBits;
	size_t iWidth, iHeight;

	void InitializeVariables(void);
public:
	TCanvas();
	TCanvas(size_t width, size_t height);
	~TCanvas();
	void SetPixel(size_t x, size_t y, unsigned int bgraColor);
	unsigned int GetPixel(size_t x, size_t y);
	int DrawImage(HDC hdc, int x, int y);
	void Clear(unsigned int rgbaColor=BGR(255, 0, 0));
	void FreeCanvas(void);
	void InitializeBitmap(HWND hWnd, size_t iWidth, size_t iHeight);
	VOID *GetBuffer(void) { return l_ppvBits; }
	inline size_t GetWidth(void) { return iWidth; }
	inline size_t GetHeight(void) { return iHeight; }
};

