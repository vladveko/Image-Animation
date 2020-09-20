#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#pragma comment(lib, "gdi32.lib")

#define WIDTH 800
#define HEIGHT 600
#define RECT_WIDTH 50
#define TIMER_ID 1001

// Global variables

// The main window class name.
static TCHAR szWindowClass[] = _T("DesktopApp");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("Windows Desktop App");

static TCHAR szImageName[] = _T("LogoImage");

HINSTANCE hInst;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int x = 0;
int y = 0;
int xDelta = 5;
int yDelta = 5;

bool btnDown = FALSE;

void OnPaint(HWND hWnd, WPARAM wParam) {
	PAINTSTRUCT ps;
	RECT r;
	if (!GetClientRect(hWnd, &r)) 
		GetLastError();

	HDC hdc = BeginPaint(hWnd, &ps);
	HDC  memDC = CreateCompatibleDC(hdc);
	HBITMAP hBM = CreateCompatibleBitmap(hdc, r.right, r.bottom);
	HBITMAP mOldBmp = (HBITMAP)SelectObject(memDC, hBM);

	Gdiplus::Graphics graphics(memDC);
	Gdiplus::Image image(L"D:\\Semester 5\\OSaSP\\LabProject1\\logo.png");
	Gdiplus::Rect destRect(x, y, RECT_WIDTH, RECT_WIDTH);
	graphics.DrawImage(&image, destRect);

	BitBlt(hdc, 0, 0, r.right, r.bottom, memDC, 0, 0, SRCCOPY);
	SelectObject(memDC, mOldBmp);
	DeleteObject(hBM);
	DeleteDC(memDC);

	EndPaint(hWnd, &ps);
}

void MoveImage(HWND hWnd, int xStep, int yStep) {
	RECT r;
	GetClientRect(hWnd, &r);

	if (x + xStep > 0 && x + xStep + RECT_WIDTH < r.right)
		x += xStep;

	if (y + yStep > 0 && y + yStep + RECT_WIDTH < r.bottom)
		y += yStep;

	InvalidateRect(hWnd, NULL, TRUE);
}

void StopTimer(HWND hWnd) {
	KillTimer(hWnd, TIMER_ID);
}

void StartTimer(HWND hWnd) {
	SetTimer(hWnd, TIMER_ID, 10, NULL);
}

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Windows Desktop App"),
			NULL);

		return 1;
	}

	// Store instance handle in our global variable
	hInst = hInstance;

	// The parameters to CreateWindow explained:
	// szWindowClass: the name of the application
	// szTitle: the text that appears in the title bar
	// WS_OVERLAPPEDWINDOW: the type of window to create
	// CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
	// 500, 100: initial size (width, length)
	// NULL: the parent of this window
	// NULL: this application does not have a menu bar
	// hInstance: the first parameter from WinMain
	// NULL: not used in this application
	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		WIDTH, HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Windows Desktop App"),
			NULL);

		return 1;
	}

	// The parameters to ShowWindow explained:
	// hWnd: the value returned from CreateWindow
	// nCmdShow: the fourth parameter from WinMain
	ShowWindow(hWnd,
		nCmdShow);
	UpdateWindow(hWnd);

	// Main message loop:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Gdiplus::GdiplusShutdown(gdiplusToken);
	return (int)msg.wParam;
}

//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT r;
	short delta, fwkey;
	int mouseX, mouseY;

	switch (message)
	{
	case WM_CREATE:
		StartTimer(hWnd);
		break;

	case WM_KEYDOWN:	
		switch (wParam) {
		case VK_LEFT:
			StopTimer(hWnd);
			MoveImage(hWnd, -5, 0);
			break;

		case VK_RIGHT:
			StopTimer(hWnd);
			MoveImage(hWnd, 5, 0);
			break;

		case VK_UP:
			StopTimer(hWnd);
			MoveImage(hWnd, 0, -5);
			break;

		case VK_DOWN:
			StopTimer(hWnd);
			MoveImage(hWnd, 0, 5);
			break;
		
		case VK_SPACE:
			StartTimer(hWnd);
			break;
		}

		break;

	case WM_LBUTTONDOWN:
		mouseX = GET_X_LPARAM(lParam);
		mouseY = GET_Y_LPARAM(lParam);
		
		if (mouseX >= x - RECT_WIDTH && mouseX <= x + RECT_WIDTH &&
			mouseY >= y - RECT_WIDTH && mouseY <= y + RECT_WIDTH)
		{
			btnDown = TRUE;
			StopTimer(hWnd);
		}
		break;

	case WM_MOUSEMOVE:
		if (btnDown) {
			x = GET_X_LPARAM(lParam);
			y = GET_Y_LPARAM(lParam);

			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;

	case WM_LBUTTONUP:
		btnDown = FALSE;
		StartTimer(hWnd);
		break;

	case WM_MOUSEWHEEL:
		StopTimer(hWnd);
		delta = GET_WHEEL_DELTA_WPARAM(wParam);
		fwkey = GET_KEYSTATE_WPARAM(wParam);

		if (delta > 0)	
			if (fwkey == MK_SHIFT)
				MoveImage(hWnd, 5, 0);
			else
				MoveImage(hWnd, 0, -5);

		else if (delta < 0)
			if (fwkey == MK_SHIFT)
				MoveImage(hWnd, -5, 0);
			else
				MoveImage(hWnd, 0, 5);

		break;

	case WM_TIMER:
		RECT r;
		GetClientRect(hWnd, &r);

		if (wParam == TIMER_ID && btnDown == FALSE) {
			if (x + RECT_WIDTH > r.right || x < 0)
				xDelta = -xDelta;

			if (y + RECT_WIDTH > r.bottom || y < 0)
				yDelta = -yDelta;

			x += xDelta;
			y += yDelta;

			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;

	case WM_PAINT:
		// Here your application is laid out.
		// For this introduction, we just print out "Hello, Windows desktop!"
		// in the top left corner.
		OnPaint(hWnd, wParam);
		// End application-specific layout section.
		break;

	case WM_DESTROY:
		KillTimer(hWnd, TIMER_ID);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}