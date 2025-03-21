#ifndef SCREENSHOT_S
#define SCREENSHOT_S

#include <Windows.h>
#include <opencv2/opencv.hpp>

// Main capture function, which will take a window handle 
// to get its associated context device and return a cv::Mat object
// with the appropriate information

BITMAPINFOHEADER k77tl7wchpnur67(int width, int height) { //create bitmap info header
	BITMAPINFOHEADER bi;
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height;
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	
	return bi;
}

cv::Mat pp0xkp00wx0dtih(HWND& __hwnd, //captue screen mat
	HDC& __hwindowDC, 
	HDC& __hwindowCompatibleDC,
	int __screenx,
	int __screeny,
	int __width,
	int __height) {

	cv::Mat src;

	src.create(__height, __width, CV_8UC3);

	HBITMAP hbwindow = CreateCompatibleBitmap(__hwindowDC, __width, __height);
	BITMAPINFOHEADER bi = k77tl7wchpnur67(__width, __height);

	SelectObject(__hwindowCompatibleDC, hbwindow);

	StretchBlt(__hwindowCompatibleDC, 0, 0, __width, __height, __hwindowDC, __screenx, __screeny, __width, __height, SRCCOPY);
	GetDIBits(__hwindowCompatibleDC, hbwindow, 0, __height, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	DeleteObject(hbwindow);

	return src;
}

#endif // !SCREENSHOT_S
