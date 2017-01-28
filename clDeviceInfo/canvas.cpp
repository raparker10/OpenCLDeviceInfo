#include "stdafx.h"


void TCanvas::InitializeVariables(void)
{
	l_hbmpCanvas = NULL;
	memset(&l_bmi, 0, sizeof(l_bmi));
	l_pbmi = NULL;
	l_ppvBits = NULL;
	iWidth = iHeight = 0;

}

void TCanvas::InitializeBitmap(HWND hWnd, size_t width, size_t height)
{
	// initialize the bitmapinfo structure
	HDC hdc = GetDC(hWnd);
	l_bmi.bmiHeader.biSize = sizeof(l_bmi.bmiHeader);
	l_bmi.bmiHeader.biWidth = width;
	l_bmi.bmiHeader.biHeight = -height;
	l_bmi.bmiHeader.biPlanes = 1;
	l_bmi.bmiHeader.biBitCount = 32;
	l_bmi.bmiHeader.biCompression = BI_RGB;
	l_bmi.bmiHeader.biSizeImage = 0;
	l_bmi.bmiHeader.biXPelsPerMeter = 0;
	l_bmi.bmiHeader.biYPelsPerMeter = 0;
	l_bmi.bmiHeader.biClrUsed = 0;
	l_bmi.bmiHeader.biClrImportant = 0;
	l_hbmpCanvas = CreateDIBSection(hdc, &l_bmi, DIB_RGB_COLORS, &l_ppvBits, NULL, 0);
	ReleaseDC(hWnd, hdc);
	iWidth = width;
	iHeight = height;
	Clear();

}
TCanvas::TCanvas()
{
	InitializeVariables();
}
TCanvas::TCanvas(size_t width, size_t height)
{
	InitializeVariables();
}
TCanvas::~TCanvas()
{
	FreeCanvas();
}
void TCanvas::SetPixel(size_t x, size_t y, unsigned int rgbaColor)
{

}
unsigned int TCanvas::GetPixel(size_t x, size_t y)
{
	return 0;
}
int TCanvas::DrawImage(HDC hdc, int x, int y)
{
	int iLines = StretchDIBits(
		hdc,
		x, y, iWidth, iHeight,
		0, 0, iWidth, iHeight,
		l_ppvBits,
		&l_bmi,
		DIB_RGB_COLORS,
		SRCCOPY);
	return iLines;
}
void TCanvas::Clear(unsigned int rgbaColor)
{
	for (int i = 0; i < iHeight * iWidth; ++i)
	{
		((unsigned int*)l_ppvBits)[i] = rgbaColor;
	}
}

void TCanvas::FreeCanvas(void)
{
	if (l_hbmpCanvas != NULL)
	{
		DeleteObject(l_hbmpCanvas);
		l_hbmpCanvas = NULL;
	}
}