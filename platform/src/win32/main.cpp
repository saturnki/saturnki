#include "stdafx.h"
#include "main.h"

#include "io_interfaces.h"
#include "..\..\..\lib\c\saturnki.h"
#include "..\..\..\src\saturn_input.h"

#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>

#include "../common/cdbase.h"

namespace impl//put everything in this namespace
{
	HANDLE hConsole;
	io::video::DirectDrawVideo video_interface;
	void VideoRefresh(const uint32_t *data, int width, int height)
	{
		uint32_t *buffer;
		unsigned pitch;
		if(impl::video_interface.Lock(buffer, pitch, width, height)) {
			for(unsigned y = 0; y < height; y++) {
				uint32_t *line = buffer + y * (pitch >> 2);
				const uint32_t *source = data + y * width;
				for(unsigned x = 0; x < width; x++) {
					uint32_t color = *source++;
					*line++ = color;
				}
			}
			impl::video_interface.Unlock();
			impl::video_interface.Refresh();
		}
	}
	void VideoUpdate(const uint32_t *data, const unsigned width, const unsigned height)
	{
		VideoRefresh(data,704,526);
	}
	void AudioUpdate(const uint16_t left, const uint16_t right)
	{
	}
	SaturnStandardPadInputType InputUpdate()
	{
		SaturnStandardPadInputType s;
		return s;
	}

	void OpenConsole() 
	{
		if (hConsole) return;
		AllocConsole();

		//redirect stdio
		long lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
		int hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
		FILE *fp = _fdopen( hConHandle, "w" );
		*stdout = *fp;
	}

	void Initialize(HWND hWnd)
	{
		OpenConsole();
		impl::video_interface.settings.handle = hWnd;
		impl::video_interface.Initialize();

		SaturnSetVideoUpdateCallback(VideoUpdate);
		SaturnSetAudioSampleUpdateCallback(AudioUpdate);
		SaturnSetInputUpdateCallback(InputUpdate);

		SaturnSetCdGetPhysicalStatusCallback(GetPhysicalStatus);
		SaturnSetCdReadTocCallback(ReadToc);
		SaturnSetCdReadSectorAtFadCallback(ReadSectorAtFad);

		CdInit("C:\\SaturnOrbit\\EXAMPLES\\CharlesMacDonald\\satwin2\\main_coff.iso");
		SaturnPressPowerButton();
	}

	const int size = 704*526;
	uint32_t data[size];

	void MainLoop()
	{
		static unsigned frameCounter = 0;
		static time_t timeCounter = time(0);

		frameCounter++;
		time_t currentTime = time(0);
		if(currentTime != timeCounter) 
		{
			timeCounter = currentTime;

			wchar_t buf[100];
			swprintf(buf,L"saturnki - fps: %d",frameCounter);
			SetWindowText(video_interface.settings.handle,buf);
			frameCounter = 0;
		}

		SaturnRunFrame();
	}
}

// saturnki.cpp : Defines the entry point for the application.
//windows api boilerplate below here

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SATURNKI, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SATURNKI));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		impl::MainLoop();

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
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SATURNKI));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_SATURNKI);
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

void SetClientSize(HWND hWnd,int width, int height)
{
	RECT window_rect,client_rect;
	GetWindowRect(hWnd,&window_rect);
	GetClientRect(hWnd,&client_rect);
	int dx=(client_rect.right-client_rect.left)-width;
	int dy=(client_rect.bottom-client_rect.top)-height;
	SetWindowPos(hWnd,0,0,0,window_rect.right-window_rect.left-dx,window_rect.bottom-window_rect.top-dy,SWP_NOZORDER|SWP_NOMOVE);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 
		CW_USEDEFAULT, 0, 
		NULL, NULL, hInstance, NULL);

	SetClientSize(hWnd,704,526);

	if(!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	impl::Initialize(hWnd);

	return TRUE;
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

	switch (message)
	{
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
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_SIZE:
//		impl::video_interface.Resize((lParam >> 16) & 0xFF, lParam & 0xFF);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
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
